
#include "MemoryViewer.h"
#include "Camera.h"
#include <vector>
#include "image/FastYUVToBMPImage.h"

namespace qtool {
namespace viewer {

using namespace data;
using namespace man::memory;
using namespace qtool::image;
using namespace man::corpus;

MemoryViewer::MemoryViewer(RobotMemoryManager::const_ptr memoryManager) :
                 memoryManager(memoryManager) {
    MImage::const_ptr rawMImage = memoryManager->getMemory()->
        getMImage(Camera::TOP);
    FastYUVToBMPImage* rawBMP = new FastYUVToBMPImage(rawMImage, this);


    BMPImageViewer* imageViewer;

      VisualInfoImage* shapes = new VisualInfoImage(memoryManager->getMemory()->getMVision());
      OverlayedImage* combo = new OverlayedImage(rawBMP, shapes, this);
    
      imageViewer = new BMPImageViewer(combo, this);

  
    this->setCentralWidget(imageViewer);
    memoryManager->connectSlotToMObject(imageViewer,
                        SLOT(updateView()), MIMAGE_ID);


    //corner ownership
    this->setCorner(Qt::TopRightCorner, Qt::RightDockWidgetArea);
    this->setCorner(Qt::BottomRightCorner, Qt::RightDockWidgetArea);

    std::vector<QTreeView> messageViewers;
    for (MObject_ID id = FIRST_OBJECT_ID;
            id != LAST_OBJECT_ID; id++) {
        if (id != MIMAGE_ID) {
            QDockWidget* dockWidget =
                    new QDockWidget(QString(MObject_names[id].c_str()), this);
            MObjectViewer* view = new MObjectViewer(
                    memoryManager->getMemory()->
                    getMObject(id)->getProtoMessage());
            dockWidget->setWidget(view);
            this->addDockWidget(Qt::RightDockWidgetArea, dockWidget);
            memoryManager->connectSlotToMObject(view, SLOT(updateView()), id);
        }
    }
   
}
}
}
