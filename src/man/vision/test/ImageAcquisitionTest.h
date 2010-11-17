#ifndef _ImageAcquisitionTest_h_DEFINED
#define _ImageAcquisitionTest_h_DEFINED

#include "Tests.h"

class ImageAcquisitionTest
{
public:
    ImageAcquisitionTest();
    virtual ~ImageAcquisitionTest(){ }

    int runTests();

private:
    void test_out_sane_values();
    void test_avg();
};

#endif /* _ImageAcquisitionTest_h_DEFINED */
