#include "BMPImageViewer.h"

using namespace qtool::image;

#include <iostream>
using namespace std;

namespace qtool {
namespace viewer {

BMPImageViewer::BMPImageViewer(image::BMPImage::ptr image,
                                 QWidget *parent)
    : QWidget(parent), image(image) {

    QVBoxLayout* layout = new QVBoxLayout;
    layout->addWidget(&imagePlaceholder);

    this->setLayout(layout);
}

BMPImageViewer::~BMPImageViewer() {
}

void BMPImageViewer::updateView() {
    if (this->isVisible()) {
        image->updateBitmap();
        //enqueues a repaint - thread-safe
        this->QWidget::update();
        imagePlaceholder.setPixmap(QPixmap::fromImage(image->getBitmap()));
    }
}

void BMPImageViewer::showEvent(QShowEvent * ) {
    //explicitely update the bitmap when the widget becomes visible again
    //since it might have changed!
    this->updateView();
}

}
}
