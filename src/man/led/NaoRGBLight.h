#ifndef NaoRGBLight_h_DEFINED
#define NaoRGBLight_h_DEFINED

#include <string>
#include "alvalue/alvalue.h"
#include "ALLedNames.h"

class NaoRGBLight{
 public:
    NaoRGBLight(const std::string _NBLedName,
                const unsigned int _NBLedID,
                const unsigned int numSubLeds,
                const unsigned int _startColor = 0,
                const unsigned int _endColor = ALNames::NUM_LED_COLORS);

    virtual ~NaoRGBLight();

    bool updateCommand(const int newRgbHex);

    AL::ALValue * getAlias(){return &alias;}
    AL::ALValue * getCommand(){return &command;}

 private:
    void makeAlias();
    void makeCommand();
    const float getColor(const ALNames::LedColor c, const int rgbHex) const;

 private:
    AL::ALValue command;
    AL::ALValue alias;
    bool newValue;
    int rgbHex;

    const std::string NBLedName;
    const unsigned int NBLedID;
    const unsigned int numRGBSubLeds;
    const unsigned int startColor;
    const unsigned int endColor;

 public:
    static const float LED_ON;
    static const float LED_OFF;

};
#endif
