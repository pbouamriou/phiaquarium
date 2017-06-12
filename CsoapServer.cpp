/*
 * CsoapServer.cpp
 *
 *  Created on: 10 juil. 2013
 *      Author: philippe
 */

#include "CsoapServer.h"

#include "soapH.h"
#include "phiaquarium.nsmap"

#include <boost/thread.hpp>
#include <boost/bind.hpp>

#include "Cautomate.h"

namespace AquaAutomate
{

boost::shared_ptr<CsoapServer> CsoapServer::m_singleton;

CsoapServer::CsoapServer(boost::shared_ptr<AquaAutomate::Cautomate> i_automate) :
	m_automate(i_automate)
{
}

CsoapServer::~CsoapServer()
{
}

void CsoapServer::start()
{
	if (!m_soapThread)
	{
		m_singleton = shared_from_this();
		m_soapThread = boost::shared_ptr<boost::thread>(
				new boost::thread(boost::bind(&AquaAutomate::CsoapServer::run,
						shared_from_this())));
	}
}

double CsoapServer::_getPhValue()
{
	return m_singleton->getPhValue();
}

double CsoapServer::getPhValue()
{
	return m_automate->getPhValue();
}

void CsoapServer::run()
{
	struct soap soap;
	int m, s;
	soap_init(&soap);
	m = soap_bind(&soap, "phidgets", 4141, 100);
	if (m < 0)
		soap_print_fault(&soap, stderr);
	else
	{
		fprintf(stderr, "Socket connection successful: master socket = %d\n", m);
		for (int i = 1;; i++)
		{
			s = soap_accept(&soap);
			if (s < 0)
			{
				soap_print_fault(&soap, stderr);
				break;
			}
			fprintf(stderr,
					"%d: accepted connection from IP=%d.%d.%d.%d socket=%d", i,
					(soap.ip >> 24) & 0xFF, (soap.ip >> 16) & 0xFF, (soap.ip
							>> 8) & 0xFF, soap.ip & 0xFF, s);
			if (soap_serve(&soap) != SOAP_OK) // process RPC request
				soap_print_fault(&soap, stderr); // print error
			fprintf(stderr, "request served\n");
			soap_destroy(&soap); // clean up class instances
			soap_end(&soap); // clean up everything and close socket
		}
	}
	soap_done(&soap);
}

void CsoapServer::stop()
{
}

}

int ns2__getPhValue(struct soap*, double &result)
{
	result = AquaAutomate::CsoapServer::_getPhValue();
	return SOAP_OK;
}
