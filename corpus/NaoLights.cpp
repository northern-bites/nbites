#include "NaoLights.h"
#include "ALLedNames.h"

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

void NaoLights::postLights(){


}


void NaoLights::initDCMAliases(){
    ALValue faceLightsAlias;
    faceLightsAlias.arraySetSize(3);
    faceLightsAlias[0] = string("AllFaceValues");
    faceLightsAlias[1].arraySetSize(ALNames::NUM_FACE_LEDS*
                                    ALNames::NUM_LED_COLORS*
                                    ALNames::NUM_LED_ORIENTATIONS);
    int index = 0;
    for(unsigned int o = 0; o < ALNames::NUM_LED_ORIENTATIONS; o++){
        for(unsigned int c = 0; c < ALNames::NUM_LED_ORIENTATIONS; c++){
            for(unsigned int n = 0; n < ALNames::NUM_FACE_LEDS; n++){
                faceLightsAlias[1][index] = ALNames::faceL[o][c][n];
                index++;
            }
        }
    }
}

void NaoLights::initDCMCommands(){

}
