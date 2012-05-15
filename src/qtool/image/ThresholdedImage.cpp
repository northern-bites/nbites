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
    if (this->needToResizeBitmap()) {
        bitmap = QImage(rawImage->width(),
                        rawImage->height(),
                        QImage::Format_RGB32);
    }

    for (int j = 0; j < getHeight(); ++j) {
        QRgb* bitmapLine = (QRgb*) bitmap.scanLine(j);
        for (int i = 0; i < getWidth(); ++i) {
            int rawColor = rawImage->image()[j*rawImage->width() + i];
	    int threshColor = 0, mix = 1;
	    for (int c = 0; c < NUM_COLORS; c++) {
	      if ((rawColor & Color_bits[c]) > 0) {
		threshColor += Color_RGB[c];
		threshColor /= mix;
		mix++;
	      }
	    }
            bitmapLine[i] = threshColor;
        }
    }
}

void ThresholdedImage::scaleBitmap_640_480() {
    bitmap.scaled(640, 480);
}
}
}
