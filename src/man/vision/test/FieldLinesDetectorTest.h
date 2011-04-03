#ifndef _FieldLinesDetectorTest_h_DEFINED
#define _FieldLinesDetectorTest_h_DEFINED

#define private public
#define protected public
#include "../FieldLinesDetector.h"
#undef private
#undef protected

#include "Tests.h"
#include "Gradient.h"

class FieldLinesDetectorTest
{
public:
    FieldLinesDetectorTest();
    virtual ~FieldLinesDetectorTest(){ }

    void runTests();

private:
    void test_lines();


private:
    FieldLinesDetector fld;

};

#endif /* _FieldLinesDetectorTest_h_DEFINED */
