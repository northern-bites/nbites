
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
    MImage::const_ptr rawMTopImage = memoryManager->getMemory()->get<MTopImage>();
    MImage::const_ptr rawMBottomImage = memoryManager->getMemory()->get<MBottomImage>();

    FastYUVToBMPImage* rawTopBMP = new
        FastYUVToBMPImage(rawMTopImage, this);
    FastYUVToBMPImage* rawBottomBMP = new
        FastYUVToBMPImage(rawMBottomImage, this);

    BMPImageViewer* topImageViewer;
    BMPImageViewer* bottomImageViewer;

      VisualInfoImage* shapes = new VisualInfoImage(memoryManager->getMemory()->get<MVision>());

      OverlayedImage* combo = new OverlayedImage(rawBottomBMP,
                                                 shapes, this);

      bottomImageViewer = new BMPImageViewer(combo, this);
      CollapsibleImageViewer * bottomCIV = new
          CollapsibleImageViewer(bottomImageViewer,
                                 QString("Bottom"),
                                 this);

      topImageViewer = new BMPImageViewer(rawTopBMP, this);
      CollapsibleImageViewer * topCIV = new
          CollapsibleImageViewer(topImageViewer,
                                 QString("Top"),
                                 this);

      //}

      //    else
      // imageViewer = new BMPImageViewer(rawBMP, this);

      QWidget* central = new QWidget(this);

      QVBoxLayout* layout = new QVBoxLayout(central);

      layout->addWidget(topCIV);
      layout->addWidget(bottomCIV);

      // Make sure one of the images is toggled off for small screens
      bottomCIV->toggle();

      central->setLayout(layout);

      this->setCentralWidget(central);

    memoryManager->connectSlot(bottomImageViewer,
                        SLOT(updateView()), "MBottomImage");

    memoryManager->connectSlot(topImageViewer,
                        SLOT(updateView()), "MTopImage");

    //corner ownership
    this->setCorner(Qt::TopRightCorner, Qt::RightDockWidgetArea);
    this->setCorner(Qt::BottomRightCorner, Qt::RightDockWidgetArea);

    Memory::const_ptr memory = memoryManager->getMemory();

    std::vector<QTreeView> messageViewers;
    for (Memory::const_iterator iterator = memory->begin(); iterator != memory->end(); iterator++) {

        if (iterator->second != memory->get<MTopImage>() &&
            iterator->second != memory->get<MBottomImage>()) {

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
