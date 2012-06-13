#include "BMPImageViewer.h"
#include <QDebug>

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
    BMPlayout = new QVBoxLayout(this);
    BMPlayout->addWidget(&imagePlaceholder);
    BMPlayout->setAlignment(Qt::AlignTop);
    BMPlayout->setSpacing(0);
    this->setLayout(BMPlayout);
}

void BMPImageViewer::updateView() {
    //shouldRedraw keeps it from redrawing faster
    //than we need paint it
    if (this->isVisible() && shouldRedraw) {
        image->updateBitmap();

        QImage* qimage = image->getBitmap();
        if (qimage) {
            imagePlaceholder.setPixmap(QPixmap::fromImage(*qimage));
        } else {
            imagePlaceholder.setText("Underlying Null image pointer!");
        }
        shouldRedraw = false;
    }
    //enqueues a repaint - thread-safe
    this->QWidget::update();
}

void BMPImageViewer::paintEvent(QPaintEvent* e) {
    QWidget::paintEvent(e);
    shouldRedraw = true;
}

void BMPImageViewer::showEvent(QShowEvent* e) {
    QWidget::showEvent(e);
    //explicitely update the bitmap when the widget becomes visible again
    //since it might have changed!
    shouldRedraw = true;
    this->updateView();
}

QVBoxLayout* BMPImageViewer::getLayout(){
    return BMPlayout;
}

}
}
