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
			if (this->isVisible()) {
				image->updateBitmap();
				QPixmap* qimage = image->getBitmap();
				if (qimage) {
					imagePlaceholder.setPixmap(*qimage);
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

		QVBoxLayout* BMPImageViewer::getLayout(){
			return BMPlayout;
		}

	}
}
