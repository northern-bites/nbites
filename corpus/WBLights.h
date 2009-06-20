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
    void setRGB(std::string led_id, int rdbHex){};

    void sendLights(){};
};

#endif
