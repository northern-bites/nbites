#include "ImageAcquisitionTest.h"

#include "ColorParams.h"
#include "VisionDef.h"
#include "ImageAcquisition.h"

#include <stdlib.h>
#include <cstdio>

#include <iostream>
#include <fstream>
#include <time.h>
#include <sys/time.h>

// #define PROFILE_ACQUIRE

using namespace std;

ImageAcquisitionTest::ImageAcquisitionTest() :
    c(0,0,0, 256, 256, 256, 128, 128, 128), // Default old table size
    sumTime_thread(0), sumTime_process(0), sumTime_mono(0),
    sum_clocks(0), min_clocks(0xFFFFFFF), numFrames(500)
{
    init();
    setup(45, 20, 33,
          255,255,255,
          10,10,10 );
}

ImageAcquisitionTest::~ImageAcquisitionTest()
{
    deallocate();
}

void ImageAcquisitionTest::init()
{
    yuv = table = yuvCopy = NULL;
    out = NULL;
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
        table[i] = static_cast<uint8_t>(rand()% 255);
    }

    for (int i = 0; i < NAO_IMAGE_BYTE_SIZE; ++i) {
        yuv[i] = static_cast<uint8_t>(rand()%255);
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
        table = new uint8_t[tableMaxSize];
    }

    if (yuv == NULL){
        yuv = new uint8_t[NAO_IMAGE_BYTE_SIZE];
    }

    // Copy the image for later checking
    if (yuvCopy == NULL){
        yuvCopy = new uint8_t[NAO_IMAGE_BYTE_SIZE];
    }

    // Allocate the output image.
    if (out == NULL){
        // 6 bytes per YUV triple, and 1 per color value
        out = reinterpret_cast<uint16_t*>(new uint8_t[320*240*7]);
    }
}

/**
 * Free the memory from the images and tables
 */
void ImageAcquisitionTest::deallocate()
{
    delete [] table;
    delete [] yuv;
    delete [] yuvCopy;
    delete [] out;
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
    uint8_t* p = yuv + 4 * (i * NAO_IMAGE_WIDTH + j);
    return (static_cast<int>(p[YOFFSET1]) +
            static_cast<int>(p[YOFFSET2]));
            // static_cast<int>(p[NAO_IMAGE_ROW_OFFSET + YOFFSET1]) +
            // static_cast<int>(p[NAO_IMAGE_ROW_OFFSET + YOFFSET2]));
}

/**
 * @param i Row of pixel in output image
 * @param j Column of pixel in output image
 * @return Averaged value of 2 u pixels (at and below) given (i,j) location
 */
int ImageAcquisitionTest::uAvgValue(int i, int j) const
{
    uint8_t * p = yuv + 4 * (i * NAO_IMAGE_WIDTH + j);
    return (static_cast<int>(p[UOFFSET]));
            // static_cast<int>(p[NAO_IMAGE_ROW_OFFSET + UOFFSET]));
}

/**
 * @param i Row of pixel in output image
 * @param j Column of pixel in output image
 * @return Averaged value of 2 v pixels (at and below) given (i,j) location
 */
int ImageAcquisitionTest::vAvgValue(int i, int j) const
{
    uint8_t * p = yuv + 4 * (i * NAO_IMAGE_WIDTH + j);
    return (static_cast<int>(p[VOFFSET]));
            // static_cast<int>(p[NAO_IMAGE_ROW_OFFSET + VOFFSET]));
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
    return u * 2;               // @TODO why times 2?
}

/**
 * See yIndex for details. Does same but for v index.
 */
int ImageAcquisitionTest::vIndex(int i, int j) const
{
    int v = max( vAvgValue(i, j) - vZero(), 0);
    v = min( (v * vSlope()) >> 16 , vLimit());
    return v * 2;
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
    uint8_t *out_color =
        reinterpret_cast<uint8_t*>(out) + Y_IMAGE_BYTE_SIZE * 3;

    // Color image is just past Y Image in array
    return out_color[IMAGE_WIDTH * i + j];
}

void ImageAcquisitionTest::test_color_segmentation()
{
    for (int i=0; i < IMAGE_HEIGHT; ++i){
        for (int j=0; j < IMAGE_WIDTH; ++j){
            int a = colorValue(i,j);
            int b = tableLookup( yIndex(i,j),
                                 uIndex(i,j),
                                 vIndex(i,j) );
            GTE(a+1, b);
            LTE(a-1, b);
        }
    }
    PASSED(COLOR_SEGMENTATION);
}

