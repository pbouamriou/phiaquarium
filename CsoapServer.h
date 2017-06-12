/*
 * CsoapServer.h
 *
 *  Created on: 10 juil. 2013
 *      Author: philippe
 */

#ifndef CSOAPSERVER_H_
#define CSOAPSERVER_H_

#include <boost/enable_shared_from_this.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/thread.hpp>

namespace AquaAutomate
{
class Cautomate;

class CsoapServer : public boost::enable_shared_from_this<CsoapServer>
{
public:
	CsoapServer(boost::shared_ptr<AquaAutomate::Cautomate> i_automate);
	virtual ~CsoapServer();

	void start();
	void stop();

	static double _getPhValue();

	double getPhValue();

	void run();
private:
	boost::shared_ptr<boost::thread> m_soapThread;
	static boost::shared_ptr<CsoapServer> m_singleton;
	boost::shared_ptr<AquaAutomate::Cautomate> m_automate;
};

}

#endif /* CSOAPSERVER_H_ */
