#pragma once

//#include <vector>
//#include <list>

#include "RoboGrams.h"

#include "ThresholdedImage.h"
#include "JointAngles.pb.h"
#include "InertialState.pb.h"

#include "VisionField.pb.h"
#include "VisionBall.pb.h"
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
		
		portals::InPortal<messages::ThresholdedImage> topImageIn;
		portals::InPortal<messages::ThresholdedImage> bottomImageIn;
		portals::InPortal<messages::JointAngles> joint_angles;
		portals::InPortal<messages::InertialState> inertial_state;

		portals::OutPortal<messages::VisionField> vision_field;
		portals::OutPortal<messages::VisionBall> vision_ball;
		portals::OutPortal<messages::VisionRobot> vision_robot;

	protected:
		virtual void run_();
		boost::shared_ptr<Vision> vision;
		void updateVisionField();
		void updateVisionBall();
		void updateVisionRobot();
	};
	
	void updateRobot(messages::VisionRobot::Robot* bot_message, VisualRobot* visualRobot);
}
}

