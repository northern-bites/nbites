#ifndef _EdgeDetectorTest_h_DEFINED
#define _EdgeDetectorTest_h_DEFINED

#define private public
#define protected public
#include "../EdgeDetector.h"
#undef private
#undef protected


class EdgeDetectorTest
{

    enum tests {
        DIR_UP,
        DIR_DOWN,
        DIR_RIGHT,
        DIR_LEFT,
        DIR_ALL,
        SOBEL_ZERO,
        SOBEL_ALL,
        PEAKS_ZERO,
        PEAKS_DIR
    };
public:
    EdgeDetectorTest();
    virtual ~EdgeDetectorTest() { }

    int runTests();

private:
    int test_sobel();
    int test_dir();
    int test_peaks();
    void passed(int a);

private:
    EdgeDetector edges;

};

#endif /* _EdgeDetectorTest_h_DEFINED */
