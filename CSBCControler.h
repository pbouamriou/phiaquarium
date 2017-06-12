/*!
 * \file CSBCControler.h
 * \brief Phidgets SBC2 interface kit controler for aquarium
 * \date 24 juil. 2011
 * \author philippe
 */

#ifndef CSBCCONTROLER_H_
#define CSBCCONTROLER_H_

#include <phidget21.h>

#include <list>
#include <boost/shared_ptr.hpp>
#include <boost/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/asio.hpp>

namespace Log
{
class Ilog;
}
;

/*! \namespace SBC
 * Namespace for Phidgets SBC cards
 */
namespace SBC
{

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
class CSBCControler
{
public:
	enum SBC_ErrorCode
	{
		SBCCtrl_OK,
		SBCCtrl_CannotCreateDevice,
		SBCCtrl_CannotOpenDevice,
		SBCCtrl_CannotCloseDevice,
		SBCCtrl_InvalidState
	};

	enum SBC_State
	{
		SBC_NotConnected, SBC_Connected, SBC_DisconnectionRequest
	};

	enum SBC_Output
	{
		SBC_FRONT_NEON_TUBE = 0, SBC_REAR_NEON_TUBE = 1, SBC_CO2 = 2
	};

	enum SBC_Input
	{
		SBC_PH_SENSOR = 0
	};

	CSBCControler(boost::shared_ptr<boost::asio::io_service> ioService,
			boost::shared_ptr<Log::Ilog> log, int ph4CalibrationValue,
			int ph7CalibrationValue,
			int mesureCacheSize, int captureFrequencyInMs);
	virtual ~CSBCControler();
	SBC_ErrorCode connect();
	SBC_ErrorCode disconnect();

	SBC_ErrorCode enableFrontNeonTube(bool enable = true);
	SBC_ErrorCode enableRearNeonTube(bool enable = true);
	SBC_ErrorCode enableCO2System(bool enable = true);

	SBC_ErrorCode getRawPhSensorValues(std::list<int> & values );
	SBC_ErrorCode getRawPhSensorValue(int &value, float &stdDeviation);
	SBC_ErrorCode getRawPhSensorValueUsingMinMax(int &value);
	SBC_ErrorCode getPhSensorValue(float &value, float &stdDeviation);
	SBC_ErrorCode getPhSensorValueUsingMinMax(float &value);
	SBC_ErrorCode flushSensorCache();
	int getNbCaptureOfPhSensor();
	int calculateAvgPhAndStandardDeviation(float &stdDeviation);
	int calculateAvgPhUsingMinMax();

	void setPh4CalibrationValue(int ph4CalibrationValue)
	{
		m_ph4Value = ph4CalibrationValue;
	}
	void setPh7CalibrationValue(int ph7CalibrationValue)
	{
		m_ph7Value = ph7CalibrationValue;
	}

	void stopAnalyse();

	void onTimer();

private:
	SBC_State m_state;
	CPhidgetInterfaceKitHandle m_ifKit;
	std::list<int> m_rawPhSensorValue;
	boost::mutex m_mutex;
	boost::shared_ptr<boost::asio::deadline_timer> m_timer;
	boost::shared_ptr<boost::asio::io_service> m_ioService;

	int m_ph4Value;
	int m_ph7Value;
	static const float deltaPh;
	int m_mesureCacheSize;
	int m_captureFrequencyInMs;

	boost::shared_ptr<Log::Ilog> m_log;

	void onPhSensorChanged(int rawPhSensorValue);
	static int handleOnSensorChanged(CPhidgetInterfaceKitHandle phid,
			void *userPtr, int index, int sensorValue);
};

} /* namespace SBC */
#endif /* CSBCCONTROLER_H_ */
