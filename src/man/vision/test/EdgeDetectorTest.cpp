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
    edges()
{

}

void EdgeDetectorTest::test_gradient()
{
    Gradient g = Gradient();
    srand(time(NULL));
    for (int i=0; i < IMAGE_HEIGHT * 3; ++i){
        for (int j=0; j < IMAGE_WIDTH; ++j){
            g.values[i][j] = static_cast<uint16_t>(rand());
        }
    }

    g.clear();

    for (int i=0; i < IMAGE_HEIGHT * 3; ++i){
        for (int j=0; j < IMAGE_WIDTH; ++j){
            EQ_INT(g.values[i][j], 0);
        }
    }
    PASSED(GRADIENT_RESET);
}

// Test the direction function in the edge detector
int EdgeDetectorTest::test_dir()
{
    Gradient g;

    // Test 4 cardinal directions for dir
    const double BYTE_PI = M_PI * BYTE_TO_RAD;
    EQ_INT(g.dir(0,1) , 0);
    PASSED(DIR_RIGHT);

    EQ_FLOAT(g.dir(1,0) , BYTE_PI/2.);
    PASSED(DIR_UP);

    EQ_FLOAT(g.dir(0,-1) , BYTE_PI);
    PASSED(DIR_LEFT);

    EQ_FLOAT(g.dir(-1,0) , 1.5 * BYTE_PI);
    PASSED(DIR_DOWN);

    // Ensure that dir() only returns values between 0 and 256 for all
    // image points
    for (int x= -IMAGE_WIDTH/2; x < IMAGE_WIDTH/2; ++x ){
        for (int y= -IMAGE_HEIGHT/2; y < IMAGE_HEIGHT/2; ++y){

            int a = g.dir(y,x);
            uint8_t trigDir = static_cast<uint8_t>(atan2(y,x) * 128/M_PI_FLOAT);

            // Make sure the direction is correct!
            LTE(g.dir(y,x),
               trigDir + 1);
            GTE(g.dir(y,x),
                trigDir - 1);

            uint8_t trigDir3 = static_cast<uint8_t>(trigDir >> 5);

            int diff3 = g.dir3(y,x) - trigDir3;
            LTE(abs(diff3%7), 1); // Handle wrap around values

            LTE(a , 256);
            GTE(a, 0);
        }
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

    Gradient g;

    edges.sobelOperator(0, c, g);
    for (int i=1; i < IMAGE_HEIGHT-1; ++i){
        for (int j=1; j < IMAGE_WIDTH-1; ++j){
            EQ_INT(g.getMagnitude(i,j) , 0);
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

    edges.sobelOperator(0, c, g);
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
            EQ_INT(g.getX(i,j) , gx);
            EQ_INT(g.getY(i,j) , gy);

            gx = gx << 3;
            gy = gy << 3;

            gx = gx * gx;
            gy = gy * gy;

            gx = gx >> 16;
            gy = gy >> 16;

            int mag = gx + gy;

            // All non above threshold points are zero
            mag = max(0,
                      mag -
                      ((edges.getThreshold() * edges.getThreshold()) >> 10));

            EQ_INT(g.getMagnitude(i,j) , mag);
            GTE(g.getMagnitude(i,j) , 0); // Useless with unsigned integers,
                                   // but kept around for austerity
        }
    PASSED(SOBEL_ALL);
    delete[] c;
    return 0;
}

/**
 * Ensure that the peaks of the edge detection are correct.
 */
int EdgeDetectorTest::test_peaks()
{
    uint16_t * c = new uint16_t[IMAGE_WIDTH * IMAGE_HEIGHT];

    Gradient g;

    for (int i=0; i < IMAGE_HEIGHT; ++i)
        for (int j=0; j < IMAGE_WIDTH; ++j)
            if (j < IMAGE_WIDTH *3./4.)
                c[(i) * IMAGE_WIDTH + j] = 0;
            else
                c[(i) * IMAGE_WIDTH + j] = 250;
    int topEdge[IMAGE_WIDTH + 1];
    for (int i=0; i < IMAGE_WIDTH+1; ++i){
        topEdge[i] = 0;
    }

    edges.detectEdges(0, &topEdge[0], c,g);

    for (int i = 0; g.isPeak(i); ++i) {
        // x,y are in image center relative coords
        int x = g.getAnglesXCoord(i) + IMAGE_WIDTH/2;
        int y = g.getAnglesYCoord(i) + IMAGE_HEIGHT/2;

        NE_INT(g.getMagnitude(y,x) , 0);
        assert(g.getX(y,x) != 0 || g.getY(y,x) != 0);
    }
    PASSED(PEAKS_ZERO);

    for (int n = 0; n < g.numPeaks; ++n) {

        TRUE(g.isPeak(n));

        // x,y are in image center relative coords
        int i = g.getAnglesYCoord(n) + IMAGE_HEIGHT/2;
        int j = g.getAnglesXCoord(n) + IMAGE_WIDTH/2;

        const int x = g.getX(i,j);
        const int y = g.getY(i,j);
        const int z = g.getMagnitude(i,j);

        int a = static_cast<int>(g.dir(y,x));
        // Get the highest 3 bits of the direction
        a = a >> 5;

        FALSE(g.peaks_list_contains(i + Gradient::dyTab[a],
                                    j + Gradient::dxTab[a]) ||
              g.peaks_list_contains(i - Gradient::dyTab[a],
                                    j - Gradient::dxTab[a]));
    }
    PASSED(PEAKS_DIR);

    for (int i = 2; i < Gradient::rows-2; ++i) {
        for (int j = 2; j < Gradient::cols-2; ++j) {

            int x = j - IMAGE_WIDTH/2;
            int y = i - IMAGE_HEIGHT/2;

            if (x == IMAGE_WIDTH * 1/4 ||
                x == IMAGE_WIDTH * 1/4 - 1){
                TRUE((g.peaks_list_contains(y, IMAGE_WIDTH * 1/4) ||
                      g.peaks_list_contains(y, IMAGE_WIDTH * 1/4 - 1)));
            } else {
                FALSE(g.peaks_list_contains(y,x));
            }
        }
    }
    PASSED(CORRECT_PEAK);

    int r = 30;
    float e = 1.5;
    int i_0 = 120, j_0 = 160;

/*************** CIRCLE TESTS ************************************/
    g.reset();

    create_circle_image(c, r, e, i_0, j_0);
    edges.detectEdges(0, &topEdge[0], c,g);
    int n = 0;

    for (int i = 2; i < Gradient::rows-2; ++i) {
        for (int j = 2; j < Gradient::cols-2; ++j) {

            // Uncomment here (and cout << endl; below) to get ASCII edge image
            // printEdgePeak(g,i-IMAGE_HEIGHT/2,j-IMAGE_WIDTH/2);

            if( (n = g.peaks_list_contains(i,j)) ){
                // cout << i << " " << j << endl;
                // The index is 1 too great
                n--;
                int x = g.getAnglesXCoord(n);
                int y = g.getAnglesYCoord(n);
                double d = r - (sqrt(x*x + y*y));
                LTE(abs(d), e*2);
            }
        }
        // cout << endl;
    }

    int index = 0;
    for(int i=0; i < IMAGE_HEIGHT; ++i){
        for (int j=0; j < IMAGE_WIDTH; ++j) {
            if ((index = g.peaks_list_contains(i - IMAGE_HEIGHT/2,
                                               j - IMAGE_WIDTH/2) )){
                EQ_INT(g.peaks[i][j], g.getAngle(index-1));
            } else {
                LTE(g.peaks[i][j], 0);
            }
        }
    }
    PASSED(CORRECT_PEAKS_GRID);

    delete[] c;
    return 0;
}

int EdgeDetectorTest::test_angles()
{
    int r = 30;
    float e = 1.5;
    int i_0 = 120, j_0 = 160;

    uint16_t *c = new uint16_t[IMAGE_WIDTH * IMAGE_HEIGHT];

    Gradient g;

    int topEdge[IMAGE_WIDTH+1];
    for (int i=0; i < IMAGE_WIDTH+1; ++i){
        topEdge[i] = 0;
    }

    create_circle_image(c, r, e, i_0, j_0);
    edges.detectEdges(0, &topEdge[0], c,g);

    for (int i = 0; i < Gradient::num_angles_limit; ++i) {
        if (g.isPeak(i)){

            int x = g.getAnglesXCoord(i) + IMAGE_WIDTH/2;
            int y = g.getAnglesYCoord(i) + IMAGE_HEIGHT/2;

            NE_INT(g.getMagnitude(y,x) , 0);
            assert(g.getX(y,x) != 0 || g.getY(y,x) != 0);

       // If we find an x coordinate of zero, there are no more angles
       // to be found
        } else {
            break;
        }
    }

    int i = 0;
    while (g.isPeak(i)){
        int y = g.getAnglesYCoord(i) + IMAGE_HEIGHT/2;
        int x = g.getAnglesXCoord(i) + IMAGE_WIDTH/2;

        int yMag = g.getY(y,x);
        int xMag = g.getX(y,x);

        int d =  Gradient::dir(yMag,xMag);

        int a = g.getAngle(i);

        // Check that angles computed match angle from gradients
        LTE(abs(a-d), 2);
        ++i;
    }
    PASSED(PEAK_ANGLES);

    EQ_INT(i, g.numPeaks);
    PASSED(NUM_PEAKS);

    delete[] c;
    return 0;
}

void EdgeDetectorTest::create_circle_image(uint16_t * img, int r,
                                           double e, int i_0, int j_0)
{
    int high = 100;
    for (int i = 0; i < Gradient::rows; ++i) {
        for (int j = 0; j < Gradient::cols; ++j) {
            img[i * Gradient::cols + j] =
                static_cast<uint16_t>
                (min( max( (sqrt((i-i_0)*(i-i_0) +
                                 (j-j_0)*(j-j_0))-r)/e, 0.), 1.)
                 * high);
        }
    }
}

/**
 * Print all the information about an edge peak.
 */
void EdgeDetectorTest::printEdgePeakInfo(Gradient& g, int n)
{
    int angle = g.getAngle(n);
    int x = g.getAnglesXCoord(n);
    int y = g.getAnglesYCoord(n);

    int xMag = g.getX(y,x);
    int yMag = g.getY(y,x);

    cout << (int)x << " "
         << (int)y << " "
         << (int)g.getAngle(n) << " "
         << (int)g.dir(yMag,xMag) << " "
         << (int16_t)xMag << " "
         << (int16_t)yMag << " "
         << (int)g.getMagnitude(y,x)
         << endl;

}

/**
 * Prints an edge peak represented as a character in the direction of
 * its gradient
 */
void EdgeDetectorTest::printEdgePeak(Gradient& g, int i, int j)
{
    int index;
    if ((index = g.peaks_list_contains(i,j))){
        index--;
        int angle = g.getAngle(index);
        if (angle > 128){
            angle -= 128;
        }

        if (angle <= 16 || angle >= 112){
            cout << "_";
        } else if (angle >= 48 && angle <= 80){
            cout << "|";
        } else if (angle >= 80 && angle <= 112){
            cout << "/";
        } else if (angle >= 16 && angle <= 48){
            cout << "\\";           // Escape character\!
        }
    } else {
        cout << ".";
    }
    cout << " ";
}

int EdgeDetectorTest::runTests()
{
    test_gradient();
    test_dir();
    test_sobel();
    test_peaks();
    test_angles();

    return 0;
}

int main(int argc, char * argv[])
{
    EdgeDetectorTest * tests = new EdgeDetectorTest();
    return tests->runTests();
}
