
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

void BMPYUVImage::updateBitmap() {
    yuvImage.updateFromRawImage();
    if (this->needToResizeBitmap()) {
        bitmap = QImage(yuvImage.getWidth(),
                        yuvImage.getHeight(),
                        QImage::Format_ARGB32);
    }

    ColorSpace c;

	for (int j = 0; j < getHeight(); ++j)
		for (int i = 0; i < getWidth(); ++i) {
			c.setYuv(yuvImage.getY(i,j), yuvImage.getU(i,j), yuvImage.getV(i,j));
			int r, g, b;
			switch (this->bitmapType) {
			case Color:
				r = c.getRb();
				g = c.getGb();
				b = c.getBb();
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
			QRgb value = qRgb(r, g, b);
			bitmap.setPixel(i, j, value);
		}
}
}
}
