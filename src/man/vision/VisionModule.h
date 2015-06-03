#pragma once

#include "RoboGrams.h"
#include "Camera.h"
#include "Images.h"
#include "PMotion.pb.h"
#include "InertialState.pb.h"
#include "FrontEnd.h"
#include "Edge.h"
#include "Hough.h"
#include "Homography.h"

namespace man {
namespace vision {

// TODO compile on both ubuntu and windows
class VisionModule : public portals::Module {
public:
    VisionModule();
    virtual ~VisionModule();

    portals::InPortal<messages::YUVImage> topIn;
    portals::InPortal<messages::YUVImage> bottomIn;
    portals::InPortal<messages::JointAngles> jointsIn;
    portals::InPortal<messages::InertialState> inertialsIn;

    // TODO better debug
    ImageFrontEnd* getFrontEnd() const { return frontEnd[1]; }
    EdgeList* getEdges() const { return edges[1]; }

protected:
    virtual void run_();

private:
    Colors* colorParams[2];
    ImageFrontEnd* frontEnd[2];
    EdgeDetector* edgeDetector[2];
    EdgeList* edges[2];
    HoughLineList* houghLines[2];
    HoughSpace* hough[2];
    FieldHomography* homography[2];
    FieldLineList* fieldLines[2];
};

}
}
