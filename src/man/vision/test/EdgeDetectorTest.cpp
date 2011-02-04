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
    for (int i=0; i < IMAGE_HEIGHT; ++i)
        for (int j=0; j < IMAGE_WIDTH; ++j)
            EQ_INT(g->getMagnitude(i,j) , 0);
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

#ifdef USE_MMX
    _sobel_operator(DEFAULT_EDGE_VALUE, &c[0], g->values);
#else
    edges.sobelOperator(c, g);
#endif
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
            int output_j = j+1;

            EQ_INT(g->getX(i,output_j) , gx);
            EQ_INT(g->getY(i,output_j) , gy);

            gx = gx << 3;
            gy = gy << 3;

            gx = gx * gx;
            gy = gy * gy;

            gx = gx >> 16;
            gy = gy >> 16;

            int mag = gx + gy;

            // All non above threshold points are zero
            mag = max(0, mag-((DEFAULT_EDGE_VALUE*DEFAULT_EDGE_VALUE) >> 10));

            EQ_INT(g->getMagnitude(i,output_j) , mag);
            GTE(g->getMagnitude(i,output_j) , 0); // Useless with unsigned integers,
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
            if (j < IMAGE_HEIGHT *3./4.)
                c[(i) * IMAGE_WIDTH + j] = 0;
            else
                c[(i) * IMAGE_WIDTH + j] = 250;

    edges.detectEdges(c,g);

    // Everywhere peaks is true, the gradient is not zero. If it's not
    // a peak, we don't really care what the gradient values are.
    for (int i=2; i < Gradient::rows-2; ++i)
        for (int j=2; j < Gradient::cols-2; ++j){
            if (g->peaks[i][j]){
                NE_INT(g->getMagnitude(i,j) , 0);
                assert(g->getX(i,j) != 0 || g->getY(i,j) != 0);
            }
        }
    PASSED(PEAKS_ZERO);

    // Test to see that no peak follows in the same direction as another
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
