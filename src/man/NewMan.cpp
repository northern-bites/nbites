#include "NewMan.h"
#include <iostream>

namespace man {

Man::Man(boost::shared_ptr<AL::ALBroker> broker, const std::string &name)
    : AL::ALModule(broker, name),
      fakeShitThread("fakeShit"),
      sensorsThread("sensors"),
      sensors(broker)
{
    setModuleDescription("The Northern Bites' soccer player.");
    sensorsThread.addModule(sensors);
    fakeShitThread.addModule(fInput);
    fakeShitThread.log<messages::Motion>(&fInput.fMotionOutput, "fOdometry");
//    sensorsThread.log<messages::JointAngles>(&sensors.jointsOutput_, "joints");
    sensorsThread.start();
    fakeShitThread.start();
}

Man::~Man()
{
}

}
