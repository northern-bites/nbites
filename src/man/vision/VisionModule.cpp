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
}



}
}
