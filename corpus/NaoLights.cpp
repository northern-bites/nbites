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

void NaoLights::sendLights(){

    //Left Eye
    for(unsigned int i = 0; i < ALNames::NUM_ONE_EYE_LEDS; i++){
        leftFaceLedCommand[5][i][0] = OFF;
    }

    leftFaceLedCommand[4][0] = dcmProxy->getTime(20);

#ifdef LEDS_ENABLED
    try {
        dcmProxy->setAlias(leftFaceLedCommand);
    } catch(AL::ALError& e) {
        std::cout << "dcm value set error " << e.toString() << std::endl;
    }
#endif
}

/**
 * Creates the appropriate aliases with the DCM
 */
void NaoLights::initDCMAliases(){
    ALValue leftFaceLightsAlias;
    leftFaceLightsAlias.arraySetSize(3);
    leftFaceLightsAlias[0] = string("LeftFaceLeds");
    leftFaceLightsAlias[1].arraySetSize(ALNames::NUM_FACE_LEDS*
                                        ALNames::NUM_LED_COLORS);
    int faceIndex = 0;
    //Left Eye
    for(unsigned int c = 0; c < ALNames::NUM_LED_ORIENTATIONS; c++){
        for(unsigned int n = 0; n < ALNames::NUM_FACE_LEDS; n++){
            leftFaceLightsAlias[1][faceIndex] =
                ALNames::faceL[ALNames::LEFT_LED][c][n];
            faceIndex++;
        }
    }

    dcmProxy->createAlias(leftFaceLightsAlias);
}

/**
 *  Initialize the relatively 'static' parts of each command since we
 *  don't want to be allocating any memory during runtime
 *
 */
void NaoLights::initDCMCommands(){
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
        leftFaceLedCommand[5][i][0]  = OFF;
    }
}

