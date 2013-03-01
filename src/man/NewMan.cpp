#include "NewMan.h"
#include <iostream>
#include "RobotConfig.h"

namespace man {

Man::Man(boost::shared_ptr<AL::ALBroker> broker, const std::string &name)
    : AL::ALModule(broker, name),
      sensorsThread("sensors"),
      sensors(broker),
      commThread("comm"),
      comm(MY_TEAM_NUMBER, MY_PLAYER_NUMBER)
	  imageTranscriber(),
	  vision()
{
	vision.topImageIn.wireTo(&imageTranscriber.topImageOut);
	vision.bottomImageIn.wireTo(&imageTranscriber.bottomImageOut);
	vision.joint_angles.wireTo(&sensors.jointsOutput_);
	vision.inertial_state.wireTo(&sensors.inertialsOutput_);
    setModuleDescription("The Northern Bites' soccer player.");

    /** Sensors **/
    sensorsThread.addModule(sensors);
	sensorsThread.addModule(imageTranscriber);
	sensorsThread.addModule(vision);
    sensorsThread.log<messages::JointAngles>(&sensors.jointsOutput_, "joints");

    /** Comm **/
    commThread.addModule(comm);

    startSubThreads();
}

Man::~Man()
{
}

void Man::startSubThreads()
{
    sensorsThread.start();
    commThread.start();
}

}
 
