#include "VisionCorner.h"

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

}
}
