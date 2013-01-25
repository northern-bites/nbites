/*
 * Defines objects in the image, ie not in the real world.
 */

#pragma once

#include "FieldCorners.h"
#include "FieldLines.h"

namespace tool{
namespace visionsim{

// Forward declaration for friending
class Image;

typedef Eigen::Vector3f CameraPoint;
typedef Eigen::Vector2i ImagePoint;

class VisionCorner
{
    friend class Image;

public:
    VisionCorner(FieldCorner which);

    int x() { return imageCoordinates[X_VALUE]; }
    int y() { return imageCoordinates[Y_VALUE]; }
    bool green();
    bool behind() { return (cameraCoordinates[Z_VALUE] < 0) ; }

private:
    FieldCorner concrete;
    CameraPoint cameraCoordinates;
    ImagePoint imageCoordinates;
};

class VisionLine
{
    friend class Image;

public:
    VisionLine(FieldLine which, VisionCorner* cor1, VisionCorner* cor2);

    VisionCorner* getCorner1() { return corner1; }
    VisionCorner* getCorner2() { return corner2; }
    ImagePoint& getIntersection() { return intersection; }
    bool green() { return corner1->green(); }

private:
    FieldLine concrete;
    ImagePoint intersection;
    VisionCorner* corner1;
    VisionCorner* corner2;
};

}
}
