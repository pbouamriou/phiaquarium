/*
 * CcalibrationState.cpp
 *
 *  Created on: 17 oct. 2011
 *      Author: philippe
 */

#include "CcalibrationState.h"
#include "CSBCControler.h"
#include "IeventHandler.h"
#include "Ilog.h"

namespace AquaAutomate {

CcalibrationState::CcalibrationState(
		boost::shared_ptr<SBC::CSBCControler> sbcCtrl,
		boost::shared_ptr<AquaAutomate::IeventHandler> eventHandler,
		boost::shared_ptr<Log::Ilog> log) :
		Cstate(sbcCtrl, eventHandler),
		m_calibrationStep(CALIBRATION_ALL_OFF),
		m_log(log) {
}

CcalibrationState::~CcalibrationState() {
}

boost::optional<boost::posix_time::ptime> CcalibrationState::onEntry(
		boost::posix_time::ptime timeOnEntryOccurs) {
	getSBCCtrl()->enableFrontNeonTube(false);
	getSBCCtrl()->enableRearNeonTube(false);
	getSBCCtrl()->enableCO2System(false);
	getSBCCtrl()->flushSensorCache();
	m_calibrationStep = CALIBRATION_ALL_OFF;
	return boost::optional<boost::posix_time::ptime>(
			timeOnEntryOccurs + boost::posix_time::seconds(30));
}

void CcalibrationState::onExit() {
}

boost::optional<boost::posix_time::ptime> CcalibrationState::onTimer(
		boost::posix_time::ptime timeOnTimerOccurs) {
	boost::optional<boost::posix_time::ptime> nextTimer;
	switch (m_calibrationStep) {
	case CALIBRATION_ALL_OFF: {
		float phValue;
		float stdDeviation = 0.0;
		int rawValue;

		getSBCCtrl()->getPhSensorValueUsingMinMax(phValue);
		getSBCCtrl()->getRawPhSensorValueUsingMinMax(rawValue);

		m_measures[CALIBRATION_ALL_OFF] = Cmeasure(phValue, rawValue,
				stdDeviation);
		m_log->log_info(
				"Calibration, ALL-OFF : Ph = %f(value = %d), standard deviation = %f",
				phValue, rawValue, stdDeviation);

		nextTimer = timeOnTimerOccurs + boost::posix_time::seconds(30);
		m_calibrationStep = CALIBRATION_REAR_ON;
		getSBCCtrl()->enableRearNeonTube(true);
		getSBCCtrl()->flushSensorCache();
		break;
	}

	case CALIBRATION_REAR_ON: {
		float phValue;
		float stdDeviation = 0.0;
		int rawValue;

		getSBCCtrl()->getPhSensorValueUsingMinMax(phValue);
		getSBCCtrl()->getRawPhSensorValueUsingMinMax(rawValue);

		m_measures[CALIBRATION_REAR_ON] = Cmeasure(phValue, rawValue,
				stdDeviation);
		m_log->log_info(
				"Calibration, REAR-ON : Ph = %f(value = %d), standard deviation = %f",
				phValue, rawValue, stdDeviation);

		nextTimer = timeOnTimerOccurs + boost::posix_time::seconds(30);
		m_calibrationStep = CALIBRATION_ALL_ON;
		getSBCCtrl()->enableFrontNeonTube(true);
		getSBCCtrl()->flushSensorCache();
		break;
	}

	case CALIBRATION_ALL_ON: {
			float phValue;
			float stdDeviation = 0.0;
			int rawValue;

			getSBCCtrl()->getPhSensorValueUsingMinMax(phValue);
			getSBCCtrl()->getRawPhSensorValueUsingMinMax(rawValue);

			m_measures[CALIBRATION_ALL_ON] = Cmeasure(phValue, rawValue,
					stdDeviation);
			m_log->log_info(
					"Calibration, ALL-ON : Ph = %f(value = %d), standard deviation = %f",
					phValue, rawValue, stdDeviation);

			m_calibrationStep = CALIBRATION_ALL_OFF;
			getSBCCtrl()->enableFrontNeonTube(false);
			getSBCCtrl()->enableRearNeonTube(false);
			getSBCCtrl()->flushSensorCache();

			getEventHandler()->sendEvent("CALIBRATION_DONE");
			break;
		}
	}

	return nextTimer;
}

bool CcalibrationState::allowsChangeStateByAutomate() const {
	return false;
}

} /* namespace AquaAutomate */
