/*
 * Decides which objects are seen in an image.
 */

#pragma once

#include "VisionCorner.h"
#include "World.h"
#include "ImageConstants.h"
#include <Eigen/Dense>
#include <vector>

namespace tool{
namespace visionsim{

static const int ERROR = -9999;
typedef std::vector<VisionCorner> CornerVector;

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

    // How we determine what's in the image
    void updateCorners();

    // Helper methods
    Eigen::Vector3f fieldToCameraCoords(int x, int y, int z);
    Eigen::Vector3f fieldToCameraCoords(Eigen::Vector3f worldPoint);
    Eigen::Vector2i cameraToImageCoords(float x, float y, float z);
    Eigen::Vector2i cameraToImageCoords(Eigen::Vector3f ccPoint);

    bool isInImage(Eigen::Vector2i point);

    // Things we need to know for the transformations
    World& world;
    Camera type;
    float cameraOffset;
    float cameraHeight;
};

}
}
