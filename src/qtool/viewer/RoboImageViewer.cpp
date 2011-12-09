#include "RoboImageViewer.h"

using namespace qtool::image;

namespace qtool {
namespace viewer {

RoboImageViewer::RoboImageViewer(image::BMPImage::ptr image,
                                 QWidget *parent)
    : QWidget(parent),
      image(image) {

    connect(image.get(), SIGNAL(bitmapUpdated()),
            this, SLOT(updateView()));
}

RoboImageViewer::~RoboImageViewer() {
}

void RoboImageViewer::updateView() {
    //enqueues a repaint - thread-safe
    this->QWidget::update();
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
