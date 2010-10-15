#include "ZmpAccExp.h"
#include "ExponentialFilter.h"

ZmpAccExp::ZmpAccExp()
	: ExponentialFilter<AccelMeasurement, num_dimensions> ()
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
	updateDimension(ZMP_ACCEXP_X, m.x);
	updateDimension(ZMP_ACCEXP_Y, m.y);
	updateDimension(ZMP_ACCEXP_Z, m.z);
}
