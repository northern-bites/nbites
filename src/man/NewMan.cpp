#include "NewMan.h"
#include <iostream>
#include "RobotConfig.h"

namespace man {

Man::Man(boost::shared_ptr<AL::ALBroker> broker, const std::string &name)
    : AL::ALModule(broker, name),
      sensorsThread("sensors"),
      sensors(broker),
      guardianThread("guardian"),
      guardian(),
      commThread("comm"),
      comm(MY_TEAM_NUMBER, MY_PLAYER_NUMBER)
{
    setModuleDescription("The Northern Bites' soccer player.");

    /** Sensors **/
    sensorsThread.addModule(sensors);
    sensorsThread.log<messages::JointAngles>(&sensors.jointsOutput_, "joints");

    /** Guardian **/
    guardianThread.addModule(guardian);
    guardian.jointsInput.wireTo(&sensors.jointsOutput_, true);
    guardian.chestButtonInput.wireTo(&sensors.chestboardButtonOutput_, true);
    guardian.footBumperInput.wireTo(&sensors.footbumperOutput_, true);
    guardian.inertialInput.wireTo(&sensors.inertialsOutput_, true);

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
    guardianThread.start();
    commThread.start();
}

}
