/*
 * Ilog.h
 *
 *  Created on: 23 déc. 2012
 *      Author: philippe
 */

#ifndef ILOG_H_
#define ILOG_H_

namespace Log {

class Ilog {
public:
	Ilog();
	virtual ~Ilog();

	virtual void initialize(const char *progName) = 0;
	virtual void log_info(const char *fmt, ...) =0;
	virtual void log_debug(const char *fmt, ...) =0;
	virtual void log_warn(const char *fmt, ...) =0;
	virtual void log_error(const char *fmt, ...) =0;
};

} /* namespace Log */
#endif /* ILOG_H_ */
