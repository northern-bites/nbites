#include "NewMan.h"
#include <iostream>

#include "jointenactor/JointEnactorModule.h"

namespace man {

Man::Man(boost::shared_ptr<AL::ALBroker> broker, const std::string &name)
    : AL::ALModule(broker, name),
      sensorsThread("sensors"),
      sensors(broker)
{
    setModuleDescription("The Northern Bites' soccer player.");

    sensorsThread.addModule(sensors);
    sensorsThread.log<messages::JointAngles>(&sensors.jointsOutput_, "joints");
    sensorsThread.start();
}

Man::~Man()
{
}

}
