#include "ImageConverterModule.h"
#include "VisionDef.h"
//#include "Profiler.h"
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
    //Message<PackedImage16> yMessage(0);
    //Message<PackedImage16> uMessage(0);
    //Message<PackedImage16> vMessage(0);
    //Message<ThresholdImage> thrMessage(0);
    PackedImage16 image;

    imageIn.latch();

    const YUVImage& yuv = imageIn.message();

    PackedImage16 tempOutput(yuv.width() / 4, yuv.height() * 7 / 4);

    ImageAcquisition::acquire_image_fast(table,
                                         params,
                                         yuv.pixelAddress(0, 0),
                                         tempOutput.pixelAddress(0, 0));

    image = tempOutput.window(0, 0, 320, 240);
    yImage.setMessage(Message<PackedImage16>(&image));

    image = tempOutput.window(0, 240, 320, 240);
    uImage.setMessage(Message<PackedImage16>(&image));

    image = tempOutput.window(0, 480, 320, 240);
    vImage.setMessage(Message<PackedImage16>(&image));

    ThresholdImage thr = tempOutput.window(0, 720, 320, 120);
    thrImage.setMessage(Message<ThresholdImage>(&thr));

    //yImage.setMessage(yMessage);
    //uImage.setMessage(uMessage);
    //vImage.setMessage(vMessage);
    //thrImage.setMessage(thrMessage);
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

}
}
