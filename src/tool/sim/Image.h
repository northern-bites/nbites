/*
 * @class Image
 *
 * The main class that does all of the transformation and projection math
 * and thus decides which objects appear on an image and where. It needs to
 * have a world model to base the projections on, and it needs to know which
 * of the Nao cameras it is simulating, ie TOP or BOTTOM.
 *
 * It keeps track of its projection results in vectors of Corners, Lines, and
 * Posts, plus a VisionBall object for the ball because there's only one.
 *
 * @author Lizzie Mamantov
 */

#pragma once

#include "VisionObjects.h"
#include "World.h"
#include "ImageConstants.h"
#include <Eigen/Dense>
#include <vector>

namespace tool{
namespace visionsim{

// Used if a value does not need to be set
static const int ERROR = -9999;

// Vectors of different visual objects
typedef std::vector<VisionCorner> CornerVector;
typedef std::vector<VisionLine> LineVector;
typedef std::vector<VisionPost> PostVector;

class Image {
public:
    Image(World& state, Camera which);
    ~Image(){};

    // The view is given access to this class because it essentially needs
    // to know everything about ann image to render it properly
    friend class ImageView;

    // Update all of the vision information
    void update();

private:
    // Things that appear in the image
    CornerVector visibleCorners;
    CornerVector allCorners;
    LineVector allLines;
    VisionBall ball;
    PostVector allPosts;

    // Methods that determine what's in the image
    void updateCorners();
    void updateLines();
    void updateBall();
    void updatePosts();

    // Helper methods
    CameraPoint fieldToCameraCoords(int x, int y, int z);
    CameraPoint fieldToCameraCoords(Eigen::Vector3f worldPoint);
    ImagePoint cameraToImageCoords(float x, float y, float z);
    ImagePoint cameraToImageCoords(CameraPoint ccPoint);
    void fixVisualPoints(VisionLine& line);
    bool isInImage(ImagePoint point);

    // Helpful for setting up lines
    VisionCorner* getCorner(FieldCorner type);

    // Things we need to know for the transformations
    World& world;
    Camera type;
    // The following are set based on the camera type
    float cameraOffset;
    float cameraHeight;
};

}
}
