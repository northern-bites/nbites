#pragma once

#include "RoboGrams.h"
#include "Camera.h"
#include "Images.h"
#include "PMotion.pb.h"
#include "Vision.pb.h"
#include "FrontEnd.h"
#include "Edge.h"
#include "Hough.h"
#include "Kinematics.h"
#include "Homography.h"

namespace man {
namespace vision {

class VisionModule : public portals::Module {
public:
    VisionModule(int wd, int ht);
    virtual ~VisionModule();

    portals::InPortal<messages::YUVImage> topIn;
    portals::InPortal<messages::YUVImage> bottomIn;
    portals::InPortal<messages::JointAngles> jointsIn;

    portals::OutPortal<messages::FieldLines> linesOut;

    ImageFrontEnd* getFrontEnd(bool topCamera = true) const { return frontEnd[!topCamera]; }
    EdgeList* getEdges(bool topCamera = true) const { return edges[!topCamera]; }
    HoughLineList* getHoughLines(bool topCamera = true) const { return houghLines[!topCamera]; }
    FieldLineList* getFieldLines(bool topCamera = true) const { return fieldLines[!topCamera]; }
    GoalboxDetector* getBox(bool topCamera = true) const { return boxDetector[!topCamera]; }
    CornerDetector* getCorners(bool topCamera = true) const { return cornerDetector[!topCamera]; }

protected:
    virtual void run_();

private:
    void sendLinesOut();

    Colors* colorParams[2];
    ImageFrontEnd* frontEnd[2];
    EdgeDetector* edgeDetector[2];
    EdgeList* edges[2];
    HoughLineList* houghLines[2];
    HoughSpace* hough[2];
    Kinematics* kinematics[2];
    FieldHomography* homography[2];
    FieldLineList* fieldLines[2];
    GoalboxDetector* boxDetector[2];
    CornerDetector* cornerDetector[2];
};

}
}
