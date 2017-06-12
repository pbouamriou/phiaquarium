/*!
 * \file IeventHandler.h
 * \brief file defining interface to handle event
 * \date 28 juil. 2011
 * \author Philippe Bouamriou
 */

#ifndef IEVENTHANDLER_H_
#define IEVENTHANDLER_H_

#include <string>

namespace AquaAutomate {

class IeventHandler {
public:
	IeventHandler();
	virtual ~IeventHandler();

	virtual void sendEvent(std::string eventName) = 0;
};

} /* namespace AquaAutomate */
#endif /* IEVENTHANDLER_H_ */
