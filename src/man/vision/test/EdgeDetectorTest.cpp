#include "EdgeDetectorTest.h"
#include "Profiler.h"
#include "visionconfig.h"

#include <assert.h>
#include <cmath>
#include <iostream>
#include <stdio.h>

#include "boost/shared_ptr.hpp"


#define BYTE_TO_RAD 128./M_PI

#define DEFAULT_EDGE_VALUE 30

using namespace std;
using boost::shared_ptr;

EdgeDetectorTest::EdgeDetectorTest() :
    edges(shared_ptr<Profiler>(new Profiler(&micro_time)), 100)
{

}

// Test the direction function in the edge detector
int EdgeDetectorTest::test_dir()
{
    shared_ptr<Gradient> g = shared_ptr<Gradient>(new Gradient());
    // Test 4 cardinal directions for dir
    const double BYTE_PI = M_PI * BYTE_TO_RAD;
    EQ_INT(g->dir(0,1) , 0);
    PASSED(DIR_RIGHT);

    EQ_FLOAT(g->dir(1,0) , BYTE_PI/2.);
    PASSED(DIR_UP);

    EQ_FLOAT(g->dir(0,-1) , BYTE_PI);
    PASSED(DIR_LEFT);

    EQ_FLOAT(g->dir(-1,0) , 1.5 * BYTE_PI);
    PASSED(DIR_DOWN);

    // Ensure that dir() only returns values between 0 and 256 for all
    // image points
    for (int x= -IMAGE_WIDTH/2; x < IMAGE_WIDTH/2; ++x )
        for (int y= -IMAGE_HEIGHT/2; y < IMAGE_HEIGHT/2; ++y){
            int a = g->dir(x,y);
            LTE(a , 256);
            GTE(a, 0);
        }
    PASSED(DIR_ALL);

    return 0;
}

// Test the sobelOperator function in the edge detector
int EdgeDetectorTest::test_sobel()
{
    // Make sure that if all points are zero, then all points in the
    // gradient are zero
    uint16_t * c;
    c =
        new uint16_t[IMAGE_WIDTH * IMAGE_HEIGHT];

    for (int i=0; i < IMAGE_HEIGHT; ++i)
        for (int j=0; j < IMAGE_WIDTH; ++j)
            c[(i) * IMAGE_WIDTH + j] = 0;

    shared_ptr<Gradient> g = shared_ptr<Gradient>(new Gradient());

    edges.sobelOperator(c, g);
    for (int i=1; i < IMAGE_HEIGHT-1; ++i){
        for (int j=1; j < IMAGE_WIDTH-1; ++j){
            EQ_INT(g->getMagnitude(i,j) , 0);
        }
    }
    PASSED(SOBEL_ZERO);


    /**
     * Fill the channel with a bunch of values and make sure the sobel
     * operator spits out the correct gradients.
     */
    srand(time(NULL));
    for (int i=0; i < IMAGE_HEIGHT; ++i)
        for (int j=0; j < IMAGE_WIDTH; ++j)
            c[(i) * IMAGE_WIDTH + j] = static_cast<uint8_t>(rand()%255);

    shared_ptr<Gradient> g2 = shared_ptr<Gradient>(new Gradient());

    edges.sobelOperator(c, g);
    for (int i=1; i < IMAGE_HEIGHT-1; ++i)
        for (int j=1; j < IMAGE_WIDTH-1; ++j){
            int gx = ((c[(i-1) * IMAGE_WIDTH + j+1] +
                       c[(i) * IMAGE_WIDTH + j+1] * 2 +
                       c[(i+1) * IMAGE_WIDTH + j+1]) -

                      (c[(i-1) * IMAGE_WIDTH + j-1] +
                       c[(i) * IMAGE_WIDTH + j-1] * 2 +
                       c[(i+1) * IMAGE_WIDTH + j-1]));

            int gy = ((c[(i+1) * IMAGE_WIDTH + j-1] +
                       c[(i+1) * IMAGE_WIDTH + j] * 2 +
                       c[(i+1) * IMAGE_WIDTH + j+1]) -

                      (c[(i-1) * IMAGE_WIDTH + j-1] +
                       c[(i-1) * IMAGE_WIDTH + j] * 2 +
                       c[(i-1) * IMAGE_WIDTH + j+1]));

            // The MMX subtracts in the opposite direction of the
            // above gradient calculation
            gx = -gx;
            gy = -gy;

            // The output gradients are shifted in by 1 value
            EQ_INT(g->getX(i,j) , gx);
            EQ_INT(g->getY(i,j) , gy);

            gx = gx << 3;
            gy = gy << 3;

            gx = gx * gx;
            gy = gy * gy;

            gx = gx >> 16;
            gy = gy >> 16;

            int mag = gx + gy;

            // All non above threshold points are zero
            mag = max(0,
                      mag-((edges.getThreshold() * edges.getThreshold()) >> 10));

            EQ_INT(g->getMagnitude(i,j) , mag);
            GTE(g->getMagnitude(i,j) , 0); // Useless with unsigned integers,
                                   // but kept around for austerity
        }
    PASSED(SOBEL_ALL);
    return 0;
}

/**
 * Ensure that the peaks of the edge detection are correct.
 */
