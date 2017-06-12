/*
 * CcalibrationState.h
 *
 *  Created on: 17 oct. 2011
 *      Author: philippe
 */

#ifndef CCALIBRATIONSTATE_H_
#define CCALIBRATIONSTATE_H_

#include "Cstate.h"
#include "Cmeasure.h"
#include <map>

namespace Log {
   class Ilog;
};

namespace AquaAutomate {

class CcalibrationState: public AquaAutomate::Cstate {
public:
	enum Estep {
		CALIBRATION_ALL_OFF,
		CALIBRATION_REAR_ON,
		CALIBRATION_ALL_ON
	};
	CcalibrationState(boost::shared_ptr<SBC::CSBCControler> sbcCtrl,
			boost::shared_ptr<AquaAutomate::IeventHandler> eventHandler,
			boost::shared_ptr<Log::Ilog> log);
	virtual ~CcalibrationState();

	virtual boost::optional<boost::posix_time::ptime> onEntry(boost::posix_time::ptime timeOnEntryOccurs);
	virtual void onExit();
	virtual boost::optional<boost::posix_time::ptime> onTimer(boost::posix_time::ptime timeOnTimerOccurs);

	virtual bool allowsChangeStateByAutomate() const;


private:
	Estep m_calibrationStep;
	std::map<Estep, Cmeasure> m_measures;
	boost::shared_ptr<Log::Ilog> m_log;
};

} /* namespace AquaAutomate */
#endif /* CCALIBRATIONSTATE_H_ */
