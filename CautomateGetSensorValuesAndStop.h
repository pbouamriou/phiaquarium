/*
 * CautomateGetSensorValuesAndStop.h
 *
 *  Created on: 26 déc. 2012
 *      Author: philippe
 */

#ifndef CAUTOMATEGETSENSORVALUESANDSTOP_H_
#define CAUTOMATEGETSENSORVALUESANDSTOP_H_

#include <boost/shared_ptr.hpp>
#include <boost/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/asio.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/date_time/posix_time/ptime.hpp>

#include "IeventHandler.h"
#include "Irunnable.h"

namespace Log
{
class Ilog;
}
;

namespace SBC
{
class CSBCControler;
}
;

namespace AquaAutomate
{
class Istate;

class CautomateGetSensorValuesAndStop: public AquaAutomate::IeventHandler,
		public ItfRunnable::Irunnable,
		public boost::enable_shared_from_this<CautomateGetSensorValuesAndStop>
{
public:
	typedef enum
	{
		ACTIVATE_NO_NEON,
		ACTIVATE_FRONT_NEON,
		ACTIVATE_REAR_NEON,
		ACTIVATE_ALL_NEON
	} EneonActivationKind;

	CautomateGetSensorValuesAndStop(boost::shared_ptr<Log::Ilog> log,
			EneonActivationKind neonActivation, int captureDuration,
			int ph4CalibrationValue, int ph7CalibrationValue,
			int mesureCacheSize, int captureFrequencyInMs,
			std::string outputFileName);
	virtual ~CautomateGetSensorValuesAndStop();

	virtual int run();

	virtual void sendEvent(std::string eventName);

private:
	void onTimer();
	boost::shared_ptr<boost::thread> m_pollingThread;
	boost::shared_ptr<boost::asio::io_service> m_ioService;
	boost::shared_ptr<SBC::CSBCControler> m_sbcCtrl;
	boost::shared_ptr<boost::asio::deadline_timer> m_timer;
	boost::shared_ptr<AquaAutomate::Istate> m_currentState;
	boost::shared_ptr<Log::Ilog> m_log;
	EneonActivationKind m_neonActivationKind;
	int m_captureDuration;
	std::string m_outputFileName;
	int m_captureFrequencyInMs;
};

}

#endif /* CAUTOMATEGETSENSORVALUESANDSTOP_H_ */
