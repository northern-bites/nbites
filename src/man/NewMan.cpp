#include "NewMan.h"
#include <iostream>

namespace man {
using namespace sensors;

Man::Man(boost::shared_ptr<AL::ALBroker> broker, const std::string &name)
    : AL::ALModule(broker, name),
      //cognitionThread(),
      sensorsThread("sensors"),
      sensors(broker)
{
    setModuleDescription("The Northern Bites' soccer player.");
    //cognitionThread.start();
    sensorsThread.addModule(sensors);
    sensorsThread.start();
}

Man::~Man()
{
}

}
