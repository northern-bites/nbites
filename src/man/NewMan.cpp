#include "NewMan.h"
#include <iostream>

namespace man {
using namespace sensors;
using namespace vision;
using namespace image;

Man::Man(boost::shared_ptr<AL::ALBroker> broker, const std::string &name)
    : AL::ALModule(broker, name),
      sensorsThread("sensors"),
      sensors(broker),
	  imageTranscriber(),
	  vision()
{
	vision.topImageIn.wireTo(&imageTranscriber.topImageOut);
	vision.bottomImageIn.wireTo(&imageTranscriber.bottomImageOut);
	vision.joint_angles.wireTo(&sensors.jointsOutput_);
	vision.inertial_state.wireTo(&sensors.inertialsOutput_);
    setModuleDescription("The Northern Bites' soccer player.");
    sensorsThread.addModule(sensors);
	sensorsThread.addModule(imageTranscriber);
	sensorsThread.addModule(vision);
    sensorsThread.log<messages::JointAngles>(&sensors.jointsOutput_, "joints");
    sensorsThread.start();
}

Man::~Man()
{
}

}
 
