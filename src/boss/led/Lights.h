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
    Lights();
    virtual ~Lights();

public:
    virtual void setRGB(const std::string led_id, const int newRgbHex){};
    virtual void setRGB(const unsigned int led_id, const int newRgbHex){};
    virtual void sendLights(){};

    static const unsigned int NUM_LED_NAMES = 29;
    static const std::string LED_NAMES[NUM_LED_NAMES];

};
#endif
