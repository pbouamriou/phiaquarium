/*
 * Csyslog.cpp
 *
 *  Created on: 23 déc. 2012
 *      Author: philippe
 */

#include "Csyslog.h"
#include <syslog.h>
#include <stdarg.h>

namespace Syslog {

Csyslog::Csyslog() {

}

Csyslog::~Csyslog() {
	closelog ();
}

void Csyslog::initialize(const char *progName)
{
	openlog(progName, LOG_CONS | LOG_PID | LOG_NDELAY, LOG_LOCAL1);
}

void Csyslog::log_info(const char *fmt, ...)
{
	va_list argp;
	va_start(argp, fmt);
	vsyslog (LOG_NOTICE, fmt, argp);
	va_end(argp);
}

void Csyslog::log_debug(const char *fmt, ...)
{
	va_list argp;
	va_start(argp, fmt);
	vsyslog (LOG_DEBUG, fmt, argp);
	va_end(argp);
}

void Csyslog::log_warn(const char *fmt, ...)
{
	va_list argp;
	va_start(argp, fmt);
	vsyslog (LOG_WARNING, fmt, argp);
	va_end(argp);
}

void Csyslog::log_error(const char *fmt, ...)
{
	va_list argp;
	va_start(argp, fmt);
	vsyslog (LOG_ERR, fmt, argp);
	va_end(argp);
}

} /* namespace Syslog */
