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

	initialW = image->getWidth();
	initialH = image->getHeight();
}

void BMPImageViewer::updateView(bool resize) {
    //shouldRedraw keeps it from redrawing faster
    //than we need paint it
    if (this->isVisible() && shouldRedraw) {
        image->updateBitmap();

        QImage* qimage = image->getBitmap();
		scaledPixmap = QPixmap::fromImage(*qimage);

		int maxWidth = this->width();
		int maxHeight = this->height();

		if(maxWidth > initialW && maxHeight > initialH && resize) {
			if((float)maxWidth/image->getWidth() < (float)maxHeight/image->getHeight())
				scaledPixmap = scaledPixmap.scaledToWidth(maxWidth);
			else scaledPixmap = scaledPixmap.scaledToHeight(maxHeight);
		}

		scale = scaledPixmap.width()/(float)image->getWidth();

        if (qimage) {
            imagePlaceholder.setPixmap(scaledPixmap);
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
