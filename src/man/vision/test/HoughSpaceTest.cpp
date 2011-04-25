
#include "HoughSpaceTest.h"
#include <list>
#include <string.h>
#include "ActiveArray.h"

#include "Profiler.h"


using namespace std;
using boost::shared_ptr;

 // approx. 3 degrees in radians
const float HoughSpaceTest::ACCEPT_ANGLE = 0.1f;

HoughSpaceTest::HoughSpaceTest() :
    hs(shared_ptr<Profiler>(new Profiler(&micro_time)))
{

}

void HoughSpaceTest::test_hs()
{
    Gradient g;

    // Create gradient map such that it has a known line
    Gradient::createLineAtPoint(g, 0, 80);

    // Run the gradient through the Hough Space
    hs.markEdges(g);

    for (int t=0; t < HoughSpace::t_span; ++t){
        for (int r=0; r < HoughSpace::r_span; ++r){
            GTE(hs.getHoughBin(r,t) , 0);
        }
    }
    PASSED(MORE_THAN_ZERO);

    // Check to make sure there is a point at r = 80 = img_width/4 , theta = 0
    // which is where the above gradient has a line, roughly.
    GT(hs.getHoughBin(IMAGE_WIDTH * 1/4 + HoughSpace::r_span/2,0) , 0);
    PASSED(EDGE_AT_BOUND);

    // Notice that it is t_span +1. This is the same as in the
    // Hough Space.
    uint16_t pre[HoughSpace::t_span+10][HoughSpace::r_span];

    for (int t=0; t < HoughSpace::t_span+10; ++t){
        for(int r=0; r < HoughSpace::r_span; ++r){
            pre[t][r] = hs.hs[t][r];
        }
    }

    hs.smooth();

    // Test if smoothing worked
    for (int t=HoughSpace::first_smoothing_row;
         t < HoughSpace::t_span + HoughSpace::first_smoothing_row; ++t){
        for (int r=0; r < HoughSpace::r_span-1; ++r){

            int preSum = (pre[t][r]   + pre[t+1][r] +
                          pre[t][r+1] + pre[t+1][r+1]) -
                hs.getAcceptThreshold()*4; // Smoothing grows mag by 4x

            preSum = max(preSum, 0); // Bound it at zero

            int smoothed = hs.getHoughBin(r,t);
            EQ_INT( smoothed, preSum);
        }
    }
    PASSED(SMOOTH_CORRECT);
}

/**
 * Test for known lines in an image
 */
void HoughSpaceTest::test_lines()
{
    for(int t=0; t < 255; t += 5){
        for (float r=5; r < 120; r += 10){
            test_for_line(static_cast<uint8_t>(t), r);
        }
    }
    PASSED(FOUND_GOOD_LINES);
}

void HoughSpaceTest::test_for_line(uint8_t angle, float radius)
{
    float radAngle = static_cast<float>(angle) * M_PI_FLOAT / 128.f;

    Gradient g;

    g.reset();
    g.clear();
    Gradient::createLineAtPoint(g, angle, radius);

    hs.reset();
    hs.findHoughLines(g);

    list<HoughLine> lines;
    for (int i=0; i < hs.activeLines.size(); ++i){
        if (hs.activeLines.active(i)){
            lines.push_back(hs.activeLines[i]);
        }
    }

    float maxRadius = sqrtf(IMAGE_WIDTH * IMAGE_WIDTH +
                           IMAGE_HEIGHT * IMAGE_HEIGHT);

    bool foundFixedLine = false;
    list<HoughLine>::iterator l = lines.begin();
    while (l != lines.end()){
        LTE(l->getRadius() , maxRadius); // Line must be in image
        GTE(l->getRadius(), -maxRadius); // in either direction
        GTE(l->getAngle() , 0);          // 0 <= Angle <= 2 * pi
        LTE(l->getAngle() , 2 * M_PI_FLOAT + ACCEPT_ANGLE);
        GTE(l->getScore() , 0);

        // Make sure the system found the one line in the gradient
        if (isDesiredLine(radius, radAngle, *l)){
            foundFixedLine = true;
        }

        l++;
    }
    // We better have found that line
    TRUE(foundFixedLine);
}

void HoughSpaceTest::test_suppress()
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
    EQ_INT(lines.numActive(), 1);
    TRUE(lines[0] == a1);

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
            FALSE (rDiff <= HoughSpace::suppress_r_bound &&
                   tDiff <= hs.angleSpread);
        }
    }

    PASSED(NO_DUPE_LINES);

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
    EQ_INT( lines.numActive() , 3 );
    bool at = false, bt = false, ct = false;

    for (int i=0; i < lines.size(); ++i){
        if (lines[i] == a)
            at = true;
        if (lines[i] == b)
            bt = true;
        if (lines[i] == c)
            ct = true;
    }

    TRUE(at && bt && ct);
    PASSED(DONT_DELETE_GOOD_LINES);

}

void HoughSpaceTest::test_pairing()
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
        TRUE(l->first != l->second &&
              isParallel(l->first, l->second));
    }
    PASSED(CORRECT_PAIRING);

    // Ensure that every line is only in a pair once
    for(l = lines.begin(); l != lines.end(); ++l){
        for(l2 = l; ++l2 != lines.end(); ){

            // Check all combos of lines between the pairs
            NE(l->first, l2->first);
            NE(l->first, l2->second);
            NE(l->second, l2->first);
            NE(l->second, l2->second);
        }
    }
}

bool HoughSpaceTest::isParallel(HoughLine& l, HoughLine& l2)
{
    int tDiff = abs(l.getTIndex() - l2.getTIndex()) - HoughSpace::t_span/2;
    return (abs(tDiff) < max_parallel_tdiff);
}

bool HoughSpaceTest::isDesiredLine(float goalR, float goalT,
                                   const HoughLine& line)
{
    float lineR = line.getRadius();
    float lineT = line.getAngle();

    LTE(lineT, 2*M_PI_FLOAT);
    GTE(lineT, 0);

    float goalLowerT = goalT - ACCEPT_ANGLE;
    float goalUpperT = goalT + ACCEPT_ANGLE;

    float tDiff = fabs(lineT - goalT);
    float rDiff = fabs(lineR - goalR);

    return (
        // Correct radius
        (rDiff < ACCEPT_RADIUS) &&

        // Correct angle
        // Greater than lower bound
        (tDiff < ACCEPT_ANGLE ||
         fabs(2*M_PI_FLOAT - tDiff) < ACCEPT_ANGLE));

}

int HoughSpaceTest::runTests()
{
    test_hs();
    test_lines();
    test_suppress();
    test_pairing();
    return 0;
}


int main(int argc, char * argv[])
{
    HoughSpaceTest * tests = new HoughSpaceTest();
    return tests->runTests();
}
