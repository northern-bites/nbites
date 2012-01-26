
#include "BMPYUVImage.h"

namespace qtool {
namespace image {

BMPYUVImage::BMPYUVImage(man::memory::MImage::const_ptr rawImage) :
	yuvImage(rawImage),
	bitmapType(Color)
{ }

bool BMPYUVImage::needToResizeBitmap() const {
    return bitmap.width() < yuvImage.getWidth() || bitmap.height() < yuvImage.getHeight();
}

void BMPYUVImage::buildBitmap() {
    yuvImage.updateFromRawImage();
    if (this->needToResizeBitmap()) {
        bitmap = QImage(yuvImage.getWidth(),
                        yuvImage.getHeight(),
                        QImage::Format_RGB32);
    }

    ColorSpace c;

    byte** yImg = yuvImage.getYImage();
    byte** uImg = yuvImage.getUImage();
    byte** vImg = yuvImage.getVImage();

	for (int j = 0; j < getHeight(); ++j) {
	    QRgb* qImageLine = (QRgb*) (bitmap.scanLine((int)(j)));
		for (int i = 0; i < getWidth(); ++i) {
		    byte y = yImg[i][j], u = uImg[i][j], v = vImg[i][j];
			int r, g, b;
			switch (this->bitmapType) {
			case Color:
			    qImageLine[i] = ColorSpace::RGBFromYUV(y, u, v);
				break;

			case Y:
				r = g = b = c.getY();
				break;

			case U:
				r = g = b = c.getU();
				break;

			case V:
				r = g = b = c.getV();
				break;

			case Red:
				r = g = b = c.getRb();
				break;

			case Green:
				r = g = b = c.getGb();
				break;

			case Blue:
				r = g = b = c.getBb();
				break;

			case Hue:
				if (c.getS() >= 0.25f && c.getY() >= 0.2f) {
					ColorSpace h = ColorSpace();
					h.setHsz(c.getH(), c.getS(), 0.875f);
					r = h.getRb();
					g = h.getGb();
					b = h.getBb();
				} else
					r = g = b = 0;
				break;

			case Saturation:
				r = g = b = c.getSb();
				break;

			case Value:
				r = g = b = c.getZb();
				break;

			default:
				r = g = b = 0;
				break;
			}
//			QRgb value = qRgb(r, g, b);
//			bitmap.setPixel(i, j, value);
		}
	}
}
}
}
