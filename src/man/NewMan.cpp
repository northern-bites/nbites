#include "NewMan.h"
#include <iostream>

namespace man {

Man::Man(boost::shared_ptr<AL::ALBroker> broker, const std::string &name)
    : AL::ALModule(broker, name),
      sensorsThread("sensors"),
      sensors(broker),
      cognitionThread("cog"),
      image()
{
    setModuleDescription("The Northern Bites' soccer player.");
    sensorsThread.addModule(sensors);
    sensorsThread.log<messages::JointAngles>(&sensors.jointsOutput_, "joints");
    cognitionThread.addModule(image);

    sensorsThread.start();
    cognitionThread.start();
}

Man::~Man()
{
}

}
