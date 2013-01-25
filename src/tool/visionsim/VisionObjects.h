/*
 * Defines objects in the image, ie not in the real world.
 */

#pragma once

#include "FieldCorners.h"
#include "FieldLines.h"
#include "Posts.h"

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

    bool green() { return corner1->green(); }
    bool bothPointsBehind() { return (corner1->behind() && corner2->behind()); }
    ImagePoint& point1() { return visualPoint1; }
    ImagePoint& point2() { return visualPoint2; }

private:
    FieldLine concrete;
    ImagePoint visualPoint1;
    ImagePoint visualPoint2;
    VisionCorner* corner1;
    VisionCorner* corner2;
};

class VisionPost
{
    friend class Image;

public:
    VisionPost(FieldPost which);

    int x() { return topLeftCorner[X_VALUE]; }
    int y() { return topLeftCorner[Y_VALUE]; }
    int getVisualWidth() { return visualWidth; }
    int getVisualHeight() { return visualHeight; }
    bool behind() { return cameraCoordinates[Z_VALUE] < 0; }

private:
    FieldPost concrete;
    ImagePoint topLeftCorner;
    ImagePoint bottomPoint;
    CameraPoint cameraCoordinates;
    int visualWidth;
    int visualHeight;
};

}
}
