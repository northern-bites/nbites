#include "FieldViewer.h"
#include "man/memory/Memory.h"
#include "image/BMPImage.h"

#include <QVBoxLayout>
#include <vector>


namespace qtool {
	namespace viewer {

		using namespace data;
		using namespace man::memory;
		using namespace image;
		using namespace overseer;

		FieldViewer::FieldViewer(DataManager::ptr dataManager, QWidget* parent):
			QWidget(parent),
			dataManager(dataManager),
			startButton(new QPushButton("Locate Robots", this)),
			stopButton(new QPushButton("Stop Location", this)) {

			mainLayout = new QVBoxLayout(this);
			scaleFactor = (float)1.25;

			//field image painted via overlay of robots, field
			fieldImage = new PaintField(this, scaleFactor);
			bot_locs = new PaintBots(this, scaleFactor);
			overlayView = new OverlayedImage(fieldImage, bot_locs, this);
			fieldView = new BMPImageViewer(fieldImage, this);

			spacer = new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding);

			field = new QHBoxLayout();
			field->addWidget(fieldView);

			buttonLayout = new QHBoxLayout();
			buttonLayout->setSpacing(10);
			buttonLayout->addWidget(startButton);
			connect(startButton, SIGNAL(clicked()), this, SLOT(drawBots()));
			buttonLayout->addWidget(stopButton);
			connect(stopButton, SIGNAL(clicked()), this, SLOT(stopDrawing()));

			//paint the field
			mainLayout->addLayout(buttonLayout);
			mainLayout->addLayout(field);
			mainLayout->addItem(spacer);
			this->setLayout(mainLayout);
		}

		void FieldViewer::drawBots(){
			if(!keepDrawing){
				bot_locs->locs->startListening();
				keepDrawing = true;
				while(keepDrawing){
					delete field;
					delete fieldView;
					fieldView = new BMPImageViewer(overlayView, this);
					fieldView->getLayout()->setAlignment(Qt::Alignment());
					field = new QHBoxLayout();
					field->addWidget(fieldView);
					mainLayout->insertLayout(1, field);
					qApp->processEvents();
				}
			} else {
				qDebug()<<"WorldView is already running.";
			}
		}

		void FieldViewer::stopDrawing(){
			if(keepDrawing){
				keepDrawing = false;
				qDebug()<<"WorldView stopped.";
				bot_locs->locs->stopListening();
			} else {
				qDebug()<<"WorldView isn't running.";
			}
		}
	}
}




