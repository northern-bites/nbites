#pragma once

#include "RoboGrams.h"
#include "SExpr.h"
#include "Camera.h"
#include "Images.h"
#include "PMotion.pb.h"
#include "FrontEnd.h"
#include "Edge.h"
#include "Hough.h"
#include "Kinematics.h"
#include "Homography.h"
#include "InertialState.pb.h"

namespace man {
namespace vision {

class VisionModule : public portals::Module {
public:
    VisionModule();
    virtual ~VisionModule();

    portals::InPortal<messages::YUVImage> topIn;
    portals::InPortal<messages::YUVImage> bottomIn;
    portals::InPortal<messages::JointAngles> jointsIn;
    portals::InPortal<messages::InertialState> inertsIn;

    ImageFrontEnd* getFrontEnd(bool topCamera = true) const { return frontEnd[!topCamera]; }
    EdgeList* getEdges(bool topCamera = true) const { return edges[!topCamera]; }
    HoughLineList* getHoughLines(bool topCamera = true) const { return houghLines[!topCamera]; }
    FieldLineList* getFieldLines(bool topCamera = true) const { return fieldLines[!topCamera]; }
    GoalboxDetector* getBox(bool topCamera = true) const { return boxDetector[!topCamera]; }
    CornerDetector* getCorners(bool topCamera = true) const { return cornerDetector[!topCamera]; }

    Colors* getColorsFromLisp(nblog::SExpr* colors, int camera);
    const std::string getStringFromTxtFile(std::string path);
    void setCameraParams(std::string robotName);
    void setCameraParams(int camera, std::string robotName);
    void setCameraParams(CameraParams* params, bool topCamera) { cameraParams[!topCamera] = params; }

    // For use by Image nbcross func
    void setColorParams(Colors* colors, bool topCamera) { colorParams[!topCamera] = colors; }

 

protected:
    virtual void run_();

private:

    void logImage(int i);
    
    Colors* colorParams[2];
    ImageFrontEnd* frontEnd[2];
    EdgeDetector* edgeDetector[2];
    EdgeList* edges[2];
    HoughLineList* houghLines[2];
    HoughSpace* hough[2];
    CameraParams* cameraParams[2];
    Kinematics* kinematics[2];
    FieldHomography* homography[2];
    FieldLineList* fieldLines[2];
    GoalboxDetector* boxDetector[2];
    CornerDetector* cornerDetector[2];

    nblog::SExpr* cameraLisp;
    size_t image_index;
};

}
}
