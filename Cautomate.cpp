/*
 * Cautomate.cpp
 *
 *  Created on: 27 juil. 2011
 *      Author: philippe
 */

#include "Cautomate.h"
#include "CSBCControler.h"
#include "CsimpleState.h"
#include "CcalibrationState.h"

#include <boost/thread.hpp>
#include "Ilog.h"

/* IEVENTHANDLER_H_ */
/*! \namespace SBC
 * Namespace for Phidgets SBC cards
 */
/*! \class CSBCControler
 * \brief SBC2 Controler
 *
 *  Class usefull to manage Phidgets SBC2 cards.
 *  This class is designed to manage acces to 1 input and 3 outputs :
 *       - 1 input :
 *             PH Sensor
 *       - 3 outputs :
 *             CO2 electrovalve control
 *             Front neon tube control
 *             Rear neon tube control
 *
 *  This class is also designed to be use in a tank control system.
 */
namespace AquaAutomate
{
const int PHIDGETS_RETRY_ATTATCH = 10;
const std::string stateOff = std::string("SimpleState-Off");
const std::string stateOnEvening = std::string("SimpleState-OnEvening");
const std::string stateOnMorning = std::string("SimpleState-OnMorning");
const std::string stateOn = std::string("SimpleState-On");
const std::string stateCalibration = std::string("SimpleState-Calibration");
Cautomate::Cautomate(boost::shared_ptr<Log::Ilog> log, int ph4CalibrationValue,
		int ph7CalibrationValue, int mesureCacheSize, int captureFrequencyInMs)
{
	m_ioService = boost::shared_ptr<boost::asio::io_service>(
			new boost::asio::io_service);
	m_sbcCtrl = boost::shared_ptr<SBC::CSBCControler>(new SBC::CSBCControler(
			m_ioService, log, ph4CalibrationValue, ph7CalibrationValue,
			mesureCacheSize, captureFrequencyInMs));
	m_log = log;
}

Cautomate::~Cautomate()
{
	m_sbcCtrl->disconnect();
}

int Cautomate::run()
{
	int nbRetry = PHIDGETS_RETRY_ATTATCH;
	m_log->log_info("Running default automate");
	if (m_stateTable.size() == 0)
	{
		m_log->log_info("Initializing automate");
		m_stateTable[stateOff] = boost::shared_ptr<AquaAutomate::CsimpleState>(
				new AquaAutomate::CsimpleState(false, false, false, stateOff,
						m_sbcCtrl, shared_from_this(), m_log));
		m_stateTable[stateOn] = boost::shared_ptr<AquaAutomate::CsimpleState>(
				new AquaAutomate::CsimpleState(true, true, true, stateOn,
						m_sbcCtrl, shared_from_this(), m_log));
		m_stateTable[stateOnEvening] = boost::shared_ptr<
				AquaAutomate::CsimpleState>(new AquaAutomate::CsimpleState(
				false, true, true, stateOnEvening, m_sbcCtrl,
				shared_from_this(), m_log));
		m_stateTable[stateOnMorning] = boost::shared_ptr<
				AquaAutomate::CsimpleState>(new AquaAutomate::CsimpleState(
				false, true, true, stateOnMorning, m_sbcCtrl,
				shared_from_this(), m_log));
		m_stateTable[stateCalibration] = boost::shared_ptr<
				AquaAutomate::CcalibrationState>(
				new AquaAutomate::CcalibrationState(m_sbcCtrl,
						shared_from_this(), m_log));
		m_log->log_info("Automate initialized");
	}
	while (m_sbcCtrl->connect() != SBC::CSBCControler::SBCCtrl_OK && nbRetry
			> 0)
	{
		m_log->log_warn("Phidgets controller not initialized, retry in 30s");
		boost::this_thread::sleep(boost::posix_time::seconds(30));
	}
	if (nbRetry == 0)
	{
		return -1;
	}
	if (!m_pollingThread)
	{
		m_pollingThread = boost::shared_ptr<boost::thread>(new boost::thread(
				boost::bind(&boost::asio::io_service::run, m_ioService)));
	}
	if (!m_timer)
	{
		m_timer = boost::shared_ptr<boost::asio::deadline_timer>(
				new boost::asio::deadline_timer(*m_ioService,
						boost::posix_time::seconds(10)));
		m_timer->async_wait(boost::bind(&Cautomate::onTimer, this));
	}
	m_sbcCtrl->enableFrontNeonTube(false);
	m_sbcCtrl->enableRearNeonTube(false);
	m_sbcCtrl->enableCO2System(false);
	m_pollingThread->join();
	m_log->log_warn("Automate killed ...");
	return 0;
}

boost::optional<boost::posix_time::ptime> Cautomate::switchToState(
		std::string stateName, const boost::posix_time::ptime & currentTime)
{
	boost::optional<boost::posix_time::ptime> timeToNextTimer;
	boost::shared_ptr<AquaAutomate::Istate> previousState = m_currentState;
	m_currentState = m_stateTable[stateName];
	if (previousState && previousState != m_currentState)
	{
		previousState->onExit();
	}
	if (previousState != m_currentState)
	{
		m_log->log_info("Switch to new state : %s", stateName.c_str());
		timeToNextTimer = m_currentState->onEntry(currentTime);
	}
	return timeToNextTimer;
}

void Cautomate::sendEvent(std::string eventName)
{
	if (eventName == "CALIBRATION_DONE")
	{
		boost::posix_time::ptime currentTime(
				boost::posix_time::microsec_clock::local_time());
		switchToState(stateOff, currentTime);
	}
}

void Cautomate::onTimer()
{
	boost::posix_time::ptime currentTime = m_timer->expires_at();
	boost::optional<boost::posix_time::ptime> timeToNextTimer;
	boost::optional<std::string> newStateName = determinateNewState();
	if (newStateName)
	{
		timeToNextTimer = switchToState(*newStateName, currentTime);
		if (!timeToNextTimer)
		{
			timeToNextTimer = m_currentState->onTimer(currentTime);
		}
	}
	else
	{
		timeToNextTimer = m_currentState->onTimer(currentTime);
	}
	if (timeToNextTimer)
	{
		m_timer->expires_at(*timeToNextTimer);
	}
	else
	{
		m_timer->expires_at(m_timer->expires_at() + boost::posix_time::seconds(
				20));
	}
	m_timer->async_wait(boost::bind(&Cautomate::onTimer, this));
}

double Cautomate::getPhValue()
{
	float phValue;
	float stdDeviation;

	m_sbcCtrl->getPhSensorValue(phValue, stdDeviation);
	return phValue;
}

boost::optional<std::string> Cautomate::determinateNewState()
{
	boost::optional<std::string> newStateName;
	if (m_currentState && m_currentState->allowsChangeStateByAutomate())
	{
		boost::posix_time::ptime currentTime(
				boost::posix_time::microsec_clock::local_time());
		boost::posix_time::time_duration timeOfDay = currentTime.time_of_day();
		int currentHour = timeOfDay.hours();
		if (currentHour >= 13 && currentHour < 14)
		{
			newStateName = stateOnMorning;
		}
		else if (currentHour >= 14 && currentHour < 20)
		{
			newStateName = stateOn;
		}
		else if (currentHour >= 20 && currentHour < 22)
		{
			newStateName = stateOnEvening;
		}
		else
		{
			newStateName = stateOff;
		}

	}
	else
	{
		newStateName = stateCalibration;
	}
	return newStateName;
}

/* STATE OFF */
/* STATE ON */
/* STATE ON EVENING */
/* STATE ON MORNING */
/* STATE CALIBRATION */
} /* namespace AquaAutomate */
