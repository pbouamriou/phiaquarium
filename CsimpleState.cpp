/*!
 * \file CsimpleState.cpp
 * \brief file defining interface to handle event
 * \date 28 juil. 2011
 * \author Philippe Bouamriou
 */


#include "CsimpleState.h"
#include "CSBCControler.h"
#include "Ilog.h"

namespace AquaAutomate {

CsimpleState::CsimpleState(
		bool frontNeonTubeOn,
		bool rearNeonTubeOn,
		bool CO2SystemOn,
		std::string myName,
		boost::shared_ptr<SBC::CSBCControler> sbcCtrl,
		boost::shared_ptr<AquaAutomate::IeventHandler> eventHandler,
		boost::shared_ptr<Log::Ilog> log) :
		Cstate(sbcCtrl, eventHandler),
		m_frontNeonTubeOn(frontNeonTubeOn),
		m_rearNeonTubeOn(rearNeonTubeOn),
		m_CO2SystemOn(CO2SystemOn),
		m_myName(myName),
		m_log(log) {
}

CsimpleState::~CsimpleState() {
}

boost::optional<boost::posix_time::ptime> CsimpleState::onEntry(boost::posix_time::ptime timeOnEntryOccurs) {
	getSBCCtrl()->enableFrontNeonTube(m_frontNeonTubeOn);
	getSBCCtrl()->enableRearNeonTube(m_rearNeonTubeOn);
	getSBCCtrl()->enableCO2System(m_CO2SystemOn);
	return boost::optional<boost::posix_time::ptime>();
}

void CsimpleState::onExit() {
}

boost::optional<boost::posix_time::ptime> CsimpleState::onTimer(boost::posix_time::ptime timeOnTimerOccurs) {
	float phValue;
	float stdDeviation;
	int rawValue;

	getSBCCtrl()->getPhSensorValueUsingMinMax(phValue);
	getSBCCtrl()->getRawPhSensorValueUsingMinMax(rawValue);
	m_log->log_info("%s : Ph = %f(value = %d)", m_myName.c_str(), phValue, rawValue);

	return boost::optional<boost::posix_time::ptime>();
}

bool CsimpleState::allowsChangeStateByAutomate() const {
	return true;
}

} /* namespace AquaAutomate */
