#include <gtest/gtest.h>
#include <list>

#include "FieldLines/Gradient.h"

#define private public
#define protected public
#include "../FieldLines/FieldLinesDetector.h"
#undef private
#undef protected

/**
 * Test the attributes of the VisualLines found
 */
TEST(FieldLinesDetector, Lines)
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

    FieldLinesDetector fld;

    // Run it through field lines
    fld.gradient = g;
    fld.houghLines = fld.hough->findLines(g);

    fld.findFieldLines();

    // Make sure there exists one line for every pair of lines
    EXPECT_EQ(fld.houghLines.size(), fld.lines.size());
}
