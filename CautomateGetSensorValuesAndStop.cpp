/*
 * CautomateGetSensorValuesAndStop.cpp
 *
 *  Created on: 26 déc. 2012
 *      Author: philippe
 */

#include "CautomateGetSensorValuesAndStop.h"

#include "CSBCControler.h"
#include "CsimpleState.h"

#include <boost/thread.hpp>
#include "Ilog.h"
#include <fstream>

const int PHIDGETS_RETRY_ATTATCH = 10;

const std::string stateGetSensorValues = std::string(
		"SimpleState-GetSensorValues");

namespace AquaAutomate
{

CautomateGetSensorValuesAndStop::CautomateGetSensorValuesAndStop(
		boost::shared_ptr<Log::Ilog> log, EneonActivationKind neonActivation,
		int captureDuration, int ph4CalibrationValue, int ph7CalibrationValue,
		int mesureCacheSize, int captureFrequencyInMs,
		std::string outputFileName)
{
	m_ioService = boost::shared_ptr<boost::asio::io_service>(
			new boost::asio::io_service);
	m_sbcCtrl = boost::shared_ptr<SBC::CSBCControler>(new SBC::CSBCControler(
			m_ioService, log, ph4CalibrationValue, ph7CalibrationValue,
			mesureCacheSize, captureFrequencyInMs));
	m_log = log;
	m_neonActivationKind = neonActivation;
	m_captureDuration = captureDuration;
	m_captureFrequencyInMs = captureFrequencyInMs;
	m_outputFileName = outputFileName;
}

CautomateGetSensorValuesAndStop::~CautomateGetSensorValuesAndStop()
{
	m_sbcCtrl->disconnect();
}

int CautomateGetSensorValuesAndStop::run()
{
	bool frontNeon = false, rearNeon = false;

	m_log->log_info("Running automate retreiving sensor values and quit");

	switch (m_neonActivationKind)
	{
	case ACTIVATE_ALL_NEON:
		frontNeon = rearNeon = true;
		break;
	case ACTIVATE_FRONT_NEON:
		frontNeon = true;
		break;
	case ACTIVATE_REAR_NEON:
		rearNeon = true;
		break;
	default:
		frontNeon = rearNeon = false;
		break;
	}

	m_log->log_info("Activate neon, rear = %s, front = %s", rearNeon ? "true"
			: "false", frontNeon ? "true" : "false");

	int nbRetry = PHIDGETS_RETRY_ATTATCH;
	if (!m_currentState)
	{
		m_log->log_info("Initializing automate");
		/* STATE GET SENSOR VALUES */
		m_currentState = boost::shared_ptr<AquaAutomate::Istate>(
				new AquaAutomate::CsimpleState(frontNeon, rearNeon, false,
						stateGetSensorValues, m_sbcCtrl, shared_from_this(),
						m_log));
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
						boost::posix_time::seconds(m_captureDuration)));
		m_timer->async_wait(boost::bind(
				&CautomateGetSensorValuesAndStop::onTimer, this));
	}

	m_sbcCtrl->enableFrontNeonTube(frontNeon);
	m_sbcCtrl->enableRearNeonTube(rearNeon);
	m_sbcCtrl->enableCO2System(false);

	m_pollingThread->join();

	m_log->log_warn("Automate stopped ...");

	float phValue;
	float stdDeviation;
	int rawValue, rawValueUsingMinMax;

	m_sbcCtrl->getPhSensorValue(phValue, stdDeviation);
	m_sbcCtrl->getRawPhSensorValue(rawValue, stdDeviation);
	m_sbcCtrl->getRawPhSensorValueUsingMinMax(rawValueUsingMinMax);
	int nbCaptureOfPhSensor = m_sbcCtrl->getNbCaptureOfPhSensor();

	std::cout << "Ph sensor (avg) = " << phValue << ", raw = " << rawValue
			<< ", raw (minMax algo) = " << rawValueUsingMinMax
			<< ", deviation = " << stdDeviation << ", nb captures = "
			<< nbCaptureOfPhSensor << std::endl;

	m_log->log_info(
			"Ph sensor (avg) = %f, raw = %d, deviation = %f, nb captures = %d",
			phValue, rawValue, stdDeviation, nbCaptureOfPhSensor);

	std::list<int> rawValues;
	int row = 1;

	if (m_outputFileName != "")
	{
		std::ofstream output(m_outputFileName.c_str());
		m_sbcCtrl->getRawPhSensorValues(rawValues);

		for (std::list<int>::iterator it = rawValues.begin(); it
				!= rawValues.end(); ++it, ++row)
		{
			output << row << ";" << *it << std::endl;
		}
	}

	return 0;
}

void CautomateGetSensorValuesAndStop::sendEvent(std::string eventName)
{
	//NOTHING TO DO
}

void CautomateGetSensorValuesAndStop::onTimer()
{
	//EVERYTHING IS DONE ==> FINISHED
	m_log->log_info("Capture finished");
	m_sbcCtrl->stopAnalyse();
}

}
