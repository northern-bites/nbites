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

			//spacers are for customized layout
			fieldSpacer = new QSpacerItem(0, 0, QSizePolicy::Minimum,
										  QSizePolicy::Expanding);
			buttonSpacer = new QSpacerItem(0, 0, QSizePolicy::Minimum,
										   QSizePolicy::Expanding);

			field = new QVBoxLayout();
			field->addWidget(fieldView);
			field->addItem(fieldSpacer);

			buttonLayout = new QVBoxLayout();
			buttonLayout->setSpacing(10);
			buttonLayout->addWidget(startButton);
			connect(startButton, SIGNAL(clicked()), this, SLOT(drawBots()));
			buttonLayout->addWidget(stopButton);
			connect(stopButton, SIGNAL(clicked()), this, SLOT(stopDrawing()));
			buttonLayout->addItem(buttonSpacer);

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
					delete fieldView;
					fieldView = new BMPImageViewer(overlayView, this);
					fieldView->getLayout()->setAlignment(Qt::Alignment());
					field = new QVBoxLayout();
					field->addWidget(fieldView);
					fieldSpacer = new QSpacerItem(0, 0, QSizePolicy::Minimum,
												  QSizePolicy::Expanding);
					field->addItem(fieldSpacer);
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




