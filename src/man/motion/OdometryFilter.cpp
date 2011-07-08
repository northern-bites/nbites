
#include "OdometryFilter.h"

const int width = 20;

OdoFilter::OdoFilter()
    : velX(width),
      velY(width),
      velTheta(width)
{
    Reset();
}

void OdoFilter::Reset()
{
    Filter:: Reset();
    velX.    Reset();
    velY.    Reset();
    velTheta.Reset();
}

void OdoFilter::update (float _velX, float _velY, float _velTheta)
{
    velX.X(_velX);
    velY.X(_velY);
    velTheta.X(_velTheta);
}

std::vector<float> OdoFilter::getOdometry()
{
    float dy;
    if (velY.Steady())
	dy = velY.Y();
    else
	dy = 0;

    float dx;
    if (velX.Steady())
	dx = velX.Y();
    else
	dx = 0;

    float dt;
    if (velTheta.Steady())
	dt = velTheta.Y();
    else
	dt = 0;

    const float odoArray[3] = { dx, dy, dt };
    return std::vector<float>(odoArray, &odoArray[3]);
}
