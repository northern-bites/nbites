#include "ImageAcquisitionTest.h"

#include "ColorParams.h"
#include "VisionDef.h"

#include <stdlib.h>
#include <cstdio>

#include <iostream>
#include <fstream>

using namespace std;


typedef unsigned char uchar;

extern "C" void _acquire_image(uchar * colorTable, ColorParams* params,
                               uchar* yuvImage, uchar* outImage);

ImageAcquisitionTest::ImageAcquisitionTest()
{

}

void ImageAcquisitionTest::test_avg()
{
    uchar * table = (uchar*)malloc(128*128*128*sizeof(uchar));

    for (int i = 0; i < 128*128*128; ++i) {
        table[i] = 1;
    }

    unsigned long long ySlopeOne = 0x0000400000004000ULL;
    unsigned long long uvSlopeOne = 0x4000400040004000ULL;
    // 0001 0001 0001 0001
    unsigned long long fourOnes = 0x0001000100010001ULL;
    // 1(15 0s...)(repeated 4 times)
    unsigned long long four128s = 0x8000800080008000ULL;
    // | 128^2 | 128 | 128^2 | 128 |
    unsigned long long uvDimVal = 0x4000008040000080ULL;

    ColorParams c = {0,
                     ySlopeOne,
                     four128s,
                     0ULL,
                     uvSlopeOne,
                     four128s,
                     uvDimVal};

    ColorParams * cp = &c;

    uchar * yuv = (uchar*)malloc(640*480*2*sizeof(uchar));
    assert(yuv != NULL);
    for (int i = 0; i < 640*480*2; ++i) {
        yuv[i] = static_cast<uchar>((i * i +7*i)%255);
    }

    uchar * yuvCopy = (uchar*)malloc(640*480*2*sizeof(uchar));
    assert(yuvCopy != NULL);
    for (int i = 0; i < 640*480*2; ++i) {
        yuvCopy[i] = yuv[i];
    }


    const int OUT_IMG_SIZE = 320 * 240 * 2;
    const int OUT_IMG_Y_SIZE = 320 * 240;
    uchar * out = (uchar*)malloc(OUT_IMG_SIZE*sizeof(uchar));
    assert(out != NULL);
    for (int i = 0; i < OUT_IMG_SIZE; ++i) {
        out[i] = 1;
    }

    _acquire_image(table, cp, yuv, out);

    for (int i = 0; i < 640*480*2; ++i) {
        EQ_INT((int)yuvCopy[i] , (int)yuv[i]);
    }
    PASSED(PRESERVE_IMAGE);


    for (int i = 0; i < 480; i += 2) {
        for (int j=0; j < 640; j += 4) {

            int inputAvg = ((int)yuv[i*640*2+j] +
                            (int)yuv[i*640*2+j+2] +
                            (int)yuv[(i+1)*640*2+j] +
                            (int)yuv[(i+1)*640*2+j+2]) >> 2;

            int output = (int)out[(i>>1)*320 + (j>>2)];

            EQ_INT( inputAvg, output);

        }
    }
}

void ImageAcquisitionTest::test_out_sane_values()
{
    uchar * table = (uchar*)malloc(128*128*128*sizeof(uchar));

    for (int i = 0; i < 128*128*128; ++i) {
        table[i] = 1;
    }

    unsigned long long ySlopeOne = 0x0000400000004000ULL;
    unsigned long long uvSlopeOne = 0x4000400040004000ULL;
    // 0001 0001 0001 0001
    unsigned long long fourOnes = 0x0001000100010001ULL;
    // 1(15 0s...)(repeated 4 times)
    unsigned long long four128s = 0x8000800080008000ULL;
    // | 128^2 | 128 | 128^2 | 128 |
    unsigned long long uvDimVal = 0x4000008040000080ULL;

    ColorParams c = {0,
                     ySlopeOne,
                     four128s,
                     0ULL,
                     uvSlopeOne,
                     four128s,
                     uvDimVal};

    ColorParams * cp = &c;

    uchar * yuv = (uchar*)malloc(640*480*2*sizeof(uchar));
    assert(yuv != NULL);
    for (int i = 0; i < 640*480*2; ++i) {
        yuv[i] = static_cast<uchar>(i/(i+3)*4+7); // Fill in a random value
    }

    const int OUT_IMG_SIZE = 320 * 240 * 2;
    uchar * out = (uchar*)malloc(OUT_IMG_SIZE*sizeof(uchar));
    assert(out != NULL);
    for (int i = 0; i < OUT_IMG_SIZE; ++i) {
        out[i] = 1;
    }

    // printf("Table Address is %p\n", table);
    // printf("Params Address is %p\n", cp);
    // printf("Yuv Address is %p\n", yuv);
    // printf("Out Address is %p\n", out);
    // fflush(stdout);

    _acquire_image(table, cp, yuv, out);

    ofstream OutFile("out.txt");

    for (int i = 0; i < OUT_IMG_SIZE; ++i) {
        OutFile << (int)out[i];
        if (i %320 == 0)
            OutFile << endl;
    }

    for (int i = 0; i < OUT_IMG_SIZE; ++i) {
        GTE((int)out[i] , 0);
        LTE((int)out[i] , 255);
    }
    PASSED(TEST_OUT_SANE_VALUES);
}

int ImageAcquisitionTest::runTests()
{
    test_out_sane_values();
    test_avg();
    return 0;
}

int main(int argc, char * argv[])
{
    ImageAcquisitionTest * iat = new ImageAcquisitionTest();
    return iat->runTests();
}
