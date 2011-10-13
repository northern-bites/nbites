#include "RoboImageViewer.h"
#include <QMouseEvent>

namespace qtool {
namespace viewer {

RoboImageViewer::RoboImageViewer(man::memory::MImage::const_ptr rawImage,
                                 QLabel *infoLabel, QWidget *parent)
    : QWidget(parent),
      image(new BMPYUVImage(rawImage)),
      infoLabel(infoLabel)
{
    setBackgroundRole(QPalette::Base);
    setAutoFillBackground(true);
}

RoboImageViewer::~RoboImageViewer() {
	delete image;
}

void RoboImageViewer::update(qtool::data::MObject_ID) {
    this->updateBitmap();
}

void RoboImageViewer::updateBitmap() {
	image->updateFromRawImage();
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
    this->updateBitmap();
    painter.drawImage(QPoint(0, 0), image->getBitmap());
}

}
}
