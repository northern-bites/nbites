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
public:
    EdgeDetectorTest();
    virtual ~EdgeDetectorTest() { }

    int runTests();

private:
    int test_sobel();
    int test_dir();
    int test_peaks();
    bool peaks_list_contains(boost::shared_ptr<Gradient> g, int i, int j);
    void create_circle_image(uint16_t * img, int r, double e, int i_0, int j_0);

private:
    EdgeDetector edges;

};

#endif /* _EdgeDetectorTest_h_DEFINED */
