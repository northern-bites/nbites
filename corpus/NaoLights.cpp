#include "NaoLights.h"
#include "ALLedNames.h"

#define LEDS_ENABLED
#define DEBUG_NAOLIGHTS_INIT

NaoLights::NaoLights(AL::ALPtr<AL::ALBroker> broker)
    :Lights()
{
    try {
        dcmProxy = AL::ALPtr<AL::DCMProxy>(new AL::DCMProxy(broker));
    } catch(AL::ALError &e) {
        cout << "Failed to initialize proxy to DCM" << endl;
    }

    initDCMAliases();
    initDCMCommands();
}


NaoLights::~NaoLights(){
    for(unsigned int i = 0; i < ALNames::NUM_UNIQUE_LEDS; i++){
        delete ledList[i];
    }
}

void NaoLights::setRGB(std::string led_id, int rgbHex){


}

void NaoLights::sendLights(){

    //Left Eye
    updateLightCommand(leftFaceLedCommand,0,ALNames::NUM_FACE_LEDS);
    sendLightCommand(leftFaceLedCommand);
}

void NaoLights::generateLeds(){
    for(unsigned int i = 0; i < ALNames::NUM_UNIQUE_LEDS; i++){

        ledList.push_back(new NaoRGBLight(LED_NAMES[i],
                                          i,
                                          ALNames::NUM_RGB_LEDS[i],
                                          ALNames::LED_START_COLOR[i],
                                          ALNames::LED_END_COLOR[i]));
    }
}

void NaoLights::updateLightCommand(ALValue & command, const int rgbHex,
                                   const unsigned int numRGBLeds,
                                   const unsigned int startColor,
                                   const unsigned int endColor){
    unsigned int ledIndex = 0;
    for(unsigned int c = startColor; c < endColor; c++){
        for(unsigned int led = 0; led < numRGBLeds; led++){
            const float color =
                getColor(static_cast<ALNames::LedColor>(c),rgbHex);
            command[5][ledIndex][0] = color;
            ledIndex++;
        }
    }
}

void NaoLights::sendLightCommand(ALValue & command){
    command[4][0] = dcmProxy->getTime(20);
#ifdef LEDS_ENABLED
    try {
        dcmProxy->setAlias(leftFaceLedCommand);
    } catch(AL::ALError& e) {
        std::cout << "dcm value set error " << e.toString() << std::endl;
    }
#endif

}

/*
 * Returns a float between 0.0 and 1.0 corresponding to the 'c' channel
 * of the hex value 
 */
const float NaoLights::getColor(const ALNames::LedColor c, const int rgbHex){
    return NaoRGBLight::OFF;
}

/**
 * Creates the appropriate aliases with the DCM
 */
void NaoLights::initDCMAliases(){
#ifdef DEBUG_NAOLIGHTS_INIT
    std::cout << "  NaoLights::initDCMAliases() start" << std::endl;
#endif
    ALValue leftFaceLightsAlias;
    leftFaceLightsAlias.arraySetSize(3);
    leftFaceLightsAlias[0] = string("LeftFaceLeds");
    leftFaceLightsAlias[1].arraySetSize(ALNames::NUM_FACE_LEDS*
                                        ALNames::NUM_LED_COLORS);
    int faceIndex = 0;
    //Left Eye
    for(unsigned int c = 0; c < ALNames::NUM_LED_COLORS; c++){
        for(unsigned int n = 0; n < ALNames::NUM_FACE_LEDS; n++){
            leftFaceLightsAlias[1][faceIndex] =
                ALNames::faceL[ALNames::LEFT_LED][c][n];
            cout << ALNames::faceL[ALNames::LEFT_LED][c][n] <<endl;

            faceIndex++;
        }
    }

    dcmProxy->createAlias(leftFaceLightsAlias);
#ifdef DEBUG_NAOLIGHTS_INIT
    std::cout << "  NaoLights::initDCMAliases() end" << std::endl;
#endif
}

/**
 *  Initialize the relatively 'static' parts of each command since we
 *  don't want to be allocating any memory during runtime
 *
 */
void NaoLights::initDCMCommands(){
#ifdef DEBUG_NAOLIGHTS_INIT
    std::cout << "  NaoLights::initDCMCommands() start" << std::endl;
#endif
    //Left Eye
    leftFaceLedCommand.arraySetSize(6);
    leftFaceLedCommand[0] = string("LeftFaceLeds");
    leftFaceLedCommand[1] = string("ClearAll");
    leftFaceLedCommand[2] = string("time-separate");
    leftFaceLedCommand[3] = 0; //importance level
    leftFaceLedCommand[4].arraySetSize(1); //list of time to send commands
    leftFaceLedCommand[5].arraySetSize(ALNames::NUM_ONE_EYE_LEDS);
    for(unsigned int i = 0; i<ALNames::NUM_ONE_EYE_LEDS; i++){
        leftFaceLedCommand[5][i].arraySetSize(1);
        leftFaceLedCommand[5][i][0]  = NaoRGBLight::OFF;
    }

#ifdef DEBUG_NAOLIGHTS_INIT
    std::cout << "  NaoLights::initDCMCommands() end" << std::endl;
#endif
}

