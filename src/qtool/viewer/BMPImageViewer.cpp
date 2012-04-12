#include "BMPImageViewer.h"

using namespace qtool::image;
using namespace man::memory;

namespace qtool {
namespace viewer {

BMPImageViewer::BMPImageViewer(image::BMPImage* image,
                               QWidget *parent)
    : QWidget(parent), image(image) {
    setupUI();
}

BMPImageViewer::~BMPImageViewer() {
}

void BMPImageViewer::setupUI() {
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addWidget(&imagePlaceholder);
    this->setLayout(layout);
}

void BMPImageViewer::updateView() {
    if (this->isVisible()) {
        image->updateBitmap();
        QImage* qimage = image->getBitmap();
        if (qimage) {
            imagePlaceholder.setPixmap(QPixmap::fromImage(*(qimage)));
        } else {
            imagePlaceholder.setText("Underlying Null image pointer!");
        }
        //enqueues a repaint - thread-safe
        this->QWidget::update();
    }
}

void BMPImageViewer::showEvent(QShowEvent * e) {
    QWidget::showEvent(e);
    //explicitely update the bitmap when the widget becomes visible again
    //since it might have changed!
    this->updateView();
}

}
}
