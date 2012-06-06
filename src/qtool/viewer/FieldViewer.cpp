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

			mainLayout = new QHBoxLayout(this);

			//field image painted via overlay of robots, field
			fieldImage = new PaintField(this);
			bot_locs = new PaintBots(this);
			overlayView = new OverlayedImage(fieldImage, bot_locs, this);
			fieldView = new BMPImageViewer(fieldImage, this);

			field = new QVBoxLayout();
			field->addWidget(fieldView);

			buttonLayout = new QVBoxLayout();
			buttonLayout->setSpacing(10);
			buttonLayout->addWidget(startButton);
			connect(startButton, SIGNAL(clicked()), this, SLOT(drawBots()));
			buttonLayout->addWidget(stopButton);
			connect(stopButton, SIGNAL(clicked()), this, SLOT(stopDrawing()));
			QSpacerItem* spacer = new QSpacerItem(0, 0, QSizePolicy::Minimum,
												   QSizePolicy::Expanding);
			buttonLayout->addItem(spacer);

			//paint the field
			mainLayout->addLayout(field);
			mainLayout->addLayout(buttonLayout);
			this->setLayout(mainLayout);
		}

		void FieldViewer::drawBots(){
			if(!keepDrawing){
				bot_locs->locs->startListening();
				keepDrawing = true;
				while(keepDrawing){
					delete field;
					//delete fieldView;
					//fieldView = new BMPImageViewer(overlayView, this);
					fieldView->update();
					field = new QVBoxLayout();
					field->addWidget(fieldView);
					mainLayout->insertLayout(0, field);
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




