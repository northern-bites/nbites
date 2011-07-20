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

    static bool isParallel(HoughLine& l, HoughLine& l2);
    static bool isDesiredLine(float goalR, float goalT, const HoughLine& line);

    // Member functions
private:
    void test_hs();
    void test_lines();
    void test_for_line(uint8_t angle, float radius);
    void test_suppress();
    void test_pairing();

    // Member variables
private:
    HoughSpace hs;

    // arbitrary acceptance thresholds
    const static float ACCEPT_ANGLE;
    const static int ACCEPT_RADIUS = 0;
    enum { max_parallel_tdiff = 5 };
};

#endif /* _HoughSpaceTest_h_DEFINED */