int EdgeDetectorTest::test_peaks()
{
    uint16_t * c = new uint16_t[IMAGE_WIDTH * IMAGE_HEIGHT];

    shared_ptr<Gradient> g = shared_ptr<Gradient>(new Gradient());

    for (int i=0; i < IMAGE_HEIGHT; ++i)
        for (int j=0; j < IMAGE_WIDTH; ++j)
            if (j < IMAGE_WIDTH *3./4.)
                c[(i) * IMAGE_WIDTH + j] = 0;
            else
                c[(i) * IMAGE_WIDTH + j] = 250;
    g->reset();
    edges.detectEdges(c,g);

#ifdef USE_MMX
    for (int i = 0; i < Gradient::num_angles_limit; ++i) {
        if (g->getAnglesXCoord(i)){
            int x = g->getAnglesXCoord(i);
            int y = g->getAnglesYCoord(i);
            NE_INT(g->getMagnitude(y,x) , 0);
            assert(g->getX(y,x) != 0 || g->getY(y,x) != 0);

       // If we find an angle of zero, there are no more angles to be found
        } else {
            break;
        }
    }
#else  /* USE_MMX */

    // Everywhere peaks is true, the gradient is not zero. If it's not
    // a peak, we don't really care what the gradient values are.
    for (int i=2; i < Gradient::rows-2; ++i)
        for (int j=2; j < Gradient::cols-2; ++j){
            if (g->peaks[i][j]){
                NE_INT(g->getMagnitude(i,j) , 0);
                assert(g->getX(i,j) != 0 || g->getY(i,j) != 0);
            }
        }
#endif  /* USE_MMX */
    PASSED(PEAKS_ZERO);

#ifdef USE_MMX
    for (int n = 0; n < Gradient::num_angles_limit; ++n) {
        // No point has an x coordinate of zero because there cannot
        // be a peak there, so we use this to know when we have
        // reached the end of the edge points.
        if (!g->getAnglesXCoord(n)){
            break;
        } else {
            int i = g->getAnglesYCoord(n);
            int j = g->getAnglesXCoord(n);
            const int z = g->getMagnitude(i,j);

            const int y = g->getY(i,j);
            const int x = g->getX(i,j);

            int a = static_cast<int>(g->dir(y,x));

            // Get the highest 3 bits of the direction
            a = a >> 5;

            if (g->peaks[i][j] && (
                    g->peaks[i + Gradient::dyTab[a]][j +
                                                     Gradient::dxTab[a]] ||
                    g->peaks[i - Gradient::dyTab[a]][j -
                                                     Gradient::dxTab[a]])){
                assert(false);
            }
        }
    }

#else
    for (int i=2; i < Gradient::rows-2; ++i) {
        for (int j=2; j < Gradient::cols-2; ++j){

            const int z = g->getMagnitude(i,j);

            const int y = g->getY(i,j);
            const int x = g->getX(i,j);

            int a = static_cast<int>(g->dir(y,x));

            // Get the highest 3 bits of the direction
            a = a >> 5;

            if (g->peaks[i][j] && (
                    g->peaks[i + Gradient::dyTab[a]][j +
                                                     Gradient::dxTab[a]] ||
                    g->peaks[i - Gradient::dyTab[a]][j -
                                                    Gradient::dxTab[a]])){
                assert(false);
            }
        }
    }
#endif

    PASSED(PEAKS_DIR);

#ifdef USE_MMX
    for (int i = 2; i < Gradient::rows-2; ++i) {
        for (int j = 2; j < Gradient::cols-2; ++j) {
            if (j == IMAGE_WIDTH * 3/4 || j == IMAGE_WIDTH * 3/4 - 1){
                assert(peaks_list_contains(g,i,j));
            } else {
                assert(!peaks_list_contains(g,i,j));
            }
        }
    }
#else
    for (int i = 2; i < Gradient::rows-2; ++i) {
        for (int j = 2; j < Gradient::cols-2; ++j) {
            if (j == IMAGE_WIDTH * 3/4){
                assert(g->peaks[i][j]);
            } else {
                assert(!g->peaks[i][j]);
            }
        }
    }
#endif
    PASSED(CORRECT_PEAK);

    int r = 30;
    int e = 4;
    int i_0 = 120, j_0 = 160;

/*************** CIRCLE TESTS ************************************/
/*    create_circle_image(c, r, e, i_0, j_0);
    g->reset();
    edges.detectEdges(c,g);

#ifdef USE_MMX
    for (int i = 2; i < Gradient::rows-2; ++i) {
        for (int j = 2; j < Gradient::cols-2; ++j) {
            double a =
                min( max( (sqrt(pow(i-i_0,2) + pow(j-j_0,2))-r)/e, 0.), 1.);
            if (a > 0 && a < 1){
                if (!peaks_list_contains(g,i,j))
                    cout << i << " " << j << " " << g->getMagnitude(i,j) << endl;
                // assert(peaks_list_contains(g,i,j));
            }
        }
    }
#else
#endif
*/

    return 0;
}

void EdgeDetectorTest::create_circle_image(uint16_t * img, int r,
                                           double e, int i_0, int j_0)
{
    int high = 255;
    for (int i = 0; i < Gradient::rows; ++i) {
        for (int j = 0; j < Gradient::cols; ++j) {
            img[i * Gradient::cols + j] =
                static_cast<uint16_t>(min( max( (sqrt(pow(i-i_0,2) + pow(j-j_0,2))-r)/e, 0.), 1.)*high);
        }
    }
}

bool EdgeDetectorTest::peaks_list_contains(shared_ptr<Gradient> g,
                                           int i, int j){
    int n = 0;
    while (g->getAnglesXCoord(n) != 0){
        if (g->getAnglesXCoord(n) == j &&
            g->getAnglesYCoord(n) == i){
            return true;
        }
        n++;
    }
    return false;
}

int EdgeDetectorTest::runTests()
{
    test_dir();
    test_sobel();
    test_peaks();
    return 0;
}

int main(int argc, char * argv[])
{
    EdgeDetectorTest * tests = new EdgeDetectorTest();
    return tests->runTests();
}
