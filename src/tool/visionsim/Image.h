/*
 * Decides which objects are seen in an image.
 */

#pragma once

#include "VisionObjects.h"
#include "World.h"
#include "ImageConstants.h"
#include <Eigen/Dense>
#include <vector>

namespace tool{
namespace visionsim{

static const int ERROR = -9999;
typedef std::vector<VisionCorner> CornerVector;
typedef std::vector<VisionLine> LineVector;

class Image {
public:
    Image(World& state, Camera which);
    ~Image(){};

    friend class ImageView;

    // Update all of the vision information
    void update();

private:
    // Things that appear in the image
    CornerVector visibleCorners;
    CornerVector allCorners;

    LineVector allLines;

    // How we determine what's in the image
    void updateCorners();
    void updateLines();

    // Helper methods
    Eigen::Vector3f fieldToCameraCoords(int x, int y, int z);
    Eigen::Vector3f fieldToCameraCoords(Eigen::Vector3f worldPoint);
    Eigen::Vector2i cameraToImageCoords(float x, float y, float z);
    Eigen::Vector2i cameraToImageCoords(Eigen::Vector3f ccPoint);
    void correctIntercept(VisionLine& line);

    bool isInImage(Eigen::Vector2i point);

    VisionCorner* getCorner(FieldCorner type);

    // Things we need to know for the transformations
    World& world;
    Camera type;
    float cameraOffset;
    float cameraHeight;
};

}
}
