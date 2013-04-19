#include "HoughVisualLine.h"
using namespace std;
#include <iostream>

namespace man {
namespace vision {

bool HoughVisualLine::intersects(const HoughVisualLine& other,
                                 point<int>& out) const
{
    point<int> a, b, c, d;
    bool allIntersecting =
        mLines.first.intersects(other.getHoughLines().first, a)  &&
        mLines.first.intersects(other.getHoughLines().second, b) &&
        mLines.second.intersects(other.getHoughLines().first, c) &&
        mLines.second.intersects(other.getHoughLines().second, d);

    if (allIntersecting) {
        out.x = (a.x + b.x + c.x + d.x)/4;
        out.y = (a.y + b.y + c.y + d.y)/4;
    }
    return allIntersecting;
}

}
}
