#include "NaoLights.h"
#include "ALLedNames.h"

#define LEDS_ENABLED
//#define DEBUG_NAOLIGHTS_INIT
//#define DEBUG_NAOLIGHTS_COMMAND

NaoLights::NaoLights(AL::ALPtr<AL::ALBroker> broker)
    :Lights(),
     hexList(ALNames::NUM_UNIQUE_LEDS,0x000000)
{
    try {
        dcmProxy = AL::ALPtr<AL::DCMProxy>(new AL::DCMProxy(broker));
    } catch(AL::ALError &e) {
		std::cout << "Failed to initialize proxy to DCM" << std::endl;
    }

    generateLeds();
    //We need to set each LED initially, no matter what
    for(unsigned int i = 0; i < ALNames::NUM_UNIQUE_LEDS; i++){
        sendLightCommand(*ledList[i]->getCommand());
    }

    pthread_mutex_init(&lights_mutex,NULL);
}


NaoLights::~NaoLights(){
    for(unsigned int i = 0; i < ALNames::NUM_UNIQUE_LEDS; i++){
        delete ledList[i];
    }
    pthread_mutex_destroy(&lights_mutex);
}

void NaoLights::setRGB(const std::string led_id, const int newRgbHex){
    pthread_mutex_lock(&lights_mutex);
    //Slow, but there are only 7 leds, so it shouldn't be too bad...
    for(unsigned int i = 0; i < ALNames::NUM_UNIQUE_LEDS; i++){
        if(LED_NAMES[i].compare(led_id) == 0){
            hexList[i] = newRgbHex;
            break;
        }
    }
    pthread_mutex_unlock(&lights_mutex);
}

void NaoLights::setRGB(const unsigned int led_id, const int newRgbHex){
    pthread_mutex_lock(&lights_mutex);
    hexList[led_id] = newRgbHex;
    pthread_mutex_unlock(&lights_mutex);
}

void NaoLights::sendLights(){
#ifdef DEBUG_NAOLIGHTS_INIT
    // std::cout << "  NaoLights::sendLights() start" << std::endl;
#endif
    pthread_mutex_lock(&lights_mutex);

    for(unsigned int i = 0; i < ALNames::NUM_UNIQUE_LEDS; i++){ //HAack
        if(ledList[i]->updateCommand(hexList[i])){
            sendLightCommand(*ledList[i]->getCommand());
        }
    }

    //sendLightCommand(*(ledList[0]->getCommand()));

    pthread_mutex_unlock(&lights_mutex);
#ifdef DEBUG_NAOLIGHTS_INIT
    // std::cout << "  NaoLights::sendLights() end" << std::endl;
#endif
}

/**
 * This method generates all the NaoRGBLed objects for each LED group
 */
void NaoLights::generateLeds(){
    for(unsigned int i = 0; i < ALNames::NUM_UNIQUE_LEDS; i++){
        ledList.push_back(new NaoRGBLight(LED_NAMES[i],
                                          i,
                                          ALNames::NUM_RGB_LEDS[i],
                                          ALNames::LED_START_COLOR[i],
                                          ALNames::LED_END_COLOR[i]));
        AL::ALValue newAlias = *ledList[i]->getAlias();
        dcmProxy->createAlias(newAlias);
    }
}

void NaoLights::sendLightCommand(AL::ALValue & command){
#ifdef DEBUG_NAOLIGHTS_COMMAND
    std::cout << "  NaoLights::sendCommand() " <<command.serializeToText()<< std::endl;
#endif
    command[4][0] = dcmProxy->getTime(20);
#ifdef LEDS_ENABLED
    try {
        dcmProxy->setAlias(command);
    } catch(AL::ALError& e) {
        std::cout << "dcm value set error " << e.toString() << std::endl;
    }
#endif

}
