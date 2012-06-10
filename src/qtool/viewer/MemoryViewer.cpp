
#include "MemoryViewer.h"
#include "Camera.h"
#include <vector>
#include "image/FastYUVToBMPImage.h"
#include "CollapsibleImageViewer.h"

namespace qtool {
namespace viewer {

using namespace data;
using namespace man::memory;
using namespace qtool::image;
using namespace man::corpus;

MemoryViewer::MemoryViewer(RobotMemoryManager::const_ptr memoryManager) :
                 memoryManager(memoryManager) {

    MRawImages::const_ptr rawImages = memoryManager->getMemory()->get<MRawImages>();

    FastYUVToBMPImage* rawTopBMP = new FastYUVToBMPImage(rawImages, Camera::TOP, this);
    FastYUVToBMPImage* rawBottomBMP = new FastYUVToBMPImage(rawImages, Camera::BOTTOM, this);

    BMPImageViewer* topImageViewer;
    BMPImageViewer* bottomImageViewer;

    VisualInfoImage* shapesBottom = new VisualInfoImage(memoryManager->getMemory()->get<MVision>(),
            Camera::BOTTOM);
    VisualInfoImage* shapesTop = new VisualInfoImage(memoryManager->getMemory()->get<MVision>(),
            Camera::TOP);

    OverlayedImage* comboBottom = new OverlayedImage(rawBottomBMP, shapesBottom, this);
    OverlayedImage* comboTop = new OverlayedImage(rawTopBMP, shapesTop, this);

    bottomImageViewer = new BMPImageViewer(comboBottom, this);
    CollapsibleImageViewer * bottomCIV = new
            CollapsibleImageViewer(bottomImageViewer,
                    QString("Bottom"),
                    this);

    topImageViewer = new BMPImageViewer(comboTop, this);
    CollapsibleImageViewer * topCIV = new
            CollapsibleImageViewer(topImageViewer,
                    QString("Top"),
                    this);

    QWidget* central = new QWidget(this);

    QVBoxLayout* layout = new QVBoxLayout(central);

    layout->addWidget(topCIV);
    layout->addWidget(bottomCIV);

    // Make sure one of the images is toggled off for small screens
    bottomCIV->toggle();

    central->setLayout(layout);

    this->setCentralWidget(central);

    memoryManager->connectSlot(bottomImageViewer,
            SLOT(updateView()), "MRawImages");

    memoryManager->connectSlot(topImageViewer,
                        SLOT(updateView()), "MRawImages");

    //corner ownership
    this->setCorner(Qt::TopRightCorner, Qt::RightDockWidgetArea);
    this->setCorner(Qt::BottomRightCorner, Qt::RightDockWidgetArea);

    Memory::const_ptr memory = memoryManager->getMemory();

    std::vector<QTreeView> messageViewers;
    for (Memory::const_iterator iterator = memory->begin(); iterator != memory->end(); iterator++) {

        if (iterator->first != "MRawImages" && iterator->first != "GroundTruth") {

            QDockWidget* dockWidget = new QDockWidget(QString(iterator->first.c_str()), this);
            MObjectViewer* view = new MObjectViewer(iterator->second->getProtoMessage());
            dockWidget->setWidget(view);
            this->addDockWidget(Qt::RightDockWidgetArea, dockWidget);
            memoryManager->connectSlot(view, SLOT(updateView()), iterator->first);
        }
    }

}
}
}