/**
 * Test the averaging of the pixels by the image acquistion.
 */
void ImageAcquisitionTest::test_avg()
{
#ifndef __APPLE__
    clock_gettime(CLOCK_MONOTONIC, &startT_mono);
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &startT_process);
    clock_gettime(CLOCK_THREAD_CPUTIME_ID, &startT_thread);
#endif

    unsigned int clockTicks = _acquire_image_fast(table, &c, yuv, out);

#ifndef __APPLE__
    clock_gettime(CLOCK_MONOTONIC, &finishT_mono);
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &finishT_process);
    clock_gettime(CLOCK_THREAD_CPUTIME_ID, &finishT_thread);

    sum_clocks += clockTicks;
    min_clocks = min(min_clocks, clockTicks);

    sumTime_thread += (finishT_thread.tv_sec -
                       startT_thread.tv_sec) * MICROS_PER_SECOND;
    sumTime_thread += (finishT_thread.tv_nsec -
                       startT_thread.tv_nsec) / 1000;

    sumTime_mono += (finishT_mono.tv_sec -
                     startT_mono.tv_sec) * MICROS_PER_SECOND;
    sumTime_mono += (finishT_mono.tv_nsec -
                     startT_mono.tv_nsec) / 1000;

    sumTime_process += (finishT_process.tv_sec -
                        startT_process.tv_sec) * MICROS_PER_SECOND;
    sumTime_process += (finishT_process.tv_nsec -
                        startT_process.tv_nsec) / 1000;

    numFrames--;
    if (numFrames == 0){
        cout << "RUN TIMES:" << endl;
        cout << "\tAverage THREAD run time for _acquire_image: " <<
            sumTime_thread / 500 << endl;
        // cout << "\tAverage PROCESS run time for _acquire_image: " <<
        //     sumTime_process / 500 << endl;
        // cout << "\tAverage MONO run time for _acquire_image: " <<
        //     sumTime_mono / 500 << endl;
        cout << "\tAverage clock ticks: " << sum_clocks/500 << endl;
        cout << "\tMinimum clock ticks: " << min_clocks << endl;
        cout << "\tMinimum clock ticks per iteration: " <<
            (min_clocks)/(160.*240.) << endl;

        numFrames = 500;
        sumTime_thread = sumTime_mono = sumTime_process =
            sum_clocks = 0;
        min_clocks = 0xFFFFFFF;
    }
#endif

#ifndef PROFILE_ACQUIRE
    run_average_test();
#endif
}

void ImageAcquisitionTest::run_average_test(){
    // Make sure that the run didn't affect the initial image
    for (int i = 0; i < 640*480*2; ++i) {
        EQ_INT((int)yuvCopy[i] , (int)yuv[i]);
    }
    PASSED(PRESERVE_IMAGE);

    uint16_t* y_out = reinterpret_cast<uint16_t*>(out);
    // Check that the average works properly.
    for (int i = 0; i < IMAGE_HEIGHT; ++i) {
        for (int j=0; j < IMAGE_WIDTH; ++j){

            uint16_t output = y_out[i*IMAGE_WIDTH + j];
            EQ_INT ( (yAvgValue(i, j)), output);
        }
    }
    PASSED(Y_AVERAGES);

    uint16_t* uv_out = y_out + IMAGE_WIDTH * IMAGE_HEIGHT;

    for (int i = 0; i < IMAGE_HEIGHT; i++) {
        for (int j=0; j < IMAGE_WIDTH; j++){

            uint16_t u_output = uv_out[i*IMAGE_WIDTH*2 + j*2];
            uint16_t v_output = uv_out[i*IMAGE_WIDTH*2 + j*2 + 1];
            EQ_INT ( (vAvgValue(i, j)), u_output);
            EQ_INT ( (uAvgValue(i, j)), v_output);
            LT(u_output, 256);
            LT(v_output, 256);
        }
    }
    PASSED(UV_AVERAGES);
}

int ImageAcquisitionTest::runTests()
{
    setup(0,0,0,256,256,256,10,10,10);

#ifdef PROFILE_ACQUIRE
    while(true)
#endif
        test_avg();
    test_color_segmentation();

    return 0;
}

int main(int argc, char * argv[])
{
    ImageAcquisitionTest * iat = new ImageAcquisitionTest();
    return iat->runTests();
}
