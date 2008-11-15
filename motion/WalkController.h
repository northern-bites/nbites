/**
 * TODO: Put in some docs =)
 */

#ifndef _WalkController_h_DEFINED
#define _WalkController_h_DEFINED

#include "Sensors.h"

class WalkController {
public:
    //WalkController(Sensors *s) : sensors(s) { }
    virtual const float tick(const float reference) = 0;
    virtual const float getPosition() const = 0;
};

#endif
