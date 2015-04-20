#pragma once

#include "RoboGrams.h"
#include "Images.h"
#include "PMotion.pb.h"
#include "InertialState.pb.h"
#include "Gradient.h"

#include "VisionField.pb.h"
#include "BallModel.pb.h"
#include "VisionRobot.pb.h"

namespace man {
namespace vision {

class VisionModule : public portals::Module {

public:
    VisionModule();
    virtual ~VisionModule();

    portals::InPortal<messages::PackedImage16> topYImage;
    portals::InPortal<messages::PackedImage8> topWhiteImage;
    portals::InPortal<messages::PackedImage8> topOrangeImage;
    portals::InPortal<messages::PackedImage8> topGreenImage;

    portals::InPortal<messages::PackedImage16> bottomYImage;
    portals::InPortal<messages::PackedImage8> bottomOrangeImage;
    portals::InPortal<messages::PackedImage8> bottomWhiteImage;
    portals::InPortal<messages::PackedImage8> bottomGreenImage;

    portals::InPortal<messages::JointAngles> joints;
    portals::InPortal<messages::InertialState> inertials;

    portals::OutPortal<messages::VisionField> vision_field;
    portals::OutPortal<messages::VisionBall> vision_ball;
    portals::OutPortal<messages::VisionRobot> vision_robot;
    portals::OutPortal<messages::VisionObstacle> vision_obstacle;

protected:
    virtual void run_();

private:
    Gradient* gradient;
};

}
}
