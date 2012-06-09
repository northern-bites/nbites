#include <gtest/gtest.h>
#include <list>
#include <string.h>
#include "ActiveArray.h"

#define private public
#define protected public
#include "../HoughSpace.h"
#include "../HoughSpaceImpl.h"
#undef private
#undef protected

#include "Gradient.h"
#include "VisionDef.h"

#include "Profiler.h"

using namespace std;
using boost::shared_ptr;

class HoughSpaceTest : public ::testing::Test
{
public:
    HoughSpaceImpl hs;

    // arbitrary acceptance thresholds
    const static float ACCEPT_ANGLE;
    const static int ACCEPT_RADIUS = 0;
};

// 1 bin in the hough space error allowed (plus a floating point error)
const float HoughSpaceTest::ACCEPT_ANGLE = 1 * M_PI_FLOAT/128.0f + 0.0001;
enum { max_parallel_tdiff = 5 };

bool isParallel(HoughLine& l, HoughLine& l2)
{
    int tDiff = abs(l.getTIndex() - l2.getTIndex()) - HoughConstants::t_span/2;
    return (abs(tDiff) < max_parallel_tdiff);
}

bool isDesiredLine(float goalR, float goalT, const HoughLine& line)
{
    float lineR = line.getRadius();
    float lineT = line.getAngle();

    EXPECT_LE(lineT, 2*M_PI_FLOAT);
    EXPECT_GE(lineT, 0);

    float goalLowerT = goalT - HoughSpaceTest::ACCEPT_ANGLE;
    float goalUpperT = goalT + HoughSpaceTest::ACCEPT_ANGLE;

    float tDiff = lineT - goalT;
    float rDiff = fabs(lineR - goalR);

    return (
        // Correct radius
        (rDiff <= HoughSpaceTest::ACCEPT_RADIUS) &&

        // Correct angle
        // Greater than lower bound
        NBMath::subPIAngle(tDiff) <= HoughSpaceTest::ACCEPT_ANGLE);
}

TEST_F(HoughSpaceTest, HoughSpace)
{
    Gradient g;

    // Create gradient map such that it has a known line
    g.createLineAtPoint(0, 80);

    // Run the gradient through the Hough Space
    hs.markEdges(g);

    for (int t=0; t < HoughConstants::t_span; ++t){
        for (int r=0; r < HoughConstants::r_span; ++r){
            EXPECT_GE(hs.getHoughBin(r,t) , 0);
        }
    }

    // Check to make sure there is a point at r = 80 = img_width/4 , theta = 0
    // which is where the above gradient has a line, roughly.
    EXPECT_GT(hs.getHoughBin(IMAGE_WIDTH * 1/4 + HoughConstants::r_span/2,0) , 0);

    // Notice that it is t_span +1. This is the same as in the
    // Hough Space.
    uint16_t pre[HoughConstants::t_span+10][HoughConstants::r_span];

    for (int t=0; t < HoughConstants::t_span+10; ++t){
        for(int r=0; r < HoughConstants::r_span; ++r){
            pre[t][r] = hs.hs[t][r];
        }
    }

    hs.smooth();

    // Test if smoothing worked
    for (int t=HoughConstants::first_smoothing_row;
         t < HoughConstants::t_span + HoughConstants::first_smoothing_row; ++t){
        for (int r=0; r < HoughConstants::r_span-1; ++r){

            int preSum = (pre[t][r]   + pre[t+1][r] +
                          pre[t][r+1] + pre[t+1][r+1]) -
                hs.getAcceptThreshold()*4; // Smoothing grows mag by 4x

            preSum = max(preSum, 0); // Bound it at zero

            int smoothed = hs.getHoughBin(r,t);
            EXPECT_EQ( smoothed, preSum);
        }
    }
}

void test_for_line(uint8_t angle, float radius)
{
    float radAngle = static_cast<float>(angle) * M_PI_FLOAT / 128.f;

    Gradient g;

    g.reset();
    g.clear();
    g.createLineAtPoint(angle, radius);

    HoughSpaceImpl hs;
    hs.reset();
    hs.findHoughLines(g);

    list<HoughLine> lines;
    for (int i=0; i < hs.activeLines.size(); ++i){
        if (hs.activeLines.active(i)){
            lines.push_back(hs.activeLines[i]);
        }
    }

    EXPECT_FALSE(lines.empty());

    float maxRadius = sqrtf(IMAGE_WIDTH * IMAGE_WIDTH +
                            IMAGE_HEIGHT * IMAGE_HEIGHT);

    bool foundFixedLine = false;
    list<HoughLine>::iterator l = lines.begin();

    while (l != lines.end()){
        EXPECT_LE(l->getRadius() , maxRadius); // Line must be in image
        EXPECT_GE(l->getRadius(), -maxRadius); // in either direction
        EXPECT_GE(l->getAngle() , 0);          // 0 <= Angle <= 2 * pi
        EXPECT_LE(l->getAngle() ,
                  2 * M_PI_FLOAT + HoughSpaceTest::ACCEPT_ANGLE);
        EXPECT_GE(l->getScore() , 0);

        // Make sure the system found the one line in the gradient
        if (isDesiredLine(radius, radAngle, *l)){
            foundFixedLine = true;
        }
        l++;
    }
    // We better have found that line
    EXPECT_TRUE(foundFixedLine);
}

