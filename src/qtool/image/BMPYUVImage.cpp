
#include "BMPYUVImage.h"

BMPYUVImage::BMPYUVImage(man::memory::RoboImage::const_ptr _roboImage) :
	YUVImage(_roboImage),
	bitmap(width, height, QImage::Format_RGB32)
{ }

void BMPYUVImage::updateFromRoboImage() {
	YUVImage::updateFromRoboImage();
	updateBitmap();
}

void BMPYUVImage::updateBitmap() {
	ColorSpace c;

	for (int j = 0; j < height; ++j)
		for (int i = 0; i < width; ++i) {
			c.setYuv(yImg[i][j], uImg[i][j], vImg[i][j]);
			int r, g, b;
			switch (this->bitmapType) {
			case Color:
				r = c.getRb();
				g = c.getGb();
				b = c.getBb();
				break;

			case Y:
				r = g = b = yImg[i][j];
				break;

			case U:
				r = g = b = uImg[i][j];
				break;

			case V:
				r = g = b = vImg[i][j];
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
