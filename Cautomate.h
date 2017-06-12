/*!
 * \file Cautomate.h
 * \brief Automate manages the tank and acces to the SBC Controler
 * \date 27 juil. 2011
 * \author philippe
 */

#ifndef CAUTOMATE_H_
#define CAUTOMATE_H_

#include <boost/shared_ptr.hpp>
#include <boost/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/asio.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/date_time/posix_time/ptime.hpp>

#include <string>
#include <map>

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

class Cautomate: public IeventHandler,
		public ItfRunnable::Irunnable,
		public boost::enable_shared_from_this<Cautomate>
{
public:
	Cautomate(boost::shared_ptr<Log::Ilog> log, int ph4CalibrationValue,
			int ph7CalibrationValue, int mesureCacheSize,
			int captureFrequencyInMs);
	virtual ~Cautomate();
	virtual int run();

	virtual void sendEvent(std::string eventName);

	double getPhValue();

private:
	void onTimer();
	boost::optional<boost::posix_time::ptime> switchToState(
			std::string stateName, const boost::posix_time::ptime &currentTime);
	boost::optional<std::string> determinateNewState();

	boost::shared_ptr<boost::thread> m_pollingThread;
	boost::shared_ptr<boost::asio::io_service> m_ioService;
	boost::shared_ptr<SBC::CSBCControler> m_sbcCtrl;
	boost::shared_ptr<boost::asio::deadline_timer> m_timer;
	boost::shared_ptr<AquaAutomate::Istate> m_currentState;
	std::map<std::string, boost::shared_ptr<AquaAutomate::Istate> >
			m_stateTable;
	boost::shared_ptr<Log::Ilog> m_log;
};

} /* namespace AquaAutomate */
#endif /* CAUTOMATE_H_ */
