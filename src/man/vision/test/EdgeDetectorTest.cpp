#include <gtest/gtest.h>

#include <cmath>
#include <iostream>
#include <stdio.h>

#include "boost/shared_ptr.hpp"

#define private public
#define protected public
#include "../FieldLines/EdgeDetector.h"
#undef private
#undef protected

#define BYTE_TO_RAD 128./M_PI

#define DEFAULT_EDGE_VALUE 30

using namespace std;
using namespace man::vision;
using boost::shared_ptr;

class EdgeDetectorTest : public ::testing::Test
{
public:
    void create_circle_image(uint16_t * img, int r, double e, int i_0, int j_0)
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
    void printEdgePeakInfo(Gradient& g, int n) {
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
    void printEdgePeak(Gradient& g, int i, int j) {
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

    EdgeDetector edges;
};

TEST_F(EdgeDetectorTest, Gradient)
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
            EXPECT_EQ(g.values[i][j], 0);
        }
    }
}

// Test the direction function in the edge detector
TEST_F(EdgeDetectorTest, Direction)
{
    Gradient g;

    // Test 4 cardinal directions for dir
    const double BYTE_PI = M_PI * BYTE_TO_RAD;
    EXPECT_EQ(g.dir(0,1) , 0);
    EXPECT_FLOAT_EQ(BYTE_PI/2.,  g.dir(1,0));
    EXPECT_FLOAT_EQ(BYTE_PI,     g.dir(0,-1));
    EXPECT_FLOAT_EQ(3*BYTE_PI/2, g.dir(-1,0));

    // Ensure that dir() only returns values between 0 and 256 for all
    // image points
    for (int x= -IMAGE_WIDTH/2; x < IMAGE_WIDTH/2; ++x ){
        for (int y= -IMAGE_HEIGHT/2; y < IMAGE_HEIGHT/2; ++y){

            int a = g.dir(y,x);
            uint8_t trigDir = static_cast<uint8_t>(atan2(y,x) * 128/M_PI_FLOAT);

            // Make sure the direction is correct!
            EXPECT_LE(g.dir(y,x),
               trigDir + 1);
            EXPECT_GE(g.dir(y,x),
                trigDir - 1);

            uint8_t trigDir3 = static_cast<uint8_t>(trigDir >> 5);

            int diff3 = g.dir3(y,x) - trigDir3;
            EXPECT_LE(abs(diff3%7), 1); // Handle wrap around values

            EXPECT_LE(a , 256);
            EXPECT_GE(a, 0);
        }
    }
}

// Test the sobelOperator function in the edge detector
TEST_F(EdgeDetectorTest, Sobel)
{
    // Make sure that if all points are zero, then all points in the
    // gradient are zero
    uint16_t * c;
    c = new uint16_t[IMAGE_WIDTH * IMAGE_HEIGHT];

    for (int i=0; i < IMAGE_HEIGHT; ++i)
        for (int j=0; j < IMAGE_WIDTH; ++j)
            c[(i) * IMAGE_WIDTH + j] = 0;

    Gradient g;

    edges.sobelOperator(0, c, g);
    for (int i=1; i < IMAGE_HEIGHT-1; ++i){
        for (int j=1; j < IMAGE_WIDTH-1; ++j){
            EXPECT_EQ(g.getMagnitude(i,j) , 0);
        }
    }


    /**
     * Fill the channel with a bunch of values and make sure the sobel
     * operator spits out the correct gradients.
     */
    srand(time(NULL));
    for (int i=0; i < IMAGE_HEIGHT; ++i) {
        for (int j=0; j < IMAGE_WIDTH; ++j) {
            c[(i) * IMAGE_WIDTH + j] = static_cast<uint8_t>(rand()%255);
        }
    }

    edges.sobelOperator(0, c, g);
    for (int i=1; i < IMAGE_HEIGHT-1; ++i) {
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
            EXPECT_EQ(g.getX(i,j) , gx);
            EXPECT_EQ(g.getY(i,j) , gy);

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

            EXPECT_EQ(g.getMagnitude(i,j) , mag);

            // Useless with unsigned integers,
            // but kept around for austerity
            EXPECT_GE(g.getMagnitude(i,j) , 0);
        }
    }
    delete[] c;
}

/**
 * Ensure that the peaks of the edge detection are correct.
 */
TEST_F(EdgeDetectorTest, Peaks)
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

        EXPECT_NE(g.getMagnitude(y,x) , 0);
        EXPECT_TRUE(g.getX(y,x) != 0 ||
                    g.getY(y,x) != 0);
    }

    for (int n = 0; n < g.numPeaks; ++n) {

        EXPECT_TRUE(g.isPeak(n));

        // x,y are in image center relative coords
        int i = g.getAnglesYCoord(n) + IMAGE_HEIGHT/2;
        int j = g.getAnglesXCoord(n) + IMAGE_WIDTH/2;

        const int x = g.getX(i,j);
        const int y = g.getY(i,j);
        const int z = g.getMagnitude(i,j);

        int a = static_cast<int>(g.dir(y,x));
        // Get the highest 3 bits of the direction
        a = a >> 5;

        EXPECT_FALSE(g.peaks_list_contains(i + Gradient::dyTab[a],
                                    j + Gradient::dxTab[a]) ||
              g.peaks_list_contains(i - Gradient::dyTab[a],
                                    j - Gradient::dxTab[a]));
    }

    for (int i = 2; i < Gradient::rows-2; ++i) {
        for (int j = 2; j < Gradient::cols-2; ++j) {

            int x = j - IMAGE_WIDTH/2;
            int y = i - IMAGE_HEIGHT/2;

            if (x == IMAGE_WIDTH * 1/4 ||
                x == IMAGE_WIDTH * 1/4 - 1){
                EXPECT_TRUE((g.peaks_list_contains(y, IMAGE_WIDTH * 1/4) ||
                             g.peaks_list_contains(y, IMAGE_WIDTH * 1/4 - 1)));
            } else {
                EXPECT_FALSE(g.peaks_list_contains(y,x));
            }
        }
    }

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
                EXPECT_LE(abs(d), e*2);
            }
        }
        // cout << endl;
    }

    int index = 0;
    for(int i=0; i < IMAGE_HEIGHT; ++i){
        for (int j=0; j < IMAGE_WIDTH; ++j) {
            if ((index = g.peaks_list_contains(i - IMAGE_HEIGHT/2,
                                               j - IMAGE_WIDTH/2) )){
                EXPECT_EQ(g.peaks[i][j], g.getAngle(index-1));
            } else {
                EXPECT_LE(g.peaks[i][j], 0);
            }
        }
    }

    delete[] c;
}

TEST_F(EdgeDetectorTest, Angles)
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

            EXPECT_NE(g.getMagnitude(y,x) , 0);
            EXPECT_TRUE(g.getX(y,x) != 0 || g.getY(y,x) != 0);

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
        EXPECT_LE(abs(a-d), 2);
        ++i;
    }

    EXPECT_EQ(i, g.numPeaks);

    delete[] c;
}
