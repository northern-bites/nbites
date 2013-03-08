#include "ImageTranscriberModule.h"
#include <iostream>

using namespace portals;

namespace man {
namespace image {

ImageTranscriberModule::ImageTranscriberModule()
    : Module(),
      topImageOut(base()),
      bottomImageOut(base()),
      topImageTranscriber(Camera::TOP, &topImageOut),
      bottomImageTranscriber(Camera::BOTTOM, &bottomImageOut)
{
    topImageTranscriber.initTable("/home/nao/nbites/lib/table/top_table.mtb");
    bottomImageTranscriber.initTable("/home/nao/nbites/lib/table/bottom_table.mtb");
}

void ImageTranscriberModule::run_()
{
    topImageTranscriber.acquireImage();
    bottomImageTranscriber.acquireImage();
}

}
}
