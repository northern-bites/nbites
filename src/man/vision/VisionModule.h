#pragma once

namespace man {
namespace vision {

class VisionModule : public portals::Module {
public:
    VisionModule();
    virtual ~VisionModule();

    portals::InPortal<messages::PackedImage16> yImageIn;
    portals::InPortal<messages::PackedImage8> whiteImageIn;
    portals::InPortal<messages::PackedImage8> greenImageIn;
    portals::InPortal<messages::PackedImage8> orangeImageIn;

    portals::InPortal<messages::JointAngles> jointsIn;
    portals::InPortal<messages::InertialState> inertialsIn;

protected:
    virtual void run_();

private:
    EdgeDetector* edgeDetector;
    EdgeList* edges;
    HoughLineList* houghLines;
    HoughSpace* hough;
};

}
}
