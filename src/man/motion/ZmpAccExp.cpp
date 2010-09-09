#include "ZmpAccExp.h"
#include "ExponentialFilter.h"

ZmpAccExp::ZmpAccExp()
	: ExponentialFilter<AccelMeasurement, num_dimensions> (alpha)
{
}

ZmpAccExp::~ZmpAccExp() {}

void ZmpAccExp::update(const float accX,
					   const float accY,
					   const float accZ)
{
	AccelMeasurement m = { accX, accY, accZ };
	updateStep(m);
}

void ZmpAccExp::incorporateMeasurement(AccelMeasurement m) {
	// maintain the x, y & z components of the AccelMeasurement
	updateDimension(0, m.x);
	updateDimension(1, m.y);
	updateDimension(2, m.z);
}

