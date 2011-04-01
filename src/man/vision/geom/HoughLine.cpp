#include "HoughLine.h"
#include <stdlib.h>
#include <cmath>
#include <iostream>

#include "HoughSpace.h"

using namespace std;

HoughLine::HoughLine() :
    rIndex(0), tIndex(0), r(0), t(0), score(0),
    sinT(0), cosT(0), didSin(false), didCos(false)
{

}
HoughLine::HoughLine(int _r_Indexbit, int _t_Indexbit, int _score) :
    rIndex(_r_Indexbit), tIndex(_t_Indexbit),
    r(static_cast<float>(rIndex) - HoughSpace::r_span/2.0f + 0.5f),
    t(static_cast<float>(tIndex+0.5) * M_PI_FLOAT /128.0f),
    score(_score), sinT(0), cosT(0), didSin(false), didCos(false)
{

}

bool HoughLine::intersect(int x0, int y0,
                          const HoughLine& a, const HoughLine& b)
{
    const float sn1 = a.getSinT();
    const float cs1 = a.getCosT();
    const float sn2 = b.getSinT();
    const float cs2 = b.getCosT();

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
    // t,r follow from tIndex,rIndex; no need to compare
    return (other.getScore() == score &&
            other.getRIndex() == rIndex &&
            other.getTIndex() == tIndex);
}

bool HoughLine::operator!=(const HoughLine &other)
{
    return !(*this == other);
}
