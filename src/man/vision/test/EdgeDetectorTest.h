#ifndef _EdgeDetectorTest_h_DEFINED
#define _EdgeDetectorTest_h_DEFINED

#define private public
#define protected public
#include "../EdgeDetector.h"
#undef private
#undef protected

#include "Tests.h"

class EdgeDetectorTest
{

    enum tests {
        DIR_UP,                 // 0
        DIR_DOWN,
        DIR_RIGHT,
        DIR_LEFT,
        DIR_ALL,
        SOBEL_ZERO,
        SOBEL_ALL,
        PEAKS_ZERO,
        PEAKS_DIR               // 8
    };
public:
    EdgeDetectorTest();
    virtual ~EdgeDetectorTest() { }

    int runTests();

private:
    int test_sobel();
    int test_dir();
    int test_peaks();

private:
    EdgeDetector edges;

};

#endif /* _EdgeDetectorTest_h_DEFINED */
