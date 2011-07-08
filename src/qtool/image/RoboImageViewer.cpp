#include "RoboImageViewer.h"
#include <QMouseEvent>

RoboImageViewer::RoboImageViewer(man::memory::RoboImage::const_ptr roboImage,
                                 QLabel *infoLabel, QWidget *parent)
    : QWidget(parent),
      image(new BMPYUVImage(roboImage)),
      infoLabel(infoLabel)
{
    setBackgroundRole(QPalette::Base);
    setAutoFillBackground(true);
}

RoboImageViewer::~RoboImageViewer() {
	delete image;
}

void RoboImageViewer::updateBitmap() {
	image->updateFromRoboImage();
}

QSize RoboImageViewer::minimumSizeHint() const
{
    return QSize(image->getWidth(), image->getHeight());
}

QSize RoboImageViewer::sizeHint() const
{
    return QSize(image->getWidth(), image->getHeight());
}

void RoboImageViewer::paintEvent(QPaintEvent * /* event */)
{

    QPainter painter(this);
    painter.drawImage(QPoint(0, 0), image->getBitmap());

//    QRect draw;
//    int red, green, blue;
//    bool found;
//    for (int i = 0; i < yuvImage->getHeight(); i++)
//    {
//        for (int j = 0; j < yuvImage->getWidth(); j++)
//        {
//        	red = yuvImage->getRed(j, i);
//            green = yuvImage->getGreen(j, i);
//            blue = yuvImage->getBlue(j, i);
//
//            QColor col(red, green, blue);
//            painter.setPen(col);
//            draw.setCoords(j, i, j + 1, i + 1);
//            painter.fillRect(draw, col);
//        }
//    }
}
