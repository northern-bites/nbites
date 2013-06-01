#include "LedEnactorModule.h"
#include <iostream>
#include "Profiler.h"

namespace man {
namespace led {

LedEnactorModule::LedEnactorModule(boost::shared_ptr<AL::ALBroker> broker)
    : portals::Module(),
      naoLights(broker)
{}

void LedEnactorModule::run_()
{
    ledCommandsIn.latch();
    messages::LedCommand command = ledCommandsIn.message();

    //std::cout<<command.DebugString()<<std::endl;

    // Loop through message and send each command
    // TODO: make this more robust if arrays don't have the same size
    PROF_ENTER(P_LIGHTS);
    for (int i=0; i<command.led_id_size(); i++) {
        naoLights.setRGB(command.led_id(i),command.rgb_hex(i));
    }
    // actually set robot leds
    naoLights.sendLights();

    PROF_EXIT(P_LIGHTS);
}

}
}
