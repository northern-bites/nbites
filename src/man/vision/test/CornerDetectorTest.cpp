#include <gtest/gtest.h>
#include <vector>

#include "../FieldLines/CornerDetector.h"
#include "../FieldLines/HoughVisualLine.h"
#include "../geom/HoughLine.h"
#include "../FieldLines/HoughConstants.h"

using namespace std;
namespace HC = HoughConstants;

TEST(CornerDetector, Good)
{
    vector<HoughVisualLine> lines;
    lines.push_back(HoughVisualLine(HoughLine(HC::r_span/2 + 20, 0, 80),
                                    HoughLine(HC::r_span/2 + 20, 128, 80)));

    lines.push_back(HoughVisualLine(HoughLine(HC::r_span/2 + 20, 64, 80),
                                    HoughLine(HC::r_span/2 + 20, 194, 80)));

    CornerDetector cd;
    vector<int> edge(IMAGE_WIDTH, 0);

    cd.detect(0, &edge[0], lines);

    EXPECT_EQ(1, cd.getCorners().size());
}
