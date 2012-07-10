
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

    VisualInfoImage* shapesBottom = new VisualInfoImage(memoryManager->getMemory()->get<MVision>(),
														Camera::BOTTOM);
    VisualInfoImage* shapesTop = new VisualInfoImage(memoryManager->getMemory()->get<MVision>(),
													 Camera::TOP);

    OverlayedImage* comboBottom = new OverlayedImage(rawBottomBMP, shapesBottom, this);
    OverlayedImage* comboTop = new OverlayedImage(rawTopBMP, shapesTop, this);

	BMPImageViewer* bottomImageViewer = new BMPImageViewer(comboBottom, this);
    BMPImageViewer* topImageViewer = new BMPImageViewer(comboTop, this);

	BMPImageViewer* top = new BMPImageViewer(comboTop, this);
    BMPImageViewer* bottom = new BMPImageViewer(comboBottom, this);

	QHBoxLayout* mainLayout = new QHBoxLayout;
	QWidget* mainWidget = new QWidget;
    QTabWidget* imageTabs = new QTabWidget();

    imageTabs->addTab(topImageViewer, QString("Top Image"));
    imageTabs->addTab(bottomImageViewer, QString("Bottom Image"));

	mainLayout->addWidget(imageTabs);
	mainWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	mainWidget->setLayout(mainLayout);
    this->setCentralWidget(mainWidget);

    //TODO: we call updateView twice per vision frame
    memoryManager->connectSlot(bottomImageViewer, SLOT(updateView()), "MVision");
    memoryManager->connectSlot(topImageViewer, SLOT(updateView()), "MVision");

	//need another set of these to connect the BMPImageViewer copies (top, bottom)
    memoryManager->connectSlot(bottom, SLOT(updateView()), "MVision");
    memoryManager->connectSlot(top, SLOT(updateView()), "MVision");

    //corner ownership
    this->setCorner(Qt::TopRightCorner, Qt::RightDockWidgetArea);
    this->setCorner(Qt::BottomRightCorner, Qt::RightDockWidgetArea);

    Memory::const_ptr memory = memoryManager->getMemory();

    std::vector<QTreeView> messageViewers;
    for (Memory::const_iterator iterator = memory->begin(); iterator != memory->end(); iterator++) {

        if (iterator->first != "MRawImages" && iterator->first != "GroundTruth") {

            QDockWidget* dockWidget = new QDockWidget(QString(iterator->first.c_str()), this);
			dockWidget->setMinimumWidth(300);
			dockWidget->setMaximumHeight(125);
            MObjectViewer* view = new MObjectViewer(iterator->second, dockWidget);
            dockWidget->setWidget(view);
            this->addDockWidget(Qt::RightDockWidgetArea, dockWidget);
            memoryManager->connectSlot(view, SLOT(updateView()), iterator->first);
        }
    }

}
}
}
