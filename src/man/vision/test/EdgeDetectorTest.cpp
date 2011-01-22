#include "EdgeDetectorTest.h"
#include <iostream>
#include <cmath>

#include <stdio.h>
#include <assert.h>

#include "boost/shared_ptr.hpp"

#include "Profiler.h"


#define BYTE_TO_RAD 128./M_PI

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
    for (int i=0; i < IMAGE_HEIGHT; ++i)
        for (int j=0; j < IMAGE_WIDTH; ++j)
            EQ_INT(g->mag[i][j] , 0);
    PASSED(SOBEL_ZERO);


    /**
     * Fill the channel with a bunch of values and make sure the sobel
     * operator spits out the correct gradients.
     */

    for (int i=0; i < IMAGE_HEIGHT; ++i)
        for (int j=0; j < IMAGE_WIDTH; ++j)
            c[(i) * IMAGE_WIDTH + j] = rand() % 255;
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
            EQ_INT(g->x[i][j] , gx);
            EQ_INT(g->y[i][j] , gy);
            // EQ_INT(g->mag[i][j] , gx * gx + gy * gy);
            // GTE(g->mag[i][j] , 0); // Detect an overflow or incorrect magnitude
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
            if (j < IMAGE_HEIGHT *3./4.)
                c[(i) * IMAGE_WIDTH + j] = 0;
            else
                c[(i) * IMAGE_WIDTH + j] = 250;

    edges.detectEdges(c,g);

    // Everywhere peaks is true, the gradient is not zero. If it's not
    // a peak, we don't really care what the gradient values are.
    for (int i=0; i < Gradient::rows; ++i)
        for (int j=0; j < Gradient::cols; ++j){
            if (g->peaks[i][j]){
                NE_INT(g->mag[i][j] , 0);
                assert(g->x[i][j] != 0 || g->y[i][j] != 0);
            }
        }
    PASSED(PEAKS_ZERO);

    // Test to see that no peak follows in the same direction as another
    for (int i=0; i < Gradient::rows; ++i) {
        for (int j=0; j < Gradient::cols; ++j){

            const int z = g->mag[i][j];

            const int y = g->y[i][j];
            const int x = g->x[i][j];

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
    PASSED(PEAKS_DIR);
    return 0;
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
