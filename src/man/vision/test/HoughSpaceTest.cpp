
#include "HoughSpaceTest.h"
#include <list>

#include "Profiler.h"


using namespace std;
using boost::shared_ptr;

 // approx. 3 degrees in radians
const float HoughSpaceTest::ACCEPT_ANGLE = 0.05f;

HoughSpaceTest::HoughSpaceTest() :
    hs()
{

}


void HoughSpaceTest::test_hs()
{
    // Create gradient map such that it has a known line
    shared_ptr<Gradient> g = shared_ptr<Gradient>(new Gradient());
    for (int i=0; i < Gradient::rows; ++i){
        for (int j=0; j < Gradient::cols; ++j){
            if (j < Gradient::cols *3./4.){
                g->setX(0,i,j);
                g->setY(0,i,j);
                g->setMagnitude(0,i,j);
                g->peaks[i][j] = false;
            } else {
                g->setX(0,i,j);
                g->setY(0,i,j);
                g->setMagnitude(0,i,j);
                g->peaks[i][j] = true;
            }
        }
    }

    // Run the gradient through the Hough Space
    hs.markEdges(g);

    for (int i=0; i < HoughSpace::R_SPAN; ++i){
        for (int j=0; j < HoughSpace::T_SPAN; ++j){
            GTE(hs.hs[i][j] , 0);
        }
    }
    PASSED(MORE_THAN_ZERO);

    // Check to make sure there is a point at r = 3/.4, theta = 0
    // which is where the above gradient has a line, roughly.
    GT(hs.hs[static_cast<int>(HoughSpace::R_SPAN * 3./4.)][0] , 0);
    PASSED(EDGE_AT_BOUND);

    // Notice that it is T_SPAN +1. This is the same as in the
    // Hough Space.
    int pre[HoughSpace::R_SPAN][HoughSpace::T_SPAN+1];
    for (int r=0; r < HoughSpace::R_SPAN; ++r){
        for (int t=0; t < HoughSpace::T_SPAN; ++t){
            pre[r][t] = hs.hs[r][t];
        }
    }

    for (int r=0; r < HoughSpace::R_SPAN; ++r)
        pre[r][HoughSpace::T_SPAN] = pre[r][0];

    hs.smooth();

    for (int r=0; r < HoughSpace::R_SPAN-1; ++r){
        for (int t=0; t < HoughSpace::T_SPAN; ++t){
            EQ_INT(hs.hs[r][t] , (pre[r][t]   + pre[r][t+1] +
                                  pre[r+1][t] + pre[r+1][t+1]));
        }
    }
    PASSED(SMOOTH_CORRECT);

}

/**
 * Test for known lines in an image
 */
void HoughSpaceTest::test_lines()
{
    shared_ptr<Gradient> g = shared_ptr<Gradient>(new Gradient());
    for (int i=0; i < IMAGE_HEIGHT; ++i){
        for (int j=0; j < IMAGE_WIDTH; ++j){
            if (j < IMAGE_WIDTH *3./4.){
                g->setX(0,i,j);
                g->setY(0,i,j);
                g->setMagnitude(0,i,j);
                g->peaks[i][j] = false;
            } else {
                g->setX(0,i,j);
                g->setY(0,i,j);
                g->setMagnitude(0,i,j);
                g->peaks[i][j] = true;
            }
        }
    }

    list<HoughLine> lines = hs.findLines(g);
    list<HoughLine>::iterator l = lines.begin();
    float maxRadius = sqrtf(IMAGE_WIDTH * IMAGE_WIDTH +
                           IMAGE_HEIGHT * IMAGE_HEIGHT);

    bool foundFixedLine = false;

    while (l != lines.end()){
        LTE(l->getRadius() , maxRadius); // Line must be in image
        GTE(l->getRadius() , 0);         // Line radius must be positive
        GTE(l->getAngle() , 0);          // 0 <= Angle <= 2 * pi
        LTE(l->getAngle() , 2 * M_PI);
        GTE(l->getScore() , 0);

        // Make sure the system found the one line in the gradient
        if (l->getAngle() < ACCEPT_ANGLE &&
            (l->getAngle() > 0 ||
             l->getAngle() > -ACCEPT_ANGLE + 2*M_PI) &&
            l->getRadius() > IMAGE_WIDTH / 4. - ACCEPT_RADIUS &&
            l->getRadius() > IMAGE_WIDTH / 4. + ACCEPT_RADIUS) {
            foundFixedLine = true;
        }

        l++;
    }
    TRUE(foundFixedLine);
}

void HoughSpaceTest::test_suppress()
{
    list<HoughLine> lines;

    // test three identical lines to make sure that only one of the
    // duplicate lines survives suppress()
    HoughLine a1 = HoughSpace::createLine(100, 180, 50);
    HoughLine a2 = HoughSpace::createLine(101, 182, 2);
    HoughLine a3 = HoughSpace::createLine(99, 182, 4);

    lines.push_back(a1);
    lines.push_back(a2);
    lines.push_back(a3);

    int x0 = IMAGE_HEIGHT /2;
    int y0 = IMAGE_WIDTH /2;
    hs.suppress(x0, y0, lines);

    EQ_INT(lines.size() , 1);   // Ensure that only one duplicate line remains
    TRUE(lines.front() == a1);

    PASSED(NO_DUPE_LINES);

    // Make sure that suppress doesn't delete lines needlessly
    HoughLine a = HoughSpace::createLine(100, 180, 50);
    HoughLine b = HoughSpace::createLine(10,200,400);
    HoughLine c = HoughSpace::createLine(100,10,4);
    HoughLine b2 = HoughSpace::createLine(10,203,4);

    lines.clear();
    lines.push_back(a);
    lines.push_back(b);
    lines.push_back(c);
    lines.push_back(b2);

    hs.suppress(x0, y0, lines);
    EQ_INT( lines.size() , 3 );
    bool at = false, bt = false, ct = false;

    list<HoughLine>::iterator i = lines.begin();
    while (i != lines.end()){
        if (*i == a)
            at = true;
        if (*i == b)
            bt = true;
        if (*i == c)
            ct = true;
        i++;
    }

    TRUE(at && bt && ct);
    PASSED(DONT_DELETE_GOOD_LINES);

}

int HoughSpaceTest::runTests()
{
    test_hs();
    test_lines();
    test_suppress();
    return 0;
}


int main(int argc, char * argv[])
{
    HoughSpaceTest * tests = new HoughSpaceTest();
    return tests->runTests();
}
