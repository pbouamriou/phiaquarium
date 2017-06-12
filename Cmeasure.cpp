/*
 * Cmeasure.cpp
 *
 *  Created on: 18 oct. 2011
 *      Author: philippe
 */

#include "Cmeasure.h"

namespace AquaAutomate {

Cmeasure::Cmeasure(float phValue, int rawValue, float stdDeviation) :
		m_phValue(phValue), m_rawPhValue(rawValue), m_stdDeviation(stdDeviation), m_validMeasure(true) {
}

Cmeasure::Cmeasure() : m_phValue(0.0), m_rawPhValue(0), m_stdDeviation(0.0), m_validMeasure(false) {
}

Cmeasure::~Cmeasure() {
	// TODO Auto-generated destructor stub
}

float Cmeasure::getPhValue() const {
	return m_phValue;
}

int Cmeasure::getRawPhValue() const {
	return m_rawPhValue;
}

float Cmeasure::getStdDeviation() const {
	return m_stdDeviation;
}

bool Cmeasure::isValid() const {
	return m_validMeasure;
}

} /* namespace AquaAutomate */
