
#include "OfflineViewer.h"
#include "image/ThresholdedImage.h"

namespace qtool {
namespace offline {

using namespace man::corpus;
using namespace man::memory;
using namespace viewer;
using namespace image;
using namespace data;


OfflineViewer::OfflineViewer(Memory::const_ptr memory, QWidget* parent) :
        QWidget(parent),
        offlineControl(new OfflineManController(memory)),
        manPreloader(offlineControl),
        loaded(false) {

    memory->subscribe(this, MIMAGE_ID);

    mainLayout = new QVBoxLayout;
    QHBoxLayout* buttonLayout = new QHBoxLayout;
    mainLayout->addLayout(buttonLayout);

    QPushButton* reloadManButton = new QPushButton(tr("&Reload Man"));
    connect(reloadManButton, SIGNAL(clicked()), this, SLOT(reloadMan()));
//    buttonLayout->addWidget(reloadManButton);

    QPushButton* loadManButton = new QPushButton(tr("&Load Man"));
    connect(loadManButton, SIGNAL(clicked()), this, SLOT(loadMan()));
    buttonLayout->addWidget(loadManButton);

    QPushButton* loadTableButton = new QPushButton(tr("&Load Table"));
    connect(loadTableButton, SIGNAL(clicked()), this, SLOT(loadColorTable()));
    buttonLayout->addWidget(loadTableButton);

    mainLayout->setAlignment(Qt::AlignTop);

    this->setLayout(mainLayout);
}

void OfflineViewer::update() {
    if (loaded) {
        offlineControl->signalNextImageFrame();
    }
}

void OfflineViewer::loadColorTable() {
    if (loaded) {
        QString colorTablePath = QFileDialog::getOpenFileName(this, tr("Open Color Table"),
                "../../data/tables",
                tr("Table Files (*.mtb)"));
        offlineControl->loadTable(colorTablePath.toStdString());
    }
}

void OfflineViewer::reloadMan() {
    manPreloader.reloadMan();
}

void OfflineViewer::loadMan() {

    manPreloader.createMan();
    manMemoryManager = RobotMemoryManager::ptr(
            new RobotMemoryManager(offlineControl->getManMemory()));
    manMemoryViewer = new viewer::MemoryViewer(manMemoryManager);
    mainLayout->addWidget(manMemoryViewer);
    //add the thresholded image to the memory viewer
    ThresholdedImage::ptr threshImage(new ThresholdedImage(
            offlineControl->getManMemory()->getMImage()->getThresholded()));
    manMemoryManager->connectSlotToMObject(threshImage.get(),
            SLOT(updateBitmap()), MIMAGE_ID);

    QDockWidget* dockWidget =
            new QDockWidget(tr("Thresholded"), manMemoryViewer);
    BMPImageViewer* threshView = new BMPImageViewer(threshImage, dockWidget);
    dockWidget->setWidget(threshView);

    dockWidget->setMinimumSize(350, 300);
    manMemoryViewer->addDockWidget(Qt::BottomDockWidgetArea, dockWidget);

    loaded = true;
}

}
}
