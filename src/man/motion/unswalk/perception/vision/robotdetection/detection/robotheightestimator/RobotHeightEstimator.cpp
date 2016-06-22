#include "RobotHeightEstimator.hpp"

#include "utils/angles.hpp"
#include "perception/vision/VisionDefs.hpp"

static const double TOP_DEGREE_TO_PIXEL = TOP_IMAGE_ROWS / RAD2DEG(IMAGE_VFOV);
static const double BOT_DEGREE_TO_PIXEL = BOT_IMAGE_ROWS / RAD2DEG(IMAGE_VFOV);
static const unsigned int ROBOT_HEIGHT = 550;


/**
 * Given the bounding box it determines the RRCoord of the feet.
 */
static RRCoord getFeetCoord(const BBox &b, VisionFrame &frame, const Fovea &saliency) {
    Point feet(b.width() / 2 + b.a.x(), b.b.y());

    return frame.cameraToRR.convertToRR(saliency.mapFoveaToImage(feet), false);
}


/**
 * Using trigonometry to calculate the possible height. It uses the estimated distance
 * and the robots height to determine an angle. From there we estimate that angle
 * to pixels in the image.
 */
std::vector<PossibleRobot> RobotHeightEstimator::complete(VisionFrame &frame,
        const Fovea &saliency, std::vector<BBox> obstructions) {

    double degreeToPixel;
    if (saliency.top) {
        degreeToPixel = TOP_DEGREE_TO_PIXEL;
    } else {
        degreeToPixel = BOT_DEGREE_TO_PIXEL;
    }

    std::vector<PossibleRobot> robots;
    for (unsigned int i = 0; i < obstructions.size(); ++i) {
        BBox &obstruction = obstructions[i];

        RRCoord feet = getFeetCoord(obstruction, frame, saliency);

        float angleToFullHeight  = RAD2DEG((float)atan2(ROBOT_HEIGHT, feet.distance()));
        float fullHeightInPixels = angleToFullHeight * degreeToPixel;
        int fullHeightYPos       = obstruction.b.y() - (fullHeightInPixels / saliency.density);
        if (fullHeightYPos < 0) {
            fullHeightYPos = 0;
        }

        obstruction.a.y() = fullHeightYPos;

        robots.push_back(PossibleRobot(obstruction, feet));
    }

    return robots;
}
