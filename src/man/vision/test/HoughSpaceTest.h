#ifndef _HoughSpaceTest_h_DEFINED
#define _HoughSpaceTest_h_DEFINED

#define private public
#define protected public
#include "../HoughSpace.h"
#undef private
#undef protected

#include "Tests.h"
#include "Gradient.h"
#include "VisionDef.h"


class HoughSpaceTest
{
public:
    HoughSpaceTest();
    virtual ~HoughSpaceTest() { };

    enum tests {
        MORE_THAN_ZERO,
        EDGE_AT_BOUND,
        SMOOTH_CORRECT
    };

    int runTests();

    // Member functions
private:
    void test_hs();
    void test_lines();
    void test_suppress();

    // Member variables
private:
    HoughSpace hs;

    // arbitrary acceptance thresholds
    const static float ACCEPT_ANGLE = 0.05f; // approx. 3 degrees in radians
    const static int ACCEPT_RADIUS = 3;
};

#endif /* _HoughSpaceTest_h_DEFINED */
