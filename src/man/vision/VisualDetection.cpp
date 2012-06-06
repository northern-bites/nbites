// VisualDetection.cpp
#include "VisualDetection.h"

VisualDetection::VisualDetection(int _x, int _y, float _distance,
                                 float _bearing) : x(_x), y(_y),
                                                   distance(_distance),
                                                   bearing(_bearing)
{
}
VisualDetection::VisualDetection(const VisualDetection& other)
    : x(other.x), y(other.y), width(other.width), height(other.height),
      centerX(other.centerX), centerY(other.centerY), angleX(other.angleX),
      angleY(other.angleY), distance(other.distance),
      bearing(other.bearing), elevation(other.elevation),
      distanceSD(other.distanceSD), bearingSD(other.bearingSD) {}

VisualDetection::~VisualDetection() {}
