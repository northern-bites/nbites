#include "FieldLinesDetectorTest.h"

#include <list>

using namespace std;
using boost::shared_ptr;

FieldLinesDetectorTest::FieldLinesDetectorTest() :
    fld(shared_ptr<Profiler>(new Profiler(&thread_micro_time)))
{

}

/**
 * Test the attributes of the VisualLines found
 */
void FieldLinesDetectorTest::test_lines()
{
    Gradient g;
    g.reset();

    // Create fake image
    srand(time(NULL));
    for (int i=0; i < 50; ++i){
        uint8_t angle = static_cast<uint8_t>(rand());
        float radius = static_cast<float>(rand()%200);
        g.createLineAtPoint(angle, radius);
    }

    // Run it through field lines
    fld.gradient = g;
    fld.houghLines = fld.hough.findLines(g);

    fld.findFieldLines();

    // Make sure there exists one line for every pair of lines
    EQ_INT(fld.houghLines.size(), fld.lines.size());
}

void FieldLinesDetectorTest::runTests()
{
    test_lines();
}

int main(int argc, char * argv[])
{
    FieldLinesDetectorTest fld;

    fld.runTests();
    return 0;
}
