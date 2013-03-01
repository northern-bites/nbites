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
	  test(),
	  audio(broker,test.AudioOut)
{
    setModuleDescription("The Northern Bites' soccer player.");

    /** Sensors **/
    sensorsThread.addModule(sensors);
    sensorsThread.log<messages::JointAngles>(&sensors.jointsOutput_, "joints");

    /** Comm **/
    commThread.addModule(comm);

	/** Cognition **/
	cognitionThread.addModule(test);
	cognitionThread.addModule(audio);
	audio.AudioIn.wireTo(&test.AudioOut);

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
