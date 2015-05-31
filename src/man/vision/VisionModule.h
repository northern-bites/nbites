#pragma once

#include "RoboGrams.h"
#include "Images.h"
#include "PMotion.pb.h"
#include "InertialState.pb.h"
#include "FrontEnd.h"
#include "Edge.h"
#include "Hough.h"

namespace man {
namespace vision {

// TODO top and bottom camera
// TODO namespace
// TODO fast bool
// TODO compiles on both ubuntu and windows
class VisionModule : public portals::Module {
public:
    VisionModule();
    virtual ~VisionModule();

    // TODO why reference to messages is ambiguous?
    portals::InPortal< ::messages::YUVImage> imageIn;
    portals::InPortal< ::messages::JointAngles> jointsIn;
    portals::InPortal< ::messages::InertialState> inertialsIn;

protected:
    virtual void run_();

private:
    Colors* colorParams;
    ImageFrontEnd* frontEnd;
    EdgeDetector* edgeDetector;
    EdgeList* edges;
    HoughLineList* houghLines;
    HoughSpace* hough;
};

}
}
