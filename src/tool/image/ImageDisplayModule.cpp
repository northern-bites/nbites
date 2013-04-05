#include "ImageDisplayModule.h"
#include <iostream>

namespace tool {
namespace image {

ImageDisplayModule::ImageDisplayModule(QWidget* parent) : QLabel(parent)
{
    setText(tr("No image loaded!"));
}

void ImageDisplayModule::run_()
{
    imageIn.latch();
    // setPixmap(QPixmap::fromImage(
    //               QImage((unsigned char*)imageIn.message().pixelAddress(0, 0),
    //                      imageIn.message().width(),
    //                      imageIn.message().height(),
    //                      QImage::Format_ARGB32)));
}

}
}