/**
 * Test for known lines in an image
 */
TEST_F(HoughSpaceTest, lines)
{
    for(int t=0; t < 255; t += 5){
        for (float r=5; r < 120; r += 10){
            test_for_line(static_cast<uint8_t>(t), r);
        }
    }
}

TEST_F(HoughSpaceTest, Suppress)
{
    ActiveArray<HoughLine> lines(200);

    // test three identical lines to make sure that only one of the
    // duplicate lines survives suppress()
    HoughLine a1(100, 180, 50);
    HoughLine a3(99, 182, 4);
    HoughLine a2(101, 182, 2);

    lines.add(a1);
    lines.add(a2);
    lines.add(a3);

    int x0 = IMAGE_HEIGHT /2;
    int y0 = IMAGE_WIDTH /2;
    hs.suppress(x0, y0, lines);

    // Ensure that only one duplicate line remains
    EXPECT_EQ(lines.numActive(), 1);
    EXPECT_TRUE(lines[0] == a1);

    lines.clear();

    srand(time(NULL));
    int lastT = 0, t;
    for (int i = 0; i < 100; ++i) {
        int r = rand()%319;

        // The lines must be in sorted order by T!
        do{
            t = rand()%255;
        } while (lastT > t);

        int z = rand();
        HoughLine l(r, t, z);
        lastT = t;
        lines.add(l);
    }

    hs.suppress(x0, y0, lines);
    for (int i=0; i < lines.size(); ++i){
        if (!lines.active(i)){
            continue;
        }

        for (int j=i+1; j < lines.size(); ++j){
            if (!lines.active(j)){
                continue;
            }

            int rDiff = abs(lines[i].getRIndex() - lines[j].getRIndex());

            int tDiff = abs(abs(lines[i].getTIndex() -
                                lines[j].getTIndex()) & 255);
            EXPECT_FALSE(rDiff <= HoughConstants::suppress_r_bound &&
                         tDiff <= hs.angleSpread);
        }
    }

    // Make sure that suppress doesn't delete lines needlessly
    //
    // ***** THESE MUST BE ADDED IN INCREASING T ORDER ******
    lines.clear();
    HoughLine a(100,10,4);
    HoughLine b(100,180, 50);
    HoughLine c(10,200,400);
    HoughLine c2(10,203,4);

    lines.add(a);
    lines.add(b);
    lines.add(c);
    lines.add(c2);

    hs.suppress(x0, y0, lines);
    EXPECT_EQ( lines.numActive() , 3 );
    bool at = false, bt = false, ct = false;

    for (int i=0; i < lines.size(); ++i){
        if (lines[i] == a)
            at = true;
        if (lines[i] == b)
            bt = true;
        if (lines[i] == c)
            ct = true;
    }

    EXPECT_TRUE(at && bt && ct);
}

TEST_F(HoughSpaceTest, Pairing)
{
    // Insert random lines into list
    hs.activeLines.clear();
    srand(time(NULL));
    for (int i = 0; i < 40; ++i) {
        int r = rand()%319;
        int t = rand()%255;
        int z = rand();
        HoughLine l(r, t, z);

        bool addIt = true;
        for (int j=0; j < hs.activeLines.size(); ++j){
            if (hs.activeLines[j] == l){
                addIt = false;
            }
        }

        if (addIt){
            hs.activeLines.add(l);
        }
    }

    // Run them through narrowing
    list<pair<HoughLine, HoughLine> > lines = hs.narrowHoughLines();

    // Ensure that the only lines which return are parallel
    list<pair<HoughLine, HoughLine> >::iterator l, l2;

    for(l = lines.begin(); l != lines.end(); ++l){
        EXPECT_NE(l->first, l->second);
        EXPECT_TRUE(isParallel(l->first, l->second));
    }

    // Ensure that every line is only in a pair once
    for(l = lines.begin(); l != lines.end(); ++l){
        for(l2 = l; ++l2 != lines.end(); ){

            // Check all combos of lines between the pairs
            EXPECT_NE(l->first, l2->first);
            EXPECT_NE(l->first, l2->second);
            EXPECT_NE(l->second, l2->first);
            EXPECT_NE(l->second, l2->second);
        }
    }
}
