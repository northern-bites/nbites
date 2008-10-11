
#ifndef _WalkProvider_h_DEFINED
#define _WalkProvider_h_DEFINED

#include <vector>
using namespace std;

class WalkProvider : MotionProvider {
public:
    void requestStop();
    void calculateNextJoints();
    void addWalkCommand();

private:

};

#endif
