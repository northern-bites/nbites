/*
 * Filters accelerometer values from sensors, extending the Exponential Filter
 * class.
 */

#ifndef _ZmpAccExp_h_DEFINED
#define _ZmpAccExp_h_DEFINED

#include "ExponentialFilter.h"
#include "EKFStructs.h"

const unsigned int num_dimensions = 3;
const float alpha = 0.75;

class ZmpAccExp : public ExponentialFilter<AccelMeasurement, num_dimensions>
{
public:
	ZmpAccExp();
	virtual ~ZmpAccExp();

	void update(const float accX,
				const float accY,
				const float accZ);

	// getters
	const float getX() { return getFilteredElement(0); }
    const float getY() { return getFilteredElement(1); }
    const float getZ() { return getFilteredElement(2); }

protected:
	void incorporateMeasurement(AccelMeasurement m);

};
#endif
