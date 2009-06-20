#ifndef Lights_h_DEFINED
#define Lights_h_DEFINED

#include <string>
#include <vector>

/**
 * This class serves as an abstract, purely-virtual class to implement LED
 * access on various platforms. Interested parties should make subclasses
 * of this which connect with the approriate low level LED API on the robot.
 * For an example, see NaoLights.
 *
 * @author Johannes Strom
 * @date June 2009
 */
class Lights{
public:
    Lights(){};
    virtual ~Lights(){};

public:
    virtual void setRGB(std::string led_id, int rdbHex) = 0;
    virtual void sendLights() = 0;

    static const unsigned int NUM_LED_NAMES = 7;
    static const std::string LED_NAMES[NUM_LED_NAMES];

    static const float ON = 1.0f;
    static const float OFF = 0.0f;

};

const std::string Lights::LED_NAMES[Lights::NUM_LED_NAMES] = {
    "LeftEar","RightEar",
    "LeftEye","RightEye",
    "Chest",
    "LeftFoot", "RightFoot"};
#endif
