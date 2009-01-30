/**
 * TODO: Put in some docs =)
 */

#ifndef _WalkController_h_DEFINED
#define _WalkController_h_DEFINED

#include <list>
using std::list;

#include "Sensors.h"

class WalkController {
public:
    //WalkController(Sensors *s) : sensors(s) { }
    virtual const float tick(const list<float> *zmp_ref) = 0;
    virtual const float getPosition() const = 0;
    virtual const float getZMP() const = 0;
    virtual void initState(float x, float v, float p) = 0;

};

#endif
