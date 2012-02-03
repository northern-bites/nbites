#include "BMPImageViewer.h"

using namespace qtool::image;
using namespace man::memory;

namespace qtool {
namespace viewer {

BMPImageViewer::BMPImageViewer(image::BMPImage::ptr image,
                                 QWidget *parent)
    : QWidget(parent), image(image) {
    setupUI();
}

BMPImageViewer::~BMPImageViewer() {
}

void BMPImageViewer::setupUI() {
    QVBoxLayout* layout = new QVBoxLayout;
    layout->addWidget(&imagePlaceholder);

    this->setLayout(layout);
}

void BMPImageViewer::updateView() {
    if (this->isVisible()) {
        image->updateBitmap();
        //enqueues a repaint - thread-safe
        this->QWidget::update();
        imagePlaceholder.setPixmap(QPixmap::fromImage(*(image->getBitmap())));
    }
}

void BMPImageViewer::showEvent(QShowEvent * ) {
    //explicitely update the bitmap when the widget becomes visible again
    //since it might have changed!
    this->updateView();
}

}
}
