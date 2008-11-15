/**
 * TODO: Put in some docs =)
 */

#ifndef _DummyController_h_DEFINED
#define _DummyController_h_DEFINED

#include "WalkController.h"

class DummyController : public WalkController {
public:
    DummyController();
    virtual const float tick(const float reference);
    virtual const float getPosition() const { return position; }

private:
    float position;
};

#endif
