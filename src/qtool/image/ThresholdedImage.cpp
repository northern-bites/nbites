#include "ThresholdedImage.h"

namespace qtool {
namespace image {

ThresholdedImage::ThresholdedImage(
        boost::shared_ptr<const man::memory::proto::PImage> rawImage, QObject* parent) :
    BMPImage(parent), rawImage(rawImage)
{ }

bool ThresholdedImage::needToResizeBitmap() const {
    return bitmap.width() < rawImage->width() ||
           bitmap.height() < rawImage->height();
}

void ThresholdedImage::buildBitmap() {
//    if (this->needToResizeBitmap()) {
//        bitmap = QImage(rawImage->width(),
//                        rawImage->height(),
//                        QImage::Format_RGB32);
//    }

    QImage qimage = QImage(rawImage->width(),
                           rawImage->height(),
                           QImage::Format_RGB32);

    for (int j = 0; j < getHeight(); ++j) {
        QRgb* bitmapLine = (QRgb*) qimage.scanLine(j);
        for (int i = 0; i < getWidth(); ++i) {
            int color = rawImage->image()[j*rawImage->width() + i];
            bitmapLine[i] = Color_RGB[color];
        }
    }

    bitmap = QPixmap::fromImage(qimage);
}
}
}
