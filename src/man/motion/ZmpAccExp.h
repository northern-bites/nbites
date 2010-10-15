/*
 * Filters accelerometer values from sensors, extending the Exponential Filter
 * class.
 */

#ifndef _ZmpAccExp_h_DEFINED
#define _ZmpAccExp_h_DEFINED

#include "ExponentialFilter.h"
#include "EKFStructs.h"

#define ZMP_ACCEXP_X 0
#define ZMP_ACCEXP_Y 1
#define ZMP_ACCEXP_Z 2

const unsigned int num_dimensions = 3;

class ZmpAccExp : public ExponentialFilter<AccelMeasurement, num_dimensions>
{
public:
	ZmpAccExp();
	virtual ~ZmpAccExp();

	void update(const float accX,
				const float accY,
				const float accZ);

	// getters
	const float getX() { return getFilteredElement(ZMP_ACCEXP_X); }
    const float getY() { return getFilteredElement(ZMP_ACCEXP_Y); }
    const float getZ() { return getFilteredElement(ZMP_ACCEXP_Z); }

private:
	void incorporateMeasurement(AccelMeasurement m);

};
#endif
