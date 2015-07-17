#pragma once

#include "RoboGrams.h"
#include "SExpr.h"
#include "Camera.h"
#include "Images.h"
#include "PMotion.pb.h"
#include "Vision.pb.h"
#include "FrontEnd.h"
#include "Edge.h"
#include "Hough.h"
#include "Kinematics.h"
#include "Homography.h"
#include "Field.h"

#include "BallDetector.h"
#include "RobotObstacle.h"
#include "InertialState.pb.h"
#include "VisionRobot.pb.h"


namespace man {
namespace vision {

    const int DEFAULT_TOP_IMAGE_WIDTH = 640;
    const int DEFAULT_TOP_IMAGE_HEIGHT= 480;

class VisionModule : public portals::Module {
public:
    VisionModule(int wd, int ht, std::string robotName = "");
    virtual ~VisionModule();

    portals::InPortal<messages::YUVImage> topIn;
    portals::InPortal<messages::YUVImage> bottomIn;
    portals::InPortal<messages::JointAngles> jointsIn;
    portals::InPortal<messages::InertialState> inertsIn;

    portals::OutPortal<messages::Vision> visionOut;
    portals::OutPortal<messages::RobotObstacle> robotObstacleOut;

    ImageFrontEnd* getFrontEnd(bool topCamera = true) const { return frontEnd[!topCamera]; }
    EdgeList* getEdges(bool topCamera = true) const { return edges[!topCamera]; }
    HoughLineList* getLines(bool topCamera = true) const { return houghLines[!topCamera]; }
	DebugImage* getDebugImage(bool topCamera = true) const { return debugImage[!topCamera]; }
    BallDetector* getBallDetector(bool topCamera = true) const { return ballDetector[!topCamera]; }
    EdgeList* getRejectedEdges(bool topCamera = true) const {return rejectedEdges[!topCamera]; }
    HoughLineList* getHoughLines(bool topCamera = true) const { return houghLines[!topCamera]; }
    Kinematics* getKinematics(bool topCamera = true) const {return kinematics[!topCamera]; }
    FieldHomography* getFieldHomography(bool topCamera = true) const {return homography[!topCamera]; }
    FieldLineList* getFieldLines(bool topCamera = true) const { return fieldLines[!topCamera]; }
    GoalboxDetector* getBox(bool topCamera = true) const { return boxDetector[!topCamera]; }
    CornerDetector* getCorners(bool topCamera = true) const { return cornerDetector[!topCamera]; }
    CenterCircleDetector* getCCD(bool topCamera = true) const {return centerCircleDetector[!topCamera]; }

#ifdef OFFLINE
	void setDebugDrawingParameters(nblog::SExpr* debugParams);
#endif
    
    // For use by vision_defs
    void setColorParams(Colors* colors, bool topCamera);
    const std::string getStringFromTxtFile(std::string path);
    Colors* getColorsFromLisp(nblog::SExpr* colors, int camera);
    void setCalibrationParams(std::string robotName);
    void setCalibrationParams(int camera, std::string robotName);
    void setCalibrationParams(CalibrationParams* params, bool topCamera);
    void blackStar(bool blackStar) { blackStar_ = blackStar; }
    bool blackStar() const {return blackStar_;}

protected:
    virtual void run_();

private:
#ifdef USE_LOGGING
    void logImage(int i);
#endif
    void outportalVisionField();
    void updateObstacleBox();

    Colors* colorParams[2];
    ImageFrontEnd* frontEnd[2];
    EdgeDetector* edgeDetector[2];
    EdgeList* edges[2];
    EdgeList* rejectedEdges[2];
    HoughLineList* houghLines[2];
    HoughSpace* hough[2];
    CalibrationParams* calibrationParams[2];
    Kinematics* kinematics[2];
    FieldHomography* homography[2];
    FieldLineList* fieldLines[2];
	DebugImage* debugImage[2];
	Field* field;
    GoalboxDetector* boxDetector[2];
    CornerDetector* cornerDetector[2];
    CenterCircleDetector* centerCircleDetector[2];
    BallDetector* ballDetector[2];

    bool blackStar_;

    std::string name;
    
    // Lisp tree with color params saved
    nblog::SExpr colors;

    // Tracking ball stuff
    bool ballOn;
    int ballOnCount;
    int ballOffCount;

	uint8_t * debugSpace[2];

    nblog::SExpr* calibrationLisp;
    size_t image_index;

    // Constants for tilt azimuth adjustment hack
    static constexpr double azimuth_m = 0.032228876;
    static constexpr double azimuth_b = 0.0003214286;

    // obstacleBox
    RobotObstacle* robotImageObstacle;
    float obstacleBox[4];
};

}
}
