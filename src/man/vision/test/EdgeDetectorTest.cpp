#include "EdgeDetectorTest.h"
#include <iostream>
#include <cmath>

#include <stdio.h>
#include <assert.h>

#define CLOSE_ENOUGH_EQ_FLOAT .0000001

#define EQ_FLOAT(x,y) (assert(x - CLOSE_ENOUGH_EQ_FLOAT < y \
                       && x + CLOSE_ENOUGH_EQ_FLOAT > y))
#define EQ_INT(x,y) assert(x == y)
#define NE_INT(x,y) assert(x != y)

#define LT(x,y) assert(x < y)
#define LTE(x,y) assert(x <= y)
#define GT(x,y) assert(x > y)
#define GTE(x,y) assert(x >= y)

#define BYTE_TO_RAD 128./M_PI

using namespace std;

EdgeDetectorTest::EdgeDetectorTest() : edges(100)
{

}

// Test the direction function in the edge detector
int EdgeDetectorTest::test_dir()
{
    Gradient g;
    // Test 4 cardinal directions for dir
    const double BYTE_PI = M_PI * BYTE_TO_RAD;
    EQ_INT(g.dir(0,1), 0);
    passed(DIR_RIGHT);

    EQ_FLOAT(g.dir(1,0) , BYTE_PI/2.);
    passed(DIR_UP);

    EQ_FLOAT(g.dir(0,-1) , BYTE_PI);
    passed(DIR_LEFT);

    EQ_FLOAT(g.dir(-1,0) , 1.5 * BYTE_PI);
    passed(DIR_DOWN);

    // Ensure that dir() only returns values between 0 and 256 for all
    // image points
    for (int x= -IMAGE_WIDTH/2; x < IMAGE_WIDTH/2; ++x )
        for (int y= -IMAGE_HEIGHT/2; y < IMAGE_HEIGHT/2; ++y){
            int a = g.dir(x,y);
            LTE(a , 256);
            GTE(a, 0);
        }
    passed(DIR_ALL);

    return 0;
}

// Test the sobelOperator function in the edge detector
int EdgeDetectorTest::test_sobel()
{
    // Make sure that if all points are zero, then all points in the
    // gradient are zero
    Channel c;
    for (int i=0; i < IMAGE_HEIGHT; ++i)
        for (int j=0; j < IMAGE_WIDTH; ++j)
            c.val[i][j] = 0;

    Gradient g;
    edges.sobelOperator(c, g);
    for (int i=0; i < IMAGE_HEIGHT; ++i)
        for (int j=0; j < IMAGE_WIDTH; ++j)
            EQ_INT(g.mag[i][j] , 0);
    passed(SOBEL_ZERO);


    /**
     * Fill the channel with a bunch of values and make sure the sobel
     * operator spits out the correct gradients.
     */

    for (int i=0; i < IMAGE_HEIGHT; ++i)
        for (int j=0; j < IMAGE_WIDTH; ++j)
            c.val[i][j] = i + j;
    Gradient g2;
    edges.sobelOperator(c, g);
    for (int i=1; i < IMAGE_HEIGHT-1; ++i)
        for (int j=1; j < IMAGE_WIDTH-1; ++j){
            int gx = ((c.val[i-1][j+1] +
                      c.val[i][j+1] * 2 +
                      c.val[i+1][j+1]) -

                      (c.val[i-1][j-1] +
                       c.val[i][j-1] * 2 +
                       c.val[i+1][j-1]));

            int gy = ((c.val[i+1][j-1] +
                      c.val[i+1][j] * 2 +
                      c.val[i+1][j+1]) -

                      (c.val[i-1][j-1] +
                       c.val[i-1][j] * 2 +
                       c.val[i-1][j+1]));

            EQ_INT(g.x[i][j] , gx);
            EQ_INT(g.y[i][j] , gy);
            EQ_INT(g.mag[i][j] , gx * gx + gy * gy);
            GTE(g.mag[i][j] , 0);

        }
    passed(SOBEL_ALL);
    return 0;
}

int EdgeDetectorTest::test_peaks()
{
    Channel c;
    Gradient g;
    for (int i=0; i < IMAGE_HEIGHT; ++i)
        for (int j=0; j < IMAGE_WIDTH; ++j)
            c.val[i][j] = i + j;
    Gradient g2;
    edges.sobelOperator(c, g);
    edges.findPeaks(g);

    // Ensure that everywhere peaks is false, the gradient is set to zero
    // and everywhere peaks is true, the gradient is not zero
    for (int i=0; i < IMAGE_HEIGHT; ++i)
        for (int j=0; j < IMAGE_WIDTH; ++j){
            if (g.peaks[i][j]){
                NE_INT(g.mag[i][j] , 0);
                NE_INT(g.x[i][j] , 0);
                NE_INT(g.y[i][j] , 0);
            } else {
                EQ_INT(g.mag[i][j] , 0);
                EQ_INT(g.x[i][j] , 0);
                EQ_INT(g.y[i][j] , 0);
            }
        }
    passed(PEAKS_ZERO);

    // Test to see that no peak follows in the same direction as another
    for (int i=0; i < g.rows; ++i) {
        for (int j=0; j < g.cols; ++j){

            const int z = g.mag[i][j];

            const int y = g.y[i][j];
            const int x = g.x[i][j];

            int a = static_cast<int>(g.dir(y,x));

            // Get the highest 3 bits of the direction
            a = a >> 5;

            if (g.peaks[i][j] && (
                g.peaks[i + EdgeDetector::dyTab[a]][j +
                                                     EdgeDetector::dxTab[a]] ||
                g.peaks[i + EdgeDetector::dyTab[a]][j +
                                                    EdgeDetector::dxTab[a]])){
                assert(false);
            }
        }
    }
    passed(PEAKS_DIR);
    return 0;
}

int EdgeDetectorTest::runTests()
{
    test_dir();
    test_sobel();
    test_peaks();
    return 0;
}

void EdgeDetectorTest::passed(int a)
{
    cout << "passed test " << a << endl;
}

int main(int argc, char * argv[])
{
    EdgeDetectorTest * tests = new EdgeDetectorTest();
    return tests->runTests();
}
