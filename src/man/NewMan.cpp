#include "NewMan.h"
#include "sensors/SensorsModule.h"
#include <iostream>

namespace man {

Man::Man(boost::shared_ptr<AL::ALBroker> broker, const std::string &name)
    : AL::ALModule(broker, name)
{
    setModuleDescription("The Northern Bites' soccer player.");
    std::cout << "Man constructor." << std::endl;

    sensors::SensorsModule sm(broker);
    sm.run();
}

Man::~Man()
{
    std::cout << "Man destructor." << std::endl;
}

}
