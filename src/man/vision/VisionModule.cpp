#include "VisionModule.h"
#include <iostream>


using namespace portals;


namespace man{
namespace vision{


  VisionModule::VisionModule() : Module(),
				 pose(boost::shared_ptr<NaoPose>(new NaoPose()))
{
  vision = boost::shared_ptr<Vision> (new Vision(pose));

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
