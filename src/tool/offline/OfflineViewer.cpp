
#include "OfflineViewer.h"
#include "Camera.h"
#include "image/ThresholdedImage.h"

namespace qtool {
namespace offline {

using namespace man::corpus;
using namespace man::memory;
using namespace viewer;
using namespace image;
using namespace data;


OfflineViewer::OfflineViewer(DataManager::const_ptr dataManager, QWidget* parent) :
        QWidget(parent),
        offlineControl(dataManager->getMemory()) {

    dataManager->connectSlot(this, SLOT(update()), "MRawImages");

    mainLayout = new QVBoxLayout;
    QHBoxLayout* buttonLayout = new QHBoxLayout;
    mainLayout->addLayout(buttonLayout);

    QPushButton* reloadManButton = new QPushButton(tr("&Reload Man"));
    connect(reloadManButton, SIGNAL(clicked()), this, SLOT(stopMan()));
//    buttonLayout->addWidget(reloadManButton);

    QPushButton* loadManButton = new QPushButton(tr("&Load Man"));
    connect(loadManButton, SIGNAL(clicked()), this, SLOT(startMan()));
    buttonLayout->addWidget(loadManButton);

    QPushButton* loadTableButton = new QPushButton(tr("&Load Table"));
    connect(loadTableButton, SIGNAL(clicked()), this, SLOT(loadColorTable()));
    buttonLayout->addWidget(loadTableButton);
    mainLayout->setAlignment(Qt::AlignTop);
    this->setLayout(mainLayout);
}

void OfflineViewer::update() {
    offlineControl.signalNextImageFrame();
}

void OfflineViewer::loadColorTable() {
  //  if (loaded) {
        QString colorTablePath = QFileDialog::getOpenFileName(this, tr("Open Color Table"),
                "../../data/tables",
                tr("Table Files (*.mtb)"));
        offlineControl.loadTable(colorTablePath.toStdString());
	//  }
}

void OfflineViewer::stopMan() {
    offlineControl.getMan()->stopSubThreads();
}

void OfflineViewer::startMan() {


    //TODO: to be revised and fixed

   manMemoryManager = RobotMemoryManager::ptr(
           new RobotMemoryManager(offlineControl.getMan()->memory));
   manMemoryViewer = new viewer::MemoryViewer(manMemoryManager);
   mainLayout->addWidget(manMemoryViewer);
   offlineControl.getMan()->startSubThreads();
   //add the thresholded image to the memory viewer
//    ThresholdedImage* threshImage = new ThresholdedImage(
//        offlineControl->getManMemory()->getMImage(Camera::TOP)->
//        getThresholded(), this);
   // manMemoryManager->connectSlotToMObject(threshImage,
           // SLOT(updateBitmap()), MTOPIMAGE_ID);

   // QDockWidget* dockWidget =
//            new QDockWidget(tr("Thresholded"), manMemoryViewer);
//    BMPImageViewer* threshView = new BMPImageViewer(threshImage, dockWidget);
//    dockWidget->setWidget(threshView);

//    dockWidget->setMinimumSize(350, 300);
//    manMemoryViewer->addDockWidget(Qt::BottomDockWidgetArea, dockWidget);
}

}
}
