/*!
 * \file Cstate.cpp
 * \brief file defining interface to handle event
 * \date 28 juil. 2011
 * \author Philippe Bouamriou
 */

#include "Cstate.h"
#include "IeventHandler.h"
#include "CSBCControler.h"

namespace AquaAutomate {

Cstate::Cstate(
		boost::shared_ptr<SBC::CSBCControler> sbcCtrl,
		boost::shared_ptr<AquaAutomate::IeventHandler> eventHandler) : m_sbcCtrl(sbcCtrl), m_eventHandler(eventHandler) {
}

Cstate::~Cstate() {
}

} /* namespace AquaAutomate */
