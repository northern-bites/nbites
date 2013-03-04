#include "NewMan.h"
#include <iostream>

namespace man {

Man::Man(boost::shared_ptr<AL::ALBroker> broker, const std::string &name)
    : AL::ALModule(broker, name),
      localizationThread("localization"),
      sensorsThread("sensors"),
      sensors(broker)
{
    setModuleDescription("The Northern Bites' soccer player.");
    sensorsThread.addModule(sensors);
    localizationThread.addModule(fOutput);
    localizationThread.addModule(localization);

    localization.motionInput.wireTo(&fOutput.fMotionOutput);
    localization.visionInput.wireTo(&fOutput.fVisionOutput);

    localizationThread.log<messages::RobotLocation>(&localization.output, "localization");
//    sensorsThread.log<messages::JointAngles>(&sensors.jointsOutput_, "joints");
    sensorsThread.start();
    localizationThread.start();
}

Man::~Man()
{
}

}
