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

    /* In order to keep logs synced up, joint angs and inert states are passed 
     * thru the vision system. Joint angles are taken at around 100 hz, but 
     * images are taken at 30 hz, but by passing joint angles thru vision we 
     * get joint angles at 30 hz. */
#ifdef LOG_VISION
    portals::OutPortal<messages::JointAngles> joint_angles_out;
    portals::OutPortal<messages::InertialState> inertial_state_out;
#endif

	portals::OutPortal<messages::ThresholdImage> topOutPic;
	portals::OutPortal<messages::ThresholdImage> botOutPic;

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
