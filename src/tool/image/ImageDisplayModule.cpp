#include "ImageDisplayModule.h"
#include <iostream>

namespace tool {
namespace image {

ImageDisplayModule::ImageDisplayModule(QWidget* parent) : QLabel(parent),
                                                          channel(RGB)
{
    setText(tr("No image loaded!"));
}

void ImageDisplayModule::run_()
{
    imageIn.latch();
    setPixmap(QPixmap::fromImage(makeImageOfChannel(channel)));
}

QImage ImageDisplayModule::makeImageOfChannel(ChannelType channel)
{
    messages::MemoryImage8 yImg = imageIn.message().yImage();
    messages::MemoryImage8 uImg = imageIn.message().uImage();
    messages::MemoryImage8 vImg = imageIn.message().vImage();

    QImage image(yImg.width(), yImg.height(), QImage::Format_RGB32);
    Color c;

	for (int j = 0; j < height(); j++)
    {
	    rgb_value* qImageLine = (rgb_value*) (image.scanLine(j));
		for (int i = 0; i < width()/2; ++i)
        {
		    byte y = yImg.getPixel(i, j);
            byte u = uImg.getPixel(i/2, j);
            byte v = vImg.getPixel(i/2, j);

		    byte color_byte;
		    QRgb rgb;
		    c.setYuv(y, u, v);

			switch (channel)
            {
			case RGB:
			    qImageLine[i] = c.getRGB();
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
			    qImageLine[i] = Color::makeRGBFromSingleByte(c.getRb());
				break;

			case Green:
			    qImageLine[i] = Color::makeRGBFromSingleByte(c.getGb());
				break;

			case Blue:
			    qImageLine[i] = Color::makeRGBFromSingleByte(c.getBb());
				break;

			case Hue:
			    c.setHsz(c.getH(), c.getS(), 0.875f);
			    qImageLine[i] = c.getRGB();
				break;

			case Saturation:
			    qImageLine[i] = Color::makeRGBFromSingleByte(c.getSb());
			    break;

			case Value:
			    qImageLine[i] = Color::makeRGBFromSingleByte(c.getVb());
			    break;

			default:
			    qImageLine[i] = Color::makeRGB(0, 0, 0);
				break;
			}
		}
	}

    return image;
}

}
}
