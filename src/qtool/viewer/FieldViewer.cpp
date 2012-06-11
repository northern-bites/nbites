#include "FieldViewer.h"
#include "man/memory/Memory.h"
#include "image/BMPImage.h"

#include <QDebug>
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

	/*float scaleX = parent->size().width()/(float)FIELD_WIDTH;
	  float scaleY = parent->size().height()/(float)FIELD_HEIGHT;
	  qDebug()<<parent->size();
	  if(scaleX<scaleY)
	  scaleFactor = scaleX;
	  else
	  scaleFactor = scaleY;*/
    scaleFactor = 1.35f;

    //field image painted via overlay of robots, field
    fieldImage = new PaintField(this, scaleFactor);
    bot_locs = new PaintBots(scaleFactor, this);

    overlayView = new OverlayedImage(fieldImage, bot_locs, this);
    fieldView = new BMPImageViewer(overlayView, this);

    connect(bot_locs->locs, SIGNAL(newRobotLocation()), fieldView, SLOT(updateView()));

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

FieldViewer::~FieldViewer(){
	bot_locs->locs->stopListening();
}

void FieldViewer::drawBots(){
    bot_locs->locs->startListening();
}

void FieldViewer::stopDrawing(){
    bot_locs->locs->stopListening();
}

}
}





