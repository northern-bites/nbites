#include "VisionObjects.h"

namespace tool{
namespace visionsim{

VisionCorner::VisionCorner(FieldCorner which) : concrete(which)
{
}

bool VisionCorner::green()
{
    return (concrete == BOTTOM_LEFT_GREEN ||
            concrete == TOP_LEFT_GREEN ||
            concrete == BOTTOM_RIGHT_GREEN ||
            concrete == TOP_RIGHT_GREEN);
}

VisionLine::VisionLine(FieldLine which,
                       VisionCorner* cor1,
                       VisionCorner* cor2) : concrete(which),
                                             corner1(cor1),
                                             corner2(cor2)
{}

}
}
