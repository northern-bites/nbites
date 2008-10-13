
#ifndef _WalkProvider_h_DEFINED
#define _WalkProvider_h_DEFINED

#include <vector>
using namespace std;

#include "MotionProvider.h"
#include "WalkingConstants.h"

class WalkProvider : public MotionProvider {
public:
    WalkProvider();
    virtual ~WalkProvider();

    void requestStop();
    void calculateNextJoints();
    void setMotion(float x, float y, float theta);



private:

    WalkingParameters walkParameters;
};

#endif
