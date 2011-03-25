#include "HoughLine.h"
#include <stdlib.h>
#include <cmath>
#include <iostream>

using namespace std;

HoughLine::HoughLine() :
    r(0), t(0), rIndex(0), tIndex(0), score(0)
{

}

HoughLine::HoughLine(int _rIndex, int _tIndex,
                     float _r, float _t, int _score) :
    r(_r), t(_t), rIndex(_rIndex), tIndex(_tIndex), score(_score)
{

}

bool HoughLine::intersect(int x0, int y0,
                          const HoughLine& a, const HoughLine& b)
{
    float sn1 = sinf(a.getAngle());
    float cs1 = cosf(a.getAngle());
    float sn2 = sinf(b.getAngle());
    float cs2 = cosf(b.getAngle());

    float g = cs1 * sn2 - sn1 * cs2;

    if ( g < 0.0000001 ){
        return false;
    }

    const float intX = (sn2  * a.getRadius() - sn1 * b.getRadius()) / g;
    const float intY = (-cs2 * a.getRadius()  + cs1 * b.getRadius()) / g;
    return (abs(intX) <= x0 && abs(intY) <= y0);
}

bool HoughLine::operator==(const HoughLine &other)
{
    return (other.getAngle() == t &&
            other.getRadius() == r &&
            other.getScore() == score &&
            other.getRIndex() == rIndex &&
            other.getTIndex() == tIndex
        );
}

bool HoughLine::operator!=(const HoughLine &other)
{
    return !(*this == other);
}
