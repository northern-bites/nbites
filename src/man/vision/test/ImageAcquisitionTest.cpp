#include "ImageAcquisitionTest.h"

#include "ColorParams.h"
#include "VisionDef.h"

#include <stdlib.h>
#include <cstdio>

#include <iostream>
#include <fstream>

using namespace std;

typedef unsigned char uchar;

// Our function to test!
extern "C" void _acquire_image(uchar * colorTable, ColorParams* params,
                               uchar* yuvImage, uchar* outImage);

ImageAcquisitionTest::ImageAcquisitionTest() :
    c(0,0,0, 256, 256, 256, 128, 128, 128) // Default old table size
{
    init();
    setup(0,0,0,
          16,16,16,
          10,10,10 );
}

ImageAcquisitionTest::~ImageAcquisitionTest()
{
    deallocate();
}

void ImageAcquisitionTest::init()
{
    table = yuv = yuvCopy = out = NULL;
}

/**
 * Fill the input image and the ColorParams struct
 */
void ImageAcquisitionTest::setup(int y0, int u0, int v0,
                                 int y1, int u1, int v1,
                                 int yn, int un, int vn)
{
    allocate();
    c = ColorParams(y0, u0, v0,
                    y1, u1, v1,
                    yn, un, vn);

    // Seed the random number generator
    srand ( time(NULL) );

    // Fill the table and images with random values
    for (int i = 0; i < tableMaxSize; ++i) {
        table[i] = static_cast<uchar>(rand()% 255);
    }

    for (int i = 0; i < NAO_IMAGE_BYTE_SIZE; ++i) {
        yuv[i] = static_cast<uchar>(rand() % 255);
    }

    for (int i = 0; i < NAO_IMAGE_BYTE_SIZE; ++i) {
        yuvCopy[i] = yuv[i];
    }

}

/**
 * Allocate memory for the table and image arrays
 */
void ImageAcquisitionTest::allocate()
{
    if (table == NULL){
        table = new uchar[tableMaxSize];
    }

    if (yuv == NULL){
        yuv = new uchar[NAO_IMAGE_BYTE_SIZE];
    }

    // Copy the image for later checking
    if (yuvCopy == NULL){
        yuvCopy = new uchar[NAO_IMAGE_BYTE_SIZE];
    }

    // Allocate the output image.
    if (out == NULL){
        // There are two images, the Y and the color
        out = new uchar[IMAGE_BYTE_SIZE * 2];
    }
}

/**
 * Free the memory from the images and tables
 */
void ImageAcquisitionTest::deallocate()
{
    delete table;
    delete yuv;
    delete yuvCopy;
    delete out;
}

/**
 * @param i Row of pixel in output image
 * @param j Column of pixel in output image
 * @return Averaged value of 4 y pixels around given (i,j) location
 */
int ImageAcquisitionTest::yAvgValue(int i, int j) const
{
    // Get location of pixel in full size (double width) YUV image x4
    // is for 2 bytes per pixel and double YUV image size compared to
    // output image
    uchar* p = yuv + 4 * (i * NAO_IMAGE_WIDTH + j);
    return (static_cast<int>(p[YOFFSET1]) +
            static_cast<int>(p[YOFFSET2]) +
            static_cast<int>(p[NAO_IMAGE_ROW_OFFSET + YOFFSET1]) +
            static_cast<int>(p[NAO_IMAGE_ROW_OFFSET + YOFFSET2]));
}

/**
 * @param i Row of pixel in output image
 * @param j Column of pixel in output image
 * @return Averaged value of 2 u pixels (at and below) given (i,j) location
 */
int ImageAcquisitionTest::uAvgValue(int i, int j) const
{
    uchar * p = yuv + 4 * (i * NAO_IMAGE_WIDTH + j);
    return (static_cast<int>(p[UOFFSET]) +
            static_cast<int>(p[NAO_IMAGE_ROW_OFFSET + UOFFSET]));
}

/**
 * @param i Row of pixel in output image
 * @param j Column of pixel in output image
 * @return Averaged value of 2 v pixels (at and below) given (i,j) location
 */
int ImageAcquisitionTest::vAvgValue(int i, int j) const
{
    uchar * p = yuv + 4 * (i * NAO_IMAGE_WIDTH + j);
    return (static_cast<int>(p[VOFFSET]) +
            static_cast<int>(p[NAO_IMAGE_ROW_OFFSET + VOFFSET]));
}


/**
 * Find the color table y index of a pixel using the ColorParams space values.
 *
 * @param i Row of pixel in output image
 * @param j Column of pixel in output image
 * @return Y index of given (i,j) pixel in color table space
 */
int ImageAcquisitionTest::yIndex(int i, int j) const
{
    int y = max( yAvgValue(i,j) - yZero(), 0);
    y = min( (y * ySlope()) >> 16 , yLimit());
    return y;
}

/**
 * See yIndex for details. Does same but for u index.
 */
int ImageAcquisitionTest::uIndex(int i, int j) const
{
    int u = max( uAvgValue(i, j) - uZero(), 0);
    u = min( (u * uSlope()) >> 16 , uLimit());
    return u;
}

/**
 * See yIndex for details. Does same but for v index.
 */
int ImageAcquisitionTest::vIndex(int i, int j) const
{
    int v = max( vAvgValue(i, j) - vZero(), 0);
    v = min( (v * vSlope()) >> 16 , vLimit());
    return v;
}

/**
 * @return Color in table according to given YUV values.
 */
int ImageAcquisitionTest::tableLookup(int y, int u, int v) const
{
    // Table is in UVY ordering
    int index = y +
        v * static_cast<int>(c.uvDim & 0xFFFF) +
        u * static_cast<int>( (c.uvDim >> 16) & 0xFFFF);
    return table[index];
}

/**
 * @param i Row in output image of pixel
 * @param j Column in output image of given pixel
 * @return Color in output image at given location.
 */
int ImageAcquisitionTest::colorValue(int i, int j) const
{
    // Color image is just past Y Image in array
    return out[IMAGE_WIDTH * i + j + IMAGE_BYTE_SIZE];
}

void ImageAcquisitionTest::test_color_segmentation()
{
    for (int i=0; i < IMAGE_HEIGHT; ++i){
        for (int j=0; j < IMAGE_WIDTH; ++j){
            EQ_INT( colorValue(i,j) ,
                    tableLookup( yIndex(i,j),
                                 uIndex(i,j),
                                 vIndex(i,j) ));
        }
    }
    PASSED(COLOR_SEGMENTATION);
}

/**
 * Test the averaging of the pixels by the image acquistion.
 */
void ImageAcquisitionTest::test_avg()
{

    // Run it!
    _acquire_image(table, &c, yuv, out);

    // Make sure that the run didn't affect the initial image
    for (int i = 0; i < 640*480*2; ++i) {
        EQ_INT((int)yuvCopy[i] , (int)yuv[i]);
    }
    PASSED(PRESERVE_IMAGE);

    // Check that the average works properly.
    for (int i = 0; i < IMAGE_HEIGHT; ++i) {
        for (int j=0; j < IMAGE_WIDTH; ++j){

            int output = (int)out[i*IMAGE_WIDTH + j];
            EQ_INT( yAvgValue(i, j) >> 2, output);
        }
    }
    PASSED(AVERAGES);
}

int ImageAcquisitionTest::runTests()
{
    test_avg();
    test_color_segmentation();
    return 0;
}

int main(int argc, char * argv[])
{
    ImageAcquisitionTest * iat = new ImageAcquisitionTest();
    return iat->runTests();
}
