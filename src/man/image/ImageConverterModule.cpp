#include "ImageConverterModule.h"
#include "VisionDef.h"
#include "Profiler.h"
#include <iostream>

namespace man {
namespace image {

using namespace messages;
using namespace portals;

ImageConverterModule::ImageConverterModule()
    : Module(),
      yImage(base()),
      whiteImage(base()),
      orangeImage(base()),
      greenImage(base()),
      params(y0, u0, v0, y1, u1, v1, yLimit, uLimit, vLimit)//,
  //    table(new unsigned char[tableByteSize])
{}

/*
ImageConverterModule::ImageConverterModule(char *table_pathname)
    : Module(),
      yImage(base()),
      uImage(base()),
      vImage(base()),
      thrImage(base()),
      params(y0, u0, v0, y1, u1, v1, yLimit, uLimit, vLimit),
      table(new unsigned char[tableByteSize])
{
    initTable(table_pathname);
}
*/
void ImageConverterModule::run_()
{
    std::cout << "ImageConverterMod Running" << std::endl;
    imageIn.latch();

    const YUVImage& yuv = imageIn.message();

    /* This assembly method used below is not general, it assumes the input is
       of size 640x480, therefore we make no attempt to keep this run_ method
       general, as only a 640x480 image will be processed correctly.
       IMPORTANT imageIn must be of size 640x480. */

    /* tempBuffer holds three 320x240 16-bit images and one 320x240 8-bit image
       IMPORTANT check out acquire_image_fast for more information on this image */

    /* ^^ tempBuffer now holds one 320*240 16-bit image and three 320*240 8-bit images */
    HeapPixelBuffer *tempBuffer = new HeapPixelBuffer(320*240*2*1 + 320*240*1*3);
    PackedImage16 tempOutput16(tempBuffer, 320, 1*240, 320);
    PackedImage8 tempOutput8(tempBuffer, 320, (1*2 + 3)*240, 320);

    PROF_ENTER(P_ACQUIRE_IMAGE);
    ImageAcquisition::acquire_image(240, 320, 320,
                    yuv.pixelAddress(0, 0),
                    (uint8_t*)tempOutput16.pixelAddress(0, 0));
    PROF_EXIT(P_ACQUIRE_IMAGE);

    // First 320x240 image = all the Y values in imageIn.message()
    PackedImage16 image = tempOutput16.window(0, 0, 320, 240);
    yImage.setMessage(Message<PackedImage16>(&image));

    // Second 320x240 image = all the whiteness rating values in imageIn.message()
    PackedImage8 image8 = tempOutput8.window(0, 2*240, 320, 240);
    whiteImage.setMessage(Message<PackedImage8>(&image8));

    // Third 320x240 image = all orangeness rating values in imageIn.message()
    image8 = tempOutput8.window(0, (2+1)*240, 320, 240);
    orangeImage.setMessage(Message<PackedImage8>(&image8));

    // Last 320x240 image = all greeness rating values in imageIn.message()
    image8 = tempOutput8.window(0, (3+1)*240, 320, 240);
    greenImage.setMessage(Message<PackedImage8>(&image8));
}

// Read a color table into memory from pathname
/*void ImageConverterModule::initTable(char *filename)
{
    FILE *fp = fopen(filename, "r");   //open table for reading

    if (fp == NULL) {
        printf("CAMERA::ERROR::initTable() FAILED to open filename: %s\n", filename);
        return;
    }

    // actually read the table into memory
    // color table is in VUY ordering
    int rval;
    for(int v=0; v < vLimit; ++v) {
        for(int u=0; u< uLimit; ++u) {
            rval = fread(&table[v * uLimit * yLimit + u * yLimit],
                         sizeof(unsigned char), yLimit, fp);
        }
    }

    printf("CAMERA::Loaded colortable %s.\n",filename);
    fclose(fp);
}

// Read a color table already in memory
void ImageConverterModule::changeTable(unsigned char *newTable)
{
    table = newTable;
  run_();
}

void ImageConverterModule::loadTable(unsigned char *newTable)
{
  table = newTable;
}

*/

}
}
