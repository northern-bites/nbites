#include "LedEnactor.h"
#include <iostream>

namespace boss {
namespace led {

LedEnactor::LedEnactor(boost::shared_ptr<AL::ALBroker> broker)
    : naoLights(broker)
{}

void LedEnactor::setLeds(messages::LedCommand command)
{
    // Loop through message and send each command
    // TODO: make this more robust if arrays don't have the same size
    for (int i=0; i<command.led_id_size(); i++) {
        naoLights.setRGB(command.led_id(i),command.rgb_hex(i));
    }
    // actually set robot leds
    naoLights.sendLights();
}

}
}
