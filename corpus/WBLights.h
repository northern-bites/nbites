#ifndef WBLights_h_DEFINED
#define WBLights_h_DEFINED

#include "Lights.h"

/**
 *  Leds for Webots are not implemented yet. This class is simply a placeholder
 */
class WBLights : public Lights{
public:
    WBLights(){};
    ~WBLights(){};

public:
    void setRGB(const std::string led_id, const int rdbHex){};
    void setRGB(const unsigned int led_id, const int rdbHex){};

    void sendLights(){};
};

#endif
