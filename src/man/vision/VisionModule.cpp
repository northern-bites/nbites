#include "VisionModule.h"
#include <iostream>


using namespace portals;


namespace man{
namespace vision{


VisionModule::VisionModule() : Module(),
							   topImageIn(),
							   bottomImageIn(),
							   joint_angles(),
							   inertial_state(),
							   vision_field(base()),
							   vision_ball(base()),
							   vision(boost::shared_ptr<Vision>(new Vision()))
{

}
VisionModule::~VisionModule()
{
}


void VisionModule::run_()
{
	topImageIn.latch();
	bottomImageIn.latch();
	joint_angles.latch();
	inertial_state.latch();

	vision->notifyImage(topImageIn.message().get_image(), bottomImageIn.message().get_image(),
						joint_angles.message(), inertial_state.message());

	updateVisionBall();
}

void VisionModule::updateVisionBall() {

	portals::Message<messages::VisionBall> ball_data(0);

	ball_data.get()->set_on(vision->ball->isTopCam());
	ball_data.get()->set_distance(vision->ball->getDistance());
	ball_data.get()->set_angle_x_deg(vision->ball->getAngleXDeg());
	ball_data.get()->set_angle_y_deg(vision->ball->getAngleYDeg());
	ball_data.get()->set_bearing(vision->ball->getBearing());
	ball_data.get()->set_elevation_deg(vision->ball->getElevationDeg());
	ball_data.get()->set_distance_sd(vision->ball->getDistanceSD());
	ball_data.get()->set_bearing_sd(vision->ball->getBearingSD());
	ball_data.get()->set_radius(vision->ball->getRadius());
	ball_data.get()->set_frames_on(vision->ball->getFramesOn());
	ball_data.get()->set_frames_off(vision->ball->getFramesOff());
	ball_data.get()->set_heat(vision->ball->getHeat());
	ball_data.get()->set_on(vision->ball->isOn());
	ball_data.get()->set_confidence(vision->ball->getConfidence());


	vision_ball.setMessage(ball_data);

}



void VisionModule::updateVisionField() {

	portals::Message<messages::VisionField> field_data(0);
	
	const std::vector<boost::shared_ptr<VisualLine> >* visualLines = vision->fieldLines->getLines();
	std::list<VisualCorner>* visualCorners = vision->fieldLines->getCorners();
	


}


}
}
