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
    const static float ACCEPT_ANGLE;
    const static int ACCEPT_RADIUS = 3;
};

#endif /* _HoughSpaceTest_h_DEFINED */
