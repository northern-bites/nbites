#include "RoboImageViewer.h"

using namespace qtool::image;

#include <iostream>
using namespace std;

namespace qtool {
namespace viewer {

RoboImageViewer::RoboImageViewer(image::BMPImage::ptr image,
                                 QWidget *parent)
    : QWidget(parent), image(image) {

}

RoboImageViewer::~RoboImageViewer() {
}

void RoboImageViewer::updateView() {
    if (this->isVisible()) {
        image->updateBitmap();
        //enqueues a repaint - thread-safe
        this->QWidget::update();
        cout << "updating view!" << endl;
    }
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

void RoboImageViewer::showEvent(QShowEvent * ) {
    //explicitely update the bitmap when the widget becomes visible again
    //since it might have changed!
    this->updateView();
}

}
}
