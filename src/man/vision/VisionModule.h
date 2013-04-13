#pragma once

//#include <vector>
//#include <list>

#include "RoboGrams.h"

#include "Images.h"
#include "PMotion.pb.h"

#include "InertialState.pb.h"

#include "VisionField.pb.h"
#include "BallModel.pb.h"
#include "VisionRobot.pb.h"

#include "Vision.h"
#include "ConcreteCorner.h"
#include <boost/shared_ptr.hpp>


namespace man {
namespace vision{

class VisionModule : public portals::Module {

public:
    VisionModule();
    virtual ~VisionModule();

    portals::InPortal<messages::ThresholdImage> topThrImage;
	portals::InPortal<messages::PackedImage16> topYImage;
	portals::InPortal<messages::PackedImage16> topUImage;
	portals::InPortal<messages::PackedImage16> topVImage;

    portals::InPortal<messages::ThresholdImage> botThrImage;
	portals::InPortal<messages::PackedImage16> botYImage;
	portals::InPortal<messages::PackedImage16> botUImage;
	portals::InPortal<messages::PackedImage16> botVImage;

	portals::InPortal<messages::JointAngles> joint_angles;
    portals::InPortal<messages::InertialState> inertial_state;

    portals::OutPortal<messages::VisionField> vision_field;
    portals::OutPortal<messages::VisionBall> vision_ball;
    portals::OutPortal<messages::VisionRobot> vision_robot;
    portals::OutPortal<messages::VisionObstacle> vision_obstacle;

protected:
    virtual void run_();
    boost::shared_ptr<Vision> vision;
    void updateVisionField();
    void updateVisionBall();
    void updateVisionRobot();
    void updateVisionObstacle();
};

void updateRobot(messages::Robot* bot_message, VisualRobot* visualRobot);
}
}
