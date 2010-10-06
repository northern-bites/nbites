#include "ZmpExp.h"
#include "ExponentialFilter.h"
#include "BasicWorldConstants.h"

ZmpExp::ZmpExp()
	: ExponentialFilter<ZmpMeasurement, zmp_num_dimensions> ()
{


}

ZmpExp::~ZmpExp() {}


void ZmpExp::update(const ZmpTimeUpdate tUp,
					const ZmpMeasurement zMeasure)
{
    // tUp is the same as (currentState(0),currentState(1))
	// so we don't need to do anything with it
	updateStep(zMeasure);
}

void ZmpExp::incorporateMeasurement(ZmpMeasurement zmp)
{
	static const float com_height  = 310; //TODO: Move this (says Joho)

	const float sensor_x = zmp.comX + com_height/GRAVITY_mss * zmp.accX;
	const float sensor_y = zmp.comY + com_height/GRAVITY_mss * zmp.accY;

	updateDimension(ZMP_EXP_X, sensor_x);
	updateDimension(ZMP_EXP_Y, sensor_y);
}
