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

		FieldViewer::FieldViewer(DataManager::ptr dataManager):
			QMainWindow(),
			dataManager(dataManager),
			startButton(new QPushButton("Locate Robots", this)),
			stopButton(new QPushButton("Stop Location", this)) {

			fieldImage = new PaintField(this);
			bot_locs = new PaintBots(this);
			overlayView = new OverlayedImage(fieldImage, bot_locs, this);
			fieldView = new BMPImageViewer(fieldImage, this);

			drawButtons();

			fieldView->setMinimumHeight(FIELD_HEIGHT);
			this->setCentralWidget(fieldView);
		}

		void FieldViewer::drawBots(){
			if(!keepDrawing){
				bot_locs->locs->startListening();
				keepDrawing = true;
				while(keepDrawing){
					delete fieldView;
					fieldView = new BMPImageViewer(overlayView, this);
					fieldView->setMinimumHeight(FIELD_HEIGHT);
					this->setCentralWidget(fieldView);
					qApp->processEvents();
				}
			} else {
				qDebug()<<"WorldViewer is already running.";
			}
		}

		void FieldViewer::drawButtons(){
			dockWidget = new QDockWidget(tr(""), this);
			dockWidget->setAllowedAreas(Qt::RightDockWidgetArea);
			dockWidget->setFixedSize(250,100);

			buttonWidget = new QWidget();
			buttonLayout = new QVBoxLayout(buttonWidget);

			buttonLayout->addWidget(startButton);
			connect(startButton, SIGNAL(clicked()), this, SLOT(drawBots()));

			buttonLayout->addWidget(stopButton);
			connect(stopButton, SIGNAL(clicked()), this, SLOT(stopDrawing()));

			dockWidget->setWidget(buttonWidget);
			addDockWidget(Qt::RightDockWidgetArea, dockWidget);

			this->setCentralWidget(fieldView);
		}

		void FieldViewer::stopDrawing(){
			keepDrawing = false;
			qDebug()<<"WorldView stopped.";
			bot_locs->locs->stopListening();
		}
	}
}




