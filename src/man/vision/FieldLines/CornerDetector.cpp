#include "CornerDetector.h"
#include <iostream>
#include "HoughVisualLine.h"
#include "HoughVisualCorner.h"

using namespace std;

namespace man {
namespace vision {

void CornerDetector::detect(int upperBound,
                            int * field_edge,
                            const std::vector<HoughVisualLine>& lines)
{
    mCorners.clear();

    point<int> intersection;
    for (std::vector<HoughVisualLine>::const_iterator i = lines.begin();
         i != lines.end(); ++i) {

        std::vector<HoughVisualLine>::const_iterator j = i;
        for (j++; j != lines.end(); ++j) {
            bool doesIntersect = i->intersects(*j, intersection);

            if(!doesIntersect) continue;

            intersection.x += Gradient::cols/2;
            intersection.y += Gradient::rows/2;

            if (intersection.x > 0 && intersection.x < Gradient::cols  &&
                intersection.y < Gradient::rows &&
                intersection.y > field_edge[static_cast<int>(intersection.x)]) {
                mCorners.push_back(HoughVisualCorner(intersection));
            }
        }
    }

}

}
}
