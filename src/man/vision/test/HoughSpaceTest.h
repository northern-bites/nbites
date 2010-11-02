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
        NO_ZERO,
        EDGE_AT_BOUND,
        SMOOTH_CORRECT
    };

    int runTests();

private:
    HoughSpace hs;

    void test_hs();
    void test_lines();
};

#endif /* _HoughSpaceTest_h_DEFINED */
