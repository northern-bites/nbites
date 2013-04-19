#include "NaoRGBLight.h"

using std::string;
//#define DEBUG_NAOLIGHTS_INIT

const float NaoRGBLight::LED_ON = 1.0f;
const float NaoRGBLight::LED_OFF = 0.0f;

NaoRGBLight::NaoRGBLight(const string _NBLedName,
                         const unsigned int _NBLedID,
                         const unsigned int numSubLeds,
                         const unsigned int _startColor,
                         const unsigned int _endColor)
    :newValue(true),
     rgbHex(0),
     NBLedName(_NBLedName),
     NBLedID(_NBLedID),
     numRGBSubLeds(numSubLeds),
     startColor(_startColor),
     endColor(_endColor)
{
#ifdef DEBUG_NAOLIGHTS_INIT
    std::cout << "  NaoRGBLight()-"<<NBLedName << ", id="
              << NBLedID << ", numSubLeds"<<numSubLeds
              << ", startC = "<<startColor<< ", endC=" << endColor <<std::endl;
#endif
    makeAlias();
    makeCommand();
}


NaoRGBLight::~NaoRGBLight(){}

/**
 * Each time a new command is sent to this LED, we check to see if it
 * will change the color of the LED. If so, we update the ALValue command
 * and return true so that NaoLights will send the command to the DCM
 */
bool NaoRGBLight::updateCommand(const int newRgbHex){
    if(newRgbHex == rgbHex){
        return false;
    }
    rgbHex = newRgbHex;

    unsigned int ledIndex = 0;
    for(unsigned int c = startColor; c < endColor; c++){
        for(unsigned int led = 0; led < numRGBSubLeds; led++){
			const float color =
                getColor(static_cast<ALNames::LedColor>(c),rgbHex);
            command[5][ledIndex][0] = color;
            ledIndex++;
        }
    }

    return true;
}

/*
 * Returns a float between 0.0 and 1.0 corresponding to the 'c' channel
 * of the hex value
 */
const float NaoRGBLight::getColor(const ALNames::LedColor c,
                                  const int rgbHex) const{
    const int channelColor = (rgbHex >> ((2-c)*8)) % 256;
    const float fchannelColor= static_cast<float>(channelColor);
    return fchannelColor * (1.0f/255.0f);
}

/**
 * Each NBLedName is actually many individual leds. We need to alias the large
 * group of leds to one name with the DCM. Then we can send commands directly
 * just to that group.
 */
void NaoRGBLight::makeAlias(){
#ifdef DEBUG_NAOLIGHTS_INIT
    std::cout << "  NaoRGBLight::makeAlias()-"<<NBLedName << ":"<<std::endl;
#endif
    alias.arraySetSize(2);
    alias[0] = string(NBLedName);

    const int numTotLeds = numRGBSubLeds*(endColor - startColor);
    alias[1].arraySetSize(numTotLeds);
    int index = 0;
#ifdef DEBUG_NAOLIGHTS_INIT
    std::cout << "  NaoRGBLight::makeAlias()-numTotLeds"<<numTotLeds<<std::endl;
#endif
    for(unsigned int c = startColor; c < endColor; c++){
        for(unsigned int n = 0; n < numRGBSubLeds; n++){
            const unsigned int subIndex = (c-startColor)*numRGBSubLeds + n;
#ifdef DEBUG_NAOLIGHTS_INIT
            std::cout << "    " <<ALNames::RGB_LED_STRINGS[NBLedID][subIndex]
                      << std::endl;
#endif
            alias[1][index] = ALNames::RGB_LED_STRINGS[NBLedID][subIndex];
            index++;
        }
    }
}

/**
 *  This sets up the command which will be sent by NaoLights each time
 *  the color of this LED is updated.
 */
void NaoRGBLight::makeCommand(){
#ifdef DEBUG_NAOLIGHTS_INIT
    std::cout << "  NaoRGBLights::makeCommand()-"
              << NBLedName <<":"<< std::endl;
#endif
    command.arraySetSize(6);
    command[0] = string(NBLedName);
    command[1] = string("ClearAll");
    command[2] = string("time-separate");
    command[3] = 0; //importance level
    command[4].arraySetSize(1); //list of time to send commands
    const unsigned int numLedsTotal = numRGBSubLeds* (endColor - startColor);
#ifdef DEBUG_NAOLIGHTS_INIT
    std::cout << "  NaoRGBLights::makeCommand()-numToLeds"
              << numLedsTotal <<":"<< std::endl;
#endif
    command[5].arraySetSize(numLedsTotal);
    for(unsigned int i = 0; i< numLedsTotal; i++){
        command[5][i].arraySetSize(1);
        command[5][i][0] = LED_OFF;
    }
}
