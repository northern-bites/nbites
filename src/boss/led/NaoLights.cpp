#include "alerror/alerror.h"
#include "NaoLights.h"
#include "ALLedNames.h"
#include <iostream>

#define LEDS_ENABLED
// #define DEBUG_NAOLIGHTS_INIT
// #define DEBUG_NAOLIGHTS_COMMAND

NaoLights::NaoLights(boost::shared_ptr<AL::ALBroker> broker)
    :Lights(),
     hexList(ALNames::NUM_UNIQUE_LEDS,0x000000)
{
    try {
        dcmProxy = boost::shared_ptr<AL::DCMProxy>(new AL::DCMProxy(broker));
    } catch(AL::ALError &e) {
        std::cout << "Failed to initialize proxy to DCM" << std::endl;
    }

    generateLeds();
    //We need to set each LED initially, no matter what
    for(unsigned int i = 0; i < ALNames::NUM_UNIQUE_LEDS; i++){
        sendLightCommand(*ledList[i]->getCommand());
    }
}

NaoLights::~NaoLights(){
    for(unsigned int i = 0; i < ALNames::NUM_UNIQUE_LEDS; i++){
        delete ledList[i];
    }
}

/**
 * This method generates all the NaoRGBLed objects for each LED group
 */
void NaoLights::generateLeds(){
    for(unsigned int i = 0; i < ALNames::NUM_UNIQUE_LEDS; i++){
        std::cout << LED_NAMES[i] << std::endl;
        // not creating valid naorbglight objects for the new group numbers
        ledList.push_back(new NaoRGBLight(LED_NAMES[i],
                                          i,
                                          1,
                                          ALNames::LED_START_COLOR[i],
                                          ALNames::LED_END_COLOR[i]));
        ledList.at(i)->printInfo();

        try {
            AL::ALValue newAlias = *ledList[i]->getAlias();
            dcmProxy->createAlias(newAlias);
        } catch (AL::ALError &e){
            std::cout << "dcm error in generateLeds"
                      << e.toString() << std::endl;
        }
    }

    std::cout << ledList.size() << std::endl;
}

void NaoLights::setRGB(const std::string led_id, const int newRgbHex){
    //Slow, but there are only 31 leds, so it shouldn't be too bad...
    for(unsigned int i = 0; i < ALNames::NUM_UNIQUE_LEDS; i++){
        if(LED_NAMES[i].compare(led_id) == 0){
            hexList[i] = newRgbHex;
            break;
        }
    }
}

void NaoLights::setRGB(const unsigned int led_id, const int newRgbHex){
    hexList[led_id] = newRgbHex;
    std::cout << "hexList[" << led_id << "] = 0x" << std::hex << newRgbHex << std::endl;
}

void NaoLights::sendLights(){
#ifdef DEBUG_NAOLIGHTS_INIT
    std::cout << "  NaoLights::sendLights() start" << std::endl;
#endif

    for(unsigned int i = 0; i < ALNames::NUM_UNIQUE_LEDS; i++){ //HAack
        if(ledList[i]->updateCommand(hexList[i])){ // only true under the before number
            std::cout << "sending led light command for group " << i << std::endl;
            sendLightCommand(*ledList[i]->getCommand());
        }
    }

    //sendLightCommand(*(ledList[0]->getCommand()));
#ifdef DEBUG_NAOLIGHTS_INIT
    std::cout << "  NaoLights::sendLights() end" << std::endl;
#endif
}

void NaoLights::sendLightCommand(AL::ALValue & command){
#ifdef DEBUG_NAOLIGHTS_COMMAND
    std::cout << "  NaoLights::sendCommand() " <<command.serializeToText()<< std::endl;
#endif

    try{
        command[4][0] = dcmProxy->getTime(0);

#ifdef LEDS_ENABLED
        dcmProxy->setAlias(command);
#endif

    } catch(AL::ALError& e) {
        std::cout << "dcm value set error in sendLightCommand:"
                  << e.toString() << std::endl;
    }
}
