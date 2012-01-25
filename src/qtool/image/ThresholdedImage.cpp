#include "ThresholdedImage.h"
#include "colorcreator/ColorCreator.h"

namespace qtool {
namespace image {

ThresholdedImage::ThresholdedImage(
        boost::shared_ptr<const man::memory::proto::PImage> rawImage) :
    rawImage(rawImage)
{ }

bool ThresholdedImage::needToResizeBitmap() const {
    return bitmap.width() < rawImage->width() ||
           bitmap.height() < rawImage->height();
}

void ThresholdedImage::updateBitmap() {
    if (this->needToResizeBitmap()) {
        bitmap = QImage(rawImage->width(),
                        rawImage->height(),
                        QImage::Format_RGB32);
    }

    for (int j = 0; j < getHeight(); ++j)
        for (int i = 0; i < getWidth(); ++i) {
            int color = rawImage->image()[j*rawImage->width() + i];
            bitmap.setPixel(i, j,
                    colorcreator::ColorCreator::RGBcolorValue[color].rgb());
        }
}
}
}
