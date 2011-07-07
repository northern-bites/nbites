
#include "OdometryFilter.h"

const int width = 10; // 1/10 of a second width

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
    const float odoArray[3] = {velX.Y(), velY.Y(), velTheta.Y()};
    return std::vector<float>(odoArray, &odoArray[3]);
}
