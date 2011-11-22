
#include "OfflineViewer.h"
#include "image/ThresholdedImage.h"

using namespace man::corpus;
using namespace man::memory;
using namespace qtool::viewer;
using namespace qtool::image;

namespace qtool {
namespace offline {

OfflineViewer::OfflineViewer(Memory::const_ptr memory, QWidget* parent) : QWidget(parent),
        offlineControl(new OfflineManController(memory)),
        manPreloader(offlineControl),
        manMemoryViewer(new MemoryViewer(offlineControl->getManMemory())) {

    memory->addSubscriber(this, MIMAGE_ID);

    QVBoxLayout *vertLayout = new QVBoxLayout;
    QHBoxLayout* buttonLayout = new QHBoxLayout;
    vertLayout->addLayout(buttonLayout);

    QPushButton* reloadManButton = new QPushButton(tr("&Reload Man"));
    connect(reloadManButton, SIGNAL(clicked()), this, SLOT(reloadMan()));
//    buttonLayout->addWidget(reloadManButton);

    QPushButton* loadTableButton = new QPushButton(tr("&Load Table"));
    connect(loadTableButton, SIGNAL(clicked()), this, SLOT(loadColorTable()));
    buttonLayout->addWidget(loadTableButton);

    vertLayout->addWidget(manMemoryViewer);
    //add the thresholded image to the memory viewer
    ThresholdedImage::ptr threshImage(new ThresholdedImage(
            offlineControl->getManMemory()->getMImage()->getThresholded()));
    QDockWidget* dockWidget =
            new QDockWidget(tr("Thresholded"), manMemoryViewer);
    RoboImageViewer* threshView = new RoboImageViewer(threshImage, dockWidget);
    offlineControl->getManMemory()->addSubscriber(threshView, MIMAGE_ID);
    dockWidget->setWidget(threshView);
    dockWidget->setMinimumSize(350, 300);
    manMemoryViewer->addDockWidget(Qt::BottomDockWidgetArea, dockWidget);

    this->setLayout(vertLayout);
}

void OfflineViewer::update() {
    offlineControl->signalNextImageFrame();
}

void OfflineViewer::loadColorTable() {
    QString colorTablePath = QFileDialog::getOpenFileName(this, tr("Open Color Table"),
                                                "../../data/tables",
                                                tr("Table Files (*.mtb)"));
    offlineControl->loadTable(colorTablePath.toStdString());
}

void OfflineViewer::reloadMan() {
    manPreloader.reloadMan();
}

}
}
