#ifndef NaoLights_h_DEFINED
#define NaoLights_h_DEFINED

#include "Lights.h"
#include "dcmproxy.h"

class NaoLights : public Lights{
public:
    NaoLights(AL::ALPtr<AL::ALBroker> broker);
    virtual ~NaoLights();

public:
    void setRGB(std::string led_id, int rdbHex);

private:
    AL::ALPtr<AL::DCMProxy> dcmProxy;


};

#endif
