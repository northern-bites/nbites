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
#include "RobotDetector.h"
#include "InertialState.pb.h"

#include "json.hpp"
#include "nblogio.h"

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
    RobotDetector* getRobotDetector(bool topCamera = true) const {return robotDetector[!topCamera]; }

#ifdef OFFLINE
	void setDebugDrawingParameters(nbl::SExpr* debugParams);
#endif
    
    // For use by vision_defs
    void setColorParams(Colors* colors, bool topCamera);
    void setCalibrationParams(CalibrationParams* params, bool topCamera);

    void blackStar(bool blackStar) { blackStar_ = blackStar; }
    bool blackStar() const {return blackStar_;}

    json::Object latestUsedColorParams[2];
    Colors* colorParams[2];
    CalibrationParams* calibrationParams[2];

protected:
    virtual void run_();

private:
#ifdef USE_LOGGING
    void logImage(int i);
#endif
    void outportalVisionField();

    nbl::io::FileMonitor colorParamsMonitor;
    nbl::io::FileMonitor camOffsetsMonitor;
    void reloadColorParams();
    void reloadCameraOffsets();

    ImageFrontEnd* frontEnd[2];
    EdgeDetector* edgeDetector[2];
    EdgeList* edges[2];
    EdgeList* rejectedEdges[2];
    HoughLineList* houghLines[2];
    HoughSpace* hough[2];
    Kinematics* kinematics[2];
    FieldHomography* homography[2];
    FieldLineList* fieldLines[2];
	DebugImage* debugImage[2];
	Field* field;
    GoalboxDetector* boxDetector[2];
    CornerDetector* cornerDetector[2];
    CenterCircleDetector* centerCircleDetector[2];
    BallDetector* ballDetector[2];
    RobotDetector* robotDetector[2];

    bool blackStar_;

    std::string name;

    // Tracking ball stuff
    bool ballOn;
    int ballOnCount;
    int ballOffCount;

	uint8_t * debugSpace[2];

    size_t image_index;

    // Constants for tilt azimuth adjustment hack
    static constexpr double azimuth_m = 0.032228876;
    static constexpr double azimuth_b = 0.0003214286;
};

}
}
