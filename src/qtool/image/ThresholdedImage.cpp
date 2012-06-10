#include "ThresholdedImage.h"

namespace qtool {
namespace image {

using namespace data;

ThresholdedImage::ThresholdedImage(
        ImagePtr rawImage,
        QObject* parent, byte filter) :
    BMPImage(parent), rawImage(rawImage), filter(filter)
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
            int rawColor = rawImage->image()[j*rawImage->width() + i];
            int threshColor = 0, mix = 1;
            for (int c = 0; c < NUM_COLORS; c++) {
                if ((rawColor & Color_bits[c]) > 0 && ((Color_bits[c] & filter) > 0)) {
                    threshColor += Color_RGB[c];
                    threshColor /= mix;
                    mix++;
                }
            }
            if (threshColor == 0) threshColor = Color_RGB[0];
            bitmapLine[i] = threshColor;
        }
    }

    bitmap = QPixmap::fromImage(qimage);
    scaleBitmap_640_480();
}

void ThresholdedImage::scaleBitmap_640_480() {
    bitmap = bitmap.scaled(640, 480);
}

void ThresholdedImage::scaleBitmap_320_240() {
    bitmap.scaled(320, 240);
}

}
}
