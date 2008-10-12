
#ifndef _WalkProvider_h_DEFINED
#define _WalkProvider_h_DEFINED

#include <vector>
using namespace std;

#include "MotionProvider.h"

class WalkProvider : public MotionProvider {
public:
    WalkProvider();
    virtual ~WalkProvider();

    void requestStop();
    void calculateNextJoints();
    void addWalkCommand();

private:
    // Walk vector variables
    float x;
    float y;
    float theta;
};

#endif
