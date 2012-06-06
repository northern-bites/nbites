
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
    MImage::const_ptr rawMTopImage = memoryManager->getMemory()->
        getMImage(Camera::TOP);
    MImage::const_ptr rawMBottomImage = memoryManager->getMemory()->
        getMImage(Camera::BOTTOM);

    FastYUVToBMPImage* rawTopBMP = new
        FastYUVToBMPImage(rawMTopImage, this);
    FastYUVToBMPImage* rawBottomBMP = new
        FastYUVToBMPImage(rawMBottomImage, this);

    BMPImageViewer* topImageViewer;
    BMPImageViewer* bottomImageViewer;

    VisualInfoImage* shapes = new VisualInfoImage(memoryManager->getMemory()->getMVision(), 
						  Camera::BOTTOM);

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
    
    memoryManager->connectSlotToMObject(bottomImageViewer,
					SLOT(updateView()), MBOTTOMIMAGE_ID);
    
    memoryManager->connectSlotToMObject(topImageViewer,
					SLOT(updateView()), MTOPIMAGE_ID);

    //corner ownership
    this->setCorner(Qt::TopRightCorner, Qt::RightDockWidgetArea);
    this->setCorner(Qt::BottomRightCorner, Qt::RightDockWidgetArea);

    std::vector<QTreeView> messageViewers;
    for (MObject_ID id = FIRST_OBJECT_ID;
            id != LAST_OBJECT_ID; id++) {
        if (id != MTOPIMAGE_ID && id != MBOTTOMIMAGE_ID) {
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
