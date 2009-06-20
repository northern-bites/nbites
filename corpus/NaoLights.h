#ifndef NaoLights_h_DEFINED
#define NaoLights_h_DEFINED

#include <pthread.h>

#include "Lights.h"
#include "dcmproxy.h"
#include "ALLedNames.h"
#include "NaoRGBLight.h"
/**
 *  This class implements LED capability on the Nao robot using the DCM in Naoqi
 *  @author Johannes Strom
 *  @date June 2009
 */



class NaoLights : public Lights{
public:
    NaoLights(AL::ALPtr<AL::ALBroker> broker);
    virtual ~NaoLights();

public:
    void setRGB(const std::string led_id, const int newRgbHex);
    void setRGB(const unsigned int led_id, const int newRgbHex);

    void sendLights();
private:
    void generateLeds();
    void initDCMAliases();
    void initDCMCommands();
    const float getColor(const ALNames::LedColor c, const int rgbHex);
    void updateLightCommand(ALValue &command, const int rgbHex,
                            const unsigned int numRGBLeds,
                            const unsigned int startColor = 0,
                            const unsigned int endColor =
                            ALNames::NUM_LED_COLORS);
    void sendLightCommand(ALValue &command);
private:
    AL::ALPtr<AL::DCMProxy> dcmProxy;
    AL::ALValue leftFaceLedCommand;
    std::vector<NaoRGBLight*> ledList;
    std::vector<int> hexList;

    mutable pthread_mutex_t lights_mutex;
};

#endif
