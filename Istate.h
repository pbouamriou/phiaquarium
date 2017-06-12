/*!
 * \file Istate.h
 * \brief file defining interface to handle event
 * \date 28 juil. 2011
 * \author Philippe Bouamriou
 */

#ifndef ISTATE_H_
#define ISTATE_H_

#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/optional.hpp>

namespace AquaAutomate {

class Istate {
public:
	Istate();
	virtual ~Istate();

	virtual boost::optional<boost::posix_time::ptime> onEntry(boost::posix_time::ptime timeOnEntryOccurs) = 0;
	virtual void onExit() = 0;
	virtual boost::optional<boost::posix_time::ptime> onTimer(boost::posix_time::ptime timeOnTimerOccurs) = 0;

	virtual bool allowsChangeStateByAutomate() const = 0;
};

} /* namespace AquaAutomate */
#endif /* ISTATE_H_ */
