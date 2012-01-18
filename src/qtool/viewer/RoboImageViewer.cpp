#include "RoboImageViewer.h"

using namespace qtool::image;

namespace qtool {
namespace viewer {

RoboImageViewer::RoboImageViewer(image::BMPImage::ptr image,
                                 QWidget *parent)
    : QWidget(parent),
      image(image)
{}

RoboImageViewer::~RoboImageViewer() {
}

void RoboImageViewer::update(qtool::data::MObject_ID) {
    this->updateBitmap();
    //enqueues a repaint - thread-safe
    this->QWidget::update();
}

void RoboImageViewer::updateBitmap() {
	image->updateBitmap();
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
}

}
}
