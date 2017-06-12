/*!
 * \file CSBCControler.cpp
 * \brief Phidgets SBC2 interface kit controler for aquarium
 * \date 24 juil. 2011
 * \author philippe
 */

#include "CSBCControler.h"

#include <stdio.h>
#include <stdlib.h>

#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include "Ilog.h"

namespace SBC
{

const float CSBCControler::deltaPh = 3.0;

CSBCControler::CSBCControler(
		boost::shared_ptr<boost::asio::io_service> ioService,
		boost::shared_ptr<Log::Ilog> log, int ph4CalibrationValue,
		int ph7CalibrationValue, int mesureCacheSize, int captureFrequencyInMs) :
	m_state(SBC_NotConnected), m_ioService(ioService), m_log(log), m_ph4Value(
			ph4CalibrationValue), m_ph7Value(ph7CalibrationValue),
			m_mesureCacheSize(mesureCacheSize), m_captureFrequencyInMs(
					captureFrequencyInMs)
{
	log->log_info(
			"Calibration value used : ph4CalibrationValue = %d, ph7CalibrationValue = %d",
			m_ph4Value, m_ph7Value);
	log->log_info("Capture frequency : %d", m_captureFrequencyInMs);
	CPhidgetInterfaceKit_create(&m_ifKit);
}

CSBCControler::~CSBCControler()
{
	disconnect();
	CPhidget_delete((CPhidgetHandle) m_ifKit);
}

CSBCControler::SBC_ErrorCode CSBCControler::connect()
{
	int result = 0;
	const char *err;

	// Initialize polling thread to retrieve ph sensor values
	if (!m_timer)
	{
		m_timer = boost::shared_ptr<boost::asio::deadline_timer>(
				new boost::asio::deadline_timer(*m_ioService,
						boost::posix_time::millisec(m_captureFrequencyInMs)));
		m_timer->async_wait(boost::bind(&CSBCControler::onTimer, this));
	}

	//open the interfacekit for device connections
	CPhidget_open((CPhidgetHandle) m_ifKit, -1);

	//get the program to wait for an interface kit device to be attached
	m_log->log_info("Waiting for interface kit to be attached....");
	if ((result = CPhidget_waitForAttachment((CPhidgetHandle) m_ifKit, 5000)))
	{
		CPhidget_getErrorDescription(result, &err);
		m_log->log_warn("Problem waiting for attachment: %s\n", err);
		CPhidget_close((CPhidgetHandle) m_ifKit);
		return SBCCtrl_CannotOpenDevice;
	}

	m_log->log_info("Interface kit succesfully attached....");

	//printf("Modifying sensor sensitivity triggers and ratiometric for PH Sensor ....\n");
	//CPhidgetInterfaceKit_setSensorChangeTrigger(m_ifKit, SBC_PH_SENSOR, 1);
	//CPhidgetInterfaceKit_setRatiometric(m_ifKit, PFALSE);

	//CPhidgetInterfaceKit_set_OnSensorChange_Handler(m_ifKit, &handleOnSensorChanged, (void*)this );

	m_state = SBC_Connected;

#if 0
	// Wait phSensorValue updated
	while( !m_rawPhSensorValue )
	{
		sleep(1);
	}
#endif

	return SBCCtrl_OK;
}

CSBCControler::SBC_ErrorCode CSBCControler::disconnect()
{
	CPhidget_close((CPhidgetHandle) m_ifKit);
	m_state = SBC_NotConnected;

	return SBCCtrl_OK;
}

CSBCControler::SBC_ErrorCode CSBCControler::enableFrontNeonTube(bool enable)
{
	if (m_state == SBC_Connected)
	{
		CPhidgetInterfaceKit_setOutputState(m_ifKit, SBC_FRONT_NEON_TUBE,
				enable ? PTRUE : PFALSE);
	}
	else
		return SBCCtrl_InvalidState;

	return SBCCtrl_OK;
}

CSBCControler::SBC_ErrorCode CSBCControler::enableRearNeonTube(bool enable)
{
	if (m_state == SBC_Connected)
	{
		CPhidgetInterfaceKit_setOutputState(m_ifKit, SBC_REAR_NEON_TUBE,
				enable ? PTRUE : PFALSE);
	}
	else
		return SBCCtrl_InvalidState;

	return SBCCtrl_OK;
}

CSBCControler::SBC_ErrorCode CSBCControler::enableCO2System(bool enable)
{
	if (m_state == SBC_Connected)
	{
		CPhidgetInterfaceKit_setOutputState(m_ifKit, SBC_CO2, enable ? PTRUE
				: PFALSE);
	}
	else
		return SBCCtrl_InvalidState;

	return SBCCtrl_OK;
}

CSBCControler::SBC_ErrorCode CSBCControler::getRawPhSensorValue(int &value,
		float &stdDeviation)
{
	if (m_state == SBC_Connected || m_state == SBC_DisconnectionRequest)
	{
		//CPhidgetInterfaceKit_getSensorValue(m_ifKit, SBC_PH_SENSOR, &value);
		value = calculateAvgPhAndStandardDeviation(stdDeviation);
	}
	else
	{
		value = 0;
		return SBCCtrl_InvalidState;
	}

	return SBCCtrl_OK;
}

CSBCControler::SBC_ErrorCode CSBCControler::getRawPhSensorValueUsingMinMax(int &value)
{
	if (m_state == SBC_Connected || m_state == SBC_DisconnectionRequest)
	{
		//CPhidgetInterfaceKit_getSensorValue(m_ifKit, SBC_PH_SENSOR, &value);
		value = calculateAvgPhUsingMinMax();
	}
	else
	{
		value = 0;
		return SBCCtrl_InvalidState;
	}

	return SBCCtrl_OK;
}

int CSBCControler::calculateAvgPhAndStandardDeviation(float &stdDeviation)
{
	boost::mutex::scoped_lock lock(m_mutex);
	std::list<int>::const_iterator it;
	int count = 0, sum = 0;
	int avg = 0;
	int sqtrDiffSum = 0;

	for (it = m_rawPhSensorValue.begin(); it != m_rawPhSensorValue.end(); ++it)
	{
		sum += *it;
		count += 1;
		//printf("Value %d = %d\n", count, *it);
	}

	if (count > 0)
	{
		avg = sum / count;
		for (it = m_rawPhSensorValue.begin(); it != m_rawPhSensorValue.end(); ++it)
		{
			sqtrDiffSum += ((*it) - avg) * ((*it) - avg);
		}

		stdDeviation = sqrt(float(sqtrDiffSum)) / count;
	}

	//printf("Nb values = %d, sum = %d\n", count, sum);

	return avg;
}

int CSBCControler::calculateAvgPhUsingMinMax()
{
	boost::mutex::scoped_lock lock(m_mutex);
	std::list<int>::const_iterator it;
	int min = 10000, max = 0;

	for (it = m_rawPhSensorValue.begin(); it != m_rawPhSensorValue.end(); ++it)
	{
		if( *it > max ) max = *it;
		if( *it < min ) min = *it;
	}

	return (min+max)/2;
}

CSBCControler::SBC_ErrorCode CSBCControler::getRawPhSensorValues(
		std::list<int> &values)
{
	boost::mutex::scoped_lock lock(m_mutex);
	if (m_state == SBC_Connected || m_state == SBC_DisconnectionRequest)
	{
		std::list<int>::const_iterator it;

		for (it = m_rawPhSensorValue.begin(); it != m_rawPhSensorValue.end(); ++it)
		{
			values.push_back(*it);
		}

		return SBCCtrl_OK;
	}

	return SBCCtrl_InvalidState;
}

CSBCControler::SBC_ErrorCode CSBCControler::getPhSensorValueUsingMinMax(float &value)
{
	int raw_value;
	if (getRawPhSensorValueUsingMinMax(raw_value) == SBCCtrl_OK
			&& m_ph4Value != 0 && m_ph7Value != 0)
	{
		float a = (deltaPh / (m_ph7Value - m_ph4Value));
		float b = 4 - (a * m_ph4Value);
		value = a * raw_value + b;
	}
	else
		return SBCCtrl_InvalidState;

	return SBCCtrl_OK;
}

CSBCControler::SBC_ErrorCode CSBCControler::getPhSensorValue(float &value,
		float &stdDeviation)
{
	int raw_value;
	if (getRawPhSensorValue(raw_value, stdDeviation) == SBCCtrl_OK
			&& m_ph4Value != 0 && m_ph7Value != 0)
	{
		float a = (deltaPh / (m_ph7Value - m_ph4Value));
		float b = 4 - (a * m_ph4Value);
		value = a * raw_value + b;
	}
	else
		return SBCCtrl_InvalidState;

	return SBCCtrl_OK;
}

int CSBCControler::getNbCaptureOfPhSensor()
{
	return m_rawPhSensorValue.size();
}

CSBCControler::SBC_ErrorCode CSBCControler::flushSensorCache()
{
	boost::mutex::scoped_lock lock(m_mutex);
	if (m_state == SBC_Connected || m_state == SBC_DisconnectionRequest)
	{
		m_rawPhSensorValue.clear();
		return SBCCtrl_OK;
	}
	return SBCCtrl_InvalidState;
}

void CSBCControler::onTimer()
{

	int rawPhSensorValue;
	if (m_state == SBC_Connected || m_state == SBC_DisconnectionRequest)
	{
		CPhidgetInterfaceKit_getSensorValue(m_ifKit, SBC_PH_SENSOR,
				&rawPhSensorValue);
		onPhSensorChanged(rawPhSensorValue);
	}

	if (m_state != SBC_DisconnectionRequest)
	{
		m_timer->expires_at(m_timer->expires_at()
				+ boost::posix_time::millisec(m_captureFrequencyInMs));
		m_timer->async_wait(boost::bind(&CSBCControler::onTimer, this));
	}
}

void CSBCControler::onPhSensorChanged(int rawPhSensorValue)
{
	boost::mutex::scoped_lock lock(m_mutex);
	m_rawPhSensorValue.push_back(rawPhSensorValue);
	if (m_rawPhSensorValue.size() > m_mesureCacheSize)
	{
		m_rawPhSensorValue.pop_front();
	}
}

int CSBCControler::handleOnSensorChanged(CPhidgetInterfaceKitHandle phid,
		void *userPtr, int index, int sensorValue)
{
	CSBCControler * self = (CSBCControler*) userPtr;
	//printf("Sensor %d : value = %d\n", index, sensorValue);
	if (index == SBC_PH_SENSOR)
	{
		self->onPhSensorChanged(sensorValue);
	}

	return 0;
}

void CSBCControler::stopAnalyse()
{
	if (m_state != SBC_Connected)
		return;

	m_state = SBC_DisconnectionRequest;
}

} /* namespace SBC */
