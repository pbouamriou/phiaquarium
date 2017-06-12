/*!
 * \file Cstate.h
 * \brief file defining interface to handle event
 * \date 28 juil. 2011
 * \author Philippe Bouamriou
 */

#ifndef CSTATE_H_
#define CSTATE_H_

#include "Istate.h"

namespace SBC {
class CSBCControler;
}

namespace AquaAutomate {

class IeventHandler;

class Cstate: public AquaAutomate::Istate {
public:
	Cstate( boost::shared_ptr<SBC::CSBCControler> sbcCtrl,
			boost::shared_ptr<AquaAutomate::IeventHandler> eventHandler);
	virtual ~Cstate();

protected:
	boost::shared_ptr<SBC::CSBCControler> getSBCCtrl() {return m_sbcCtrl;}
	boost::shared_ptr<AquaAutomate::IeventHandler> getEventHandler() {return m_eventHandler;}

private:
	boost::shared_ptr<SBC::CSBCControler> m_sbcCtrl;
	boost::shared_ptr<AquaAutomate::IeventHandler> m_eventHandler;
};

} /* namespace AquaAutomate */
#endif /* CSTATE_H_ */
