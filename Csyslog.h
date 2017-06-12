/*
 * Csyslog.h
 *
 *  Created on: 23 déc. 2012
 *      Author: philippe
 */

#ifndef CSYSLOG_H_
#define CSYSLOG_H_

#include "Ilog.h"

namespace Syslog {

class Csyslog: public Log::Ilog {
public:
	Csyslog();
	virtual ~Csyslog();

	virtual void initialize(const char *progName);
	virtual void log_info(const char *fmt, ...);
	virtual void log_debug(const char *fmt, ...);
	virtual void log_warn(const char *fmt, ...);
	virtual void log_error(const char *fmt, ...);
};

} /* namespace Syslog */
#endif /* CSYSLOG_H_ */
