#include "NewMan.h"
#include <iostream>

namespace man {

Man::Man(boost::shared_ptr<AL::ALBroker> broker, const std::string &name)
    : AL::ALModule(broker, name),
      cognitionThread(),
      sensorsThread(broker)
{
    setModuleDescription("The Northern Bites' soccer player.");
    std::cout << "Man constructor." << std::endl;
    //cognitionThread.start();
    sensorsThread.start();
}

Man::~Man()
{
    std::cout << "Man destructor." << std::endl;
}

}
