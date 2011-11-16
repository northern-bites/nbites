
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

    QVBoxLayout *layout = new QVBoxLayout;
    layout->setAlignment(Qt::AlignTop);

    QPushButton* loadTableButton = new QPushButton(tr("&Load Table"));
    connect(loadTableButton, SIGNAL(clicked()), this, SLOT(loadColorTable()));
    layout->addWidget(loadTableButton);

    layout->addWidget(manMemoryViewer);

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

    this->setLayout(layout);
}

void OfflineViewer::update(data::MObject_ID id) {
    offlineControl->signalNextImageFrame();
}

void OfflineViewer::loadColorTable() {
    QString colorTablePath = QFileDialog::getOpenFileName(this, tr("Open Color Table"),
                                                "../../data/tables",
                                                tr("Table Files (*.mtb)"));
    offlineControl->loadTable(colorTablePath.toStdString());
}

}
}
