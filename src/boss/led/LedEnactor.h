#pragma once

#include "NaoLights.h"
#include "alcommon/albroker.h"
#include "LedCommand.pb.h"

namespace boss {
namespace led {

class LedEnactor {

public:
    LedEnactor(boost::shared_ptr<AL::ALBroker> broker);
    virtual ~LedEnactor() {}

    void setLeds(messages::LedCommand command);
    void noMan();

protected:
    NaoLights naoLights;
};

}
}
