/*
 * Irunnable.h
 *
 *  Created on: 26 déc. 2012
 *      Author: philippe
 */

#ifndef IRUNNABLE_H_
#define IRUNNABLE_H_

namespace ItfRunnable
{

class Irunnable
{
public:
	Irunnable();
	virtual ~Irunnable();
	virtual int run() = 0;
};

}

#endif /* IRUNNABLE_H_ */
