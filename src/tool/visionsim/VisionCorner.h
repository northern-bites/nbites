/*
 * Defines a corner actually seen in vision.
 */

#pragma once

#include "FieldCorners.h"

typedef Eigen::Vector3f CameraPoint;
typedef Eigen::Vector2i ImagePoint;
class VisionCorner {
    friend class Image;

public:
    VisionCorner(FieldCorner which);
    ~VisionCorner() {};

    int x() { return imageCoordinates[X_VALUE]; }
    int y() { return imageCoordinates[Y_VALUE]; }
    bool green();
    bool behind() { return (cameraCoordinates[Z_VALUE] < 0) ; }

private:
    FieldCorner concrete;
    CameraPoint cameraCoordinates;
    ImagePoint imageCoordinates;
};
