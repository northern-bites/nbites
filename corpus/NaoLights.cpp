#include "NaoLights.h"


NaoLights::NaoLights(AL::ALPtr<AL::ALBroker> broker)
    :Lights()
{
    try {
        dcmProxy = AL::ALPtr<AL::DCMProxy>(new AL::DCMProxy(broker));
    } catch(AL::ALError &e) {
        cout << "Failed to initialize proxy to DCM" << endl;
    }
}


NaoLights::~NaoLights(){}

void NaoLights::setRGB(std::string led_id, int rgbHex){


}
