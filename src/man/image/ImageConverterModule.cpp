#include "ImageConverterModule.h"
#include "VisionDef.h"
#include "Profiler.h"
#include <iostream>

namespace man {
namespace image {

using namespace messages;
using namespace portals;

ImageConverterModule::ImageConverterModule(Camera::Type which)
    : Module(),
      yImage(base()),
      uImage(base()),
      vImage(base()),
      thrImage(base()),
      whichCamera(which),
      params(y0, u0, v0, y1, u1, v1, yLimit, uLimit, vLimit),
      table(new unsigned char[tableByteSize])
{
    switch (whichCamera)
    {
    case Camera::TOP:
        initTable("/home/nao/nbites/lib/table/top_table.mtb");
        break;
    case Camera::BOTTOM:
        initTable("/home/nao/nbites/lib/table/bottom_table.mtb");
        break;
    default:
        break;
    }
}

void ImageConverterModule::run_()
{
    imageIn.latch();

    const YUVImage& yuv = imageIn.message();

    /* This assembly method below is not general, it assumes the input image is
       of size 640x480, therefore we make no attempt to keep this run_ method
       general, as only a 640x480 image will be processed correctly.
       IMPORTANT imageIn must be of size 640x480. */
    // Temp buffer holds three 320x240 16-bit images and one 320x240 8-bit image
    HeapPixelBuffer *tempBuffer = new HeapPixelBuffer(320*240*2*3 + 320*240);
    PackedImage16 tempOutput16(tempBuffer, 320, 3*240, 320);
    PackedImage8 tempOutput8(tempBuffer, 320, (3*2 + 1)*240, 320);

    if (whichCamera == Camera::TOP)
    {
        PROF_ENTER(P_TOP_CONVERTER);
    }
    else
    {
        PROF_ENTER(P_BOT_CONVERTER);
    }
    ImageAcquisition::acquire_image_fast(table,
                                         params,
                                         yuv.pixelAddress(0, 0),
                                         tempOutput16.pixelAddress(0, 0));
    if (whichCamera == Camera::TOP)
    {
        PROF_EXIT(P_TOP_CONVERTER);
    }
    else
    {
        PROF_EXIT(P_BOT_CONVERTER);
    }

    PackedImage16 image = tempOutput16.window(0, 0, 320, 240);
    yImage.setMessage(Message<PackedImage16>(&image));

    image = tempOutput16.window(0, 240, 320, 240);
    uImage.setMessage(Message<PackedImage16>(&image));

    image = tempOutput16.window(0, 480, 320, 240);
    vImage.setMessage(Message<PackedImage16>(&image));

    ThresholdImage thr = tempOutput8.window(0, 3*2*240, 320, 240);
    thrImage.setMessage(Message<ThresholdImage>(&thr));
}

void ImageConverterModule::initTable(const std::string& filename)
{
    FILE *fp = fopen(filename.c_str(), "r");   //open table for reading

    if (fp == NULL) {
        printf("CAMERA::ERROR::initTable() FAILED to open filename: %s\n", filename.c_str());
        return;
    }

    // actually read the table into memory
    // color table is in VUY ordering
    int rval;
    if (whichCamera == Camera::TOP) {
        for(int v=0; v < vLimit; ++v) {
            for(int u=0; u< uLimit; ++u) {
                rval = fread(&table[v * uLimit * yLimit + u * yLimit],
                             sizeof(unsigned char), yLimit, fp);
            }
        }
    }
    else {
        for (int v=0; v < vLimit; ++v) {
            for (int u=0; u< uLimit; ++u) {
                rval = fread(&table[v * uLimit * yLimit + u * yLimit],
                             sizeof(unsigned char), yLimit, fp);
            }
        }
    }

    printf("CAMERA::Loaded colortable %s.\n",filename.c_str());
    fclose(fp);
}

void ImageConverterModule::initTable(unsigned char* otherTable)
{
    table = otherTable;
}

}
}
