
#include "BMPYUVImage.h"

namespace qtool {
namespace image {

using namespace man::corpus;

BMPYUVImage::BMPYUVImage(memory::MRawImages::const_ptr rawImages,
        Camera::Type which,
        ChannelType type, QObject* parent) :
        BMPImage(parent),
        yuvImage(rawImages, which),
        bitmapType(type)
{ }

bool BMPYUVImage::needToResizeBitmap() const {
    return bitmap.width() < yuvImage.getWidth() || bitmap.height() < yuvImage.getHeight();
}

// this whole thing is slow as balls! - Octavian
void BMPYUVImage::buildBitmap() {
    yuvImage.updateFromRawImage();
//    if (this->needToResizeBitmap()) {
//        bitmap = QPixmap(yuvImage.getWidth(),
//                        yuvImage.getHeight());
//    }

    bitmap = QImage(yuvImage.getWidth(),
                    yuvImage.getHeight(),
                    QImage::Format_RGB32);

    Color c;

    const byte** yImg = yuvImage.getYImage();
    const byte** uImg = yuvImage.getUImage();
    const byte** vImg = yuvImage.getVImage();

	for (int j = 0; j < getHeight(); ++j) {
	    QRgb* qImageLine = (QRgb*) (bitmap.scanLine((int)(j)));
		for (int i = 0; i < getWidth(); ++i) {
		    byte y = yImg[i][j], u = uImg[i][j], v = vImg[i][j];
		    byte color_byte;
		    QRgb rgb;
		    Color color;
		    color.setYuv(y, u, v);

			switch (this->bitmapType) {
			case RGB:
			    qImageLine[i] = color.getRGB();
				break;

			case Y:
			    qImageLine[i] = Color::makeRGBFromSingleByte(y);
				break;

			case U:
			    qImageLine[i] = Color::makeRGBFromSingleByte(u);
			    break;

			case V:
			    qImageLine[i] = Color::makeRGBFromSingleByte(v);
			    break;

			case Red:
			    qImageLine[i] = Color::makeRGBFromSingleByte(color.getRb());
				break;

			case Green:
			    qImageLine[i] = Color::makeRGBFromSingleByte(color.getGb());
				break;

			case Blue:
			    qImageLine[i] = Color::makeRGBFromSingleByte(color.getBb());
				break;

			case Hue:
			    color.setHsz(color.getH(), color.getS(), 0.875f);
			    qImageLine[i] = color.getRGB();
				break;

			case Saturation:
			    qImageLine[i] = Color::makeRGBFromSingleByte(color.getSb());
			    break;

			case Value:
			    qImageLine[i] = Color::makeRGBFromSingleByte(color.getVb());
			    break;

			default:
			    qImageLine[i] = Color::makeRGB(0, 0, 0);
				break;
			}
		}
	}
}
}
}
