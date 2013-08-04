#include "VisionObjects.h"
#include <iostream>

namespace tool{
namespace visionsim{

/*
 * @param which -- the concrete FieldCorner that this vision corner represents
 */
VisionCorner::VisionCorner(FieldCorner which) : concrete(which)
{}

/*
 * Checks whether the corner is one of the four green field edge corners
 *
 * @returns true if the corner is green, false if white
 */
bool VisionCorner::green()
{
    return (concrete == BOTTOM_LEFT_GREEN ||
            concrete == TOP_LEFT_GREEN ||
            concrete == BOTTOM_RIGHT_GREEN ||
            concrete == TOP_RIGHT_GREEN);
}

/*
 * @param which -- the concrete FieldLine that this vision line represents
 * @param cor1 -- one visual corner that defines the line
 * @param cor2 -- the other visual corner that defines the line
 * note: it makes no difference which corner is which
 */

VisionLine::VisionLine(FieldLine which,
                       VisionCorner* cor1,
                       VisionCorner* cor2) : concrete(which),
                                             corner1(cor1),
                                             corner2(cor2)
{}

/*
 * @param which -- the concrete FieldPost that this vision post represents
 */
VisionPost::VisionPost(FieldPost which) : concrete(which)
{
}

}
}
