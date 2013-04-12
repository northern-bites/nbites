#include "ImageDisplayModule.h"
#include <iostream>

namespace tool {
namespace image {

ThresholdedImageDisplayModule::ThresholdedImageDisplayModule(QWidget* parent)
    : QLabel(parent),
      filter(ALL_COLORS)
{
    setText(tr("No image loaded!"));
}

void ThresholdedImageDisplayModule::run_()
{
    imageIn.latch();
    setPixmap(QPixmap::fromImage(makeImage(filter)));
}

QImage ThresholdedImageDisplayModule::makeImage(byte filter_)
{
    QImage image(imageIn.message().width(),
                 imageIn.message().height(),
                 QImage::Format_RGB32);

    for (int j = 0; j < image.height(); ++j)
    {
        QRgb* bitmapLine = (QRgb*) image.scanLine(j);
        for (int i = 0; i < image.width(); ++i)
        {
            int rawColor = imageIn.message().getPixel(i, j);
            int threshColor = 0, mix = 1;
            for (int c = 0; c < Color::NUM_COLORS; c++) {
                if ((rawColor & Color_bits[c]) > 0 &&
                    ((Color_bits[c] & filter_) > 0))
                {
                    threshColor += Color_RGB[c];
                    threshColor /= mix;
                    mix++;
                }
            }

            if (threshColor == 0) threshColor = Color_RGB[0];

            bitmapLine[i] = threshColor;
        }
    }

    return image;
}

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

QImage ImageDisplayModule::makeImageOfChannel(ChannelType channel_)
{
    // This makes the clicking work properly for listener
    // Kind of a hack
    this->setFixedWidth(imageIn.message().width()/2);
    this->setFixedHeight(imageIn.message().height());

    messages::MemoryImage8 yImg = imageIn.message().yImage();
    messages::MemoryImage8 uImg = imageIn.message().uImage();
    messages::MemoryImage8 vImg = imageIn.message().vImage();

    QImage image(yImg.width(), yImg.height(), QImage::Format_RGB32);
    Color c;

	for (int j = 0; j < imageIn.message().height(); j++)
    {
	    rgb_value* qImageLine = (rgb_value*) (image.scanLine(j));
		for (int i = 0; i < (imageIn.message().width())/2; ++i)
        {
		    byte y = yImg.getPixel(i, j);
            byte u = uImg.getPixel(i/2, j);
            byte v = vImg.getPixel(i/2, j);

		    byte color_byte;
		    QRgb rgb;
		    c.setYuv(y, u, v);

			switch (channel_)
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

ImageDisplayListener::ImageDisplayListener(QWidget *parent)
    : ImageDisplayModule(parent),
      brushSize(DEFAULT_BRUSH_SIZE)
{

    QWidget::setAttribute(Qt::WA_NoMousePropagation, true );
}

void ImageDisplayListener::mouseReleaseEvent ( QMouseEvent * event )
{
    bool left;
    if(event->button() == Qt::LeftButton) {
        left = true;
    } else {
        left = false;
    }

    int mouseX = event->x();
    int mouseY = event->y();

    emit mouseClicked((int)((float)mouseX),
					  (int)((float)mouseY), brushSize, left);
}

void ImageDisplayListener::wheelEvent(QWheelEvent* event) {
    if (event->delta() > 0) {
        brushSize++;
    } else {
        brushSize--;
    }

    if (brushSize == 0) {
        brushSize = 1;
    }
    updateBrushCursor();
}

void ImageDisplayListener::updateBrushCursor() {
    QPixmap cursor(brushSize, brushSize);

    cursor.fill(brushColor);
    this->setCursor(QCursor(cursor, brushSize, 0)); // not exactly sure why this works
}

}
}
