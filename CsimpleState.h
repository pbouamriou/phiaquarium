/*!
 * \file CsimpleState.h
 * \brief file defining interface to handle event
 * \date 28 juil. 2011
 * \author Philippe Bouamriou
 */

#ifndef CSIMPLESTATE_H_
#define CSIMPLESTATE_H_

#include "Cstate.h"
#include <string>

namespace Log {
   class Ilog;
};

namespace AquaAutomate {

class CsimpleState: public AquaAutomate::Cstate {
public:
	CsimpleState(
			bool frontNeonTubeOn,
			bool rearNeonTubeOn,
			bool CO2SystemOn,
			std::string myName,
			boost::shared_ptr<SBC::CSBCControler> sbcCtrl,
			boost::shared_ptr<AquaAutomate::IeventHandler> eventHandler,
			boost::shared_ptr<Log::Ilog> log);
	virtual ~CsimpleState();

	virtual boost::optional<boost::posix_time::ptime> onEntry(boost::posix_time::ptime timeOnEntryOccurs);
	virtual void onExit();
	virtual boost::optional<boost::posix_time::ptime> onTimer(boost::posix_time::ptime timeOnTimerOccurs);

	virtual bool allowsChangeStateByAutomate() const;

private:
	bool m_frontNeonTubeOn;
	bool m_rearNeonTubeOn;
	bool m_CO2SystemOn;

	boost::shared_ptr<Log::Ilog> m_log;
	std::string m_myName;
};

} /* namespace AquaAutomate */
#endif /* CSIMPLESTATE_H_ */
