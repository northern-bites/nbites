#ifndef Lights_h_DEFINED
#define Lights_h_DEFINED

#include <string>
#include <vector>

class Lights{
public:
    Lights(){};
    virtual ~Lights(){};

public:
    virtual void setRGB(std::string led_id, int rdbHex) = 0;
    virtual void postLights() = 0;

    static const unsigned int NUM_LED_NAMES = 7;
    static const std::string LED_NAMES[NUM_LED_NAMES];
};

const std::string Lights::LED_NAMES[Lights::NUM_LED_NAMES] = {
    "LeftEar","RightEar",
    "LeftEye","RightEye",
    "Chest",
    "LeftFoot", "RightFoot"};
#endif
