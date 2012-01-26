
#include "BMPYUVImage.h"

namespace qtool {
namespace image {

BMPYUVImage::BMPYUVImage(man::memory::MImage::const_ptr rawImage, BitmapType type) :
	yuvImage(rawImage),
	bitmapType(type)
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
		    byte color_byte;
		    QRgb rgb;
		    QColor color;

			switch (this->bitmapType) {
			case Color:
			    qImageLine[i] = ColorSpace::RGBFromYUV(y, u, v);
				break;

			case Y:
			    qImageLine[i] = ColorSpace::makeRGB(y, y, y);
				break;

			case U:
			    qImageLine[i] = ColorSpace::makeRGB(u, u, u);
			    break;

			case V:
			    qImageLine[i] = ColorSpace::makeRGB(v, v, v);
			    break;

			case Red:
			    color_byte = ColorSpace::redFromYUV(y, u, v);
			    qImageLine[i] = ColorSpace::makeRGBFromSingleByte(color_byte);
				break;

			case Green:
			    color_byte = ColorSpace::greenFromYUV(y, u, v);
			    qImageLine[i] = ColorSpace::makeRGBFromSingleByte(color_byte);
				break;

			case Blue:
			    color_byte = ColorSpace::blueFromYUV(y, u, v);
			    qImageLine[i] = ColorSpace::makeRGBFromSingleByte(color_byte);
				break;

			case Hue:
			    rgb = ColorSpace::RGBFromYUV(y, u, v);
			    color = QColor(rgb);
			    color.toHsv();
			    color.setHsv(color.hsvHue(), color.hsvSaturation(), 200);
			    qImageLine[i] = color.rgb();
				break;

			case Saturation:
			    rgb = ColorSpace::RGBFromYUV(y, u, v);
			    color = QColor(rgb);
			    color_byte = (byte) (color.hsvSaturation());
			    qImageLine[i] = ColorSpace::makeRGBFromSingleByte(color_byte);
			    break;

			case Value:
			    rgb = ColorSpace::RGBFromYUV(y, u, v);
			    color = QColor(rgb);
			    color_byte = static_cast<byte>(color.value());
			    qImageLine[i] = ColorSpace::makeRGBFromSingleByte(color_byte);
			    break;

			default:
			    qImageLine[i] = ColorSpace::makeRGB(0, 0, 0);
				break;
			}
		}
	}
}
}
}
