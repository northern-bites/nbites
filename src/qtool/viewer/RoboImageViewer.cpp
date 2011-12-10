#include "RoboImageViewer.h"

using namespace qtool::image;

namespace qtool {
namespace viewer {

RoboImageViewer::RoboImageViewer(image::BMPImage::ptr image,
				 image::TestImage::ptr overlay,
                                 QWidget *parent)
    : QWidget(parent),
      image(image),
      overlay(overlay)
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
	overlay->updateBitmap();
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
    painter.setBackgroundMode(Qt::TransparentMode);
    painter.drawImage(QPoint(0, 0), image->getBitmap());
    painter.drawImage(QPoint(0, 0), overlay->getBitmap());
}

}
}
