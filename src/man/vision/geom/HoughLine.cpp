#include "HoughLine.h"
#include <stdlib.h>
#include <cmath>
#include <iostream>

#include "FieldLines/HoughConstants.h"
#include "FieldLines/HoughSpace.h"

using namespace std;
using namespace VisionDef;

HoughLine::HoughLine() :
    rIndex(0), tIndex(0), r(0), t(0), score(0),
    sinT(0), cosT(0), didSin(false), didCos(false)
{

}
HoughLine::HoughLine(int _r_Indexbit, int _t_Indexbit, int _score) :
    rIndex(_r_Indexbit), tIndex(_t_Indexbit),
    r(static_cast<float>(rIndex) - HoughConstants::r_span/2.0f + 1.0f),
    t(static_cast<float>(tIndex + 1) * M_PI_FLOAT /128.0f),
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

bool HoughLine::operator==(const HoughLine &other) const
{
    // t,r follow from tIndex,rIndex; no need to compare
    return (other.getScore() == score &&
            other.getRIndex() == rIndex &&
            other.getTIndex() == tIndex);
}

bool HoughLine::operator!=(const HoughLine &other) const
{
    return !(*this == other);
}

/**
 * Finds the distance from the perpendicular of a line to the origin
 * to the edge of an image.
 *
 * u1 is the minimum distance value (signed)
 * u2 is the maximum distance value (signed)
 */
void HoughLine::findLineImageIntersects(const HoughLine& line,
                                        double& u1, double& u2)
{
    // Static so they're only initialized once!
    static int bounds[num_edges] = {IMAGE_WIDTH,
                                    IMAGE_WIDTH,
                                    IMAGE_HEIGHT,
                                    IMAGE_HEIGHT};
    static double intersects[num_edges];

    // Flip cosine sign so it is correctly pointing the right
    // way along the line. (@TODO, better explain)
    const double cs = -line.getCosT();
    const double sn = line.getSinT();

    // Make sure we don't divide by zero!
    double csInv = (cs == 0) ? (10000000) : (1/cs);
    double snInv = (sn == 0) ? (10000000) : (1/sn);

    // Needs original cosine for x0
    const double x0 = -line.getRadius() * cs + IMAGE_WIDTH/2;
    const double y0 = line.getRadius() * sn + IMAGE_HEIGHT/2;

    intersects[top_edge] = -y0*csInv;
    intersects[bottom_edge] = (IMAGE_HEIGHT-1 - y0)*csInv;
    intersects[left_edge] = -x0*snInv;
    intersects[right_edge] = (IMAGE_WIDTH -1 - x0)*snInv;

    // Set up lists for each edge's attributes
    double angles[num_edges] = {sn,sn,cs,cs};
    double offset[num_edges] = {x0,x0,y0,y0};

    for (int i=0; i < num_edges; ++i){
        if (offset[i] + intersects[i]*angles[i] >= 0 &&
            offset[i] + intersects[i]*angles[i] < bounds[i]){
            if (u1 == 0) {
                u1 = intersects[i];
            } else if( abs(u1 - intersects[i]) > 10){
                // Prevent lines from being too short or the same
                // point being used twice
                u2 = intersects[i];
            }
        }
    }
    if (u1 > u2){
        swap(u1,u2);
    }
}
