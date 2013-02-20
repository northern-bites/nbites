#include "NewMan.h"
#include <iostream>

namespace man {
using namespace sensors;

Man::Man(boost::shared_ptr<AL::ALBroker> broker, const std::string &name)
    : AL::ALModule(broker, name),
      sensorsThread("sensors"),
      sensors(broker)
{
    setModuleDescription("The Northern Bites' soccer player.");
    sensorsThread.addModule(sensors);
    sensorsThread.log<messages::InertialState>(&sensors.inertialsOutput_, "inertial");
    sensorsThread.start();
}

Man::~Man()
{
}

}
