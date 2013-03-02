#include "NewMan.h"
#include <iostream>
#include "RobotConfig.h"

namespace man {

Man::Man(boost::shared_ptr<AL::ALBroker> broker, const std::string &name)
    : AL::ALModule(broker, name),
      sensorsThread("sensors"),
      sensors(broker),
      commThread("comm"),
      comm(MY_TEAM_NUMBER, MY_PLAYER_NUMBER),
	  cognitionThread("cognition"),
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
	
	
	/** Cognition **/
	cognitionThread.addModule(imageTranscriber);
	cognitionThread.addModule(vision);

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
	cognitionThread.start();
}

}
 
