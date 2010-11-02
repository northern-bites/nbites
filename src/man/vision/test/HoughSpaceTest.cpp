
#include "HoughSpaceTest.h"

using namespace std;
using boost::shared_ptr;

HoughSpaceTest::HoughSpaceTest() : hs()
{

}


void HoughSpaceTest::test_hs()
{
    // Create gradient map such that it has a known line
    shared_ptr<Gradient> g = shared_ptr<Gradient>(new Gradient());
    for (int i=0; i < IMAGE_HEIGHT; ++i){
        for (int j=0; j < IMAGE_WIDTH; ++j){
            if (j < IMAGE_WIDTH *3./4.){
                g->x[i][j] = 0;
                g->y[i][j] = 0;
                g->mag[i][j] = 0;
                g->peaks[i][j] = false;
            } else {
                g->x[i][j] = 5;
                g->y[i][j] = 0;
                g->mag[i][j] = 25;
                g->peaks[i][j] = true;
            }
        }
    }

    hs.markEdges(g);


    for (int i=0; i < HoughSpace::R_SPAN; ++i){
        for (int j=0; j < HoughSpace::T_SPAN; ++j){
            GTE(hs.hs[i][j] , 0);
        }
    }
    passed(NO_ZERO);

    // Check to make sure there is a point at r = 3/.4, theta = 0
    // which is where the above gradient has a line, roughly.
    GT(hs.hs[static_cast<int>(HoughSpace::R_SPAN * 3./4.)][0] , 0);
    passed(EDGE_AT_BOUND);

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
    passed(SMOOTH_CORRECT);

}

int HoughSpaceTest::runTests()
{
    test_hs();
    return 0;
}


int main(int argc, char * argv[])
{
    HoughSpaceTest * tests = new HoughSpaceTest();
    return tests->runTests();
}
