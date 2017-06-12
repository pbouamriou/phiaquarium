/*
 * Cmeasure.h
 *
 *  Created on: 18 oct. 2011
 *      Author: philippe
 */

#ifndef CMEASURE_H_
#define CMEASURE_H_

namespace AquaAutomate {

class Cmeasure {
public:
	Cmeasure(float phValue, int rawValue, float stdDeviation);
	Cmeasure();
	virtual ~Cmeasure();
    float getPhValue() const;
    int getRawPhValue() const;
    float getStdDeviation() const;
    bool isValid() const;

private:
	float m_phValue;
	int m_rawPhValue;
	float m_stdDeviation;
	bool m_validMeasure;
};

} /* namespace AquaAutomate */
#endif /* CMEASURE_H_ */
