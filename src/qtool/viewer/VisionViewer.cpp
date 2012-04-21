
#include "VisionViewer.h"
#include <vector>

namespace qtool {
namespace viewer {

using namespace data;
using boost::shared_ptr;
using namespace man::memory;
using namespace man::corpus;
using namespace qtool::image;

VisionViewer::VisionViewer(RobotMemoryManager::const_ptr memoryManager) :
                 memoryManager(memoryManager),
		 speech(new Speech()),
		 sensors(new Sensors(speech)),
		 rawImage(new proto::PImage()){

    memoryManager->getMemory()->getMVisionSensors()->copyTo(sensors);
    pose = shared_ptr<NaoPose> (new NaoPose(sensors));
    vision = shared_ptr<Vision> (new Vision(pose));
    offlineMVision = shared_ptr<MVision> (new MVision(vision));
    
    imageTranscribe = OfflineImageTranscriber::ptr (new OfflineImageTranscriber(sensors,
							 memoryManager->getMemory()->getMImage()));

    rawImage->set_width(AVERAGED_IMAGE_WIDTH);
    rawImage->set_height(AVERAGED_IMAGE_HEIGHT);

    QToolBar* toolBar = new QToolBar(this);
    QPushButton* loadTableButton = new QPushButton(tr("&Load Table"));
    connect(loadTableButton, SIGNAL(clicked()), this, SLOT(loadColorTable()));
    toolBar->addWidget(loadTableButton);
    this->addToolBar(toolBar);

    visionImage = new ThresholdedImage(rawImage, this);
    VisualInfoImage* shapes = new VisualInfoImage(offlineMVision);

    FastYUVToBMPImage* rawBMP = new FastYUVToBMPImage(memoryManager->getMemory()->getMImage(), this);
    OverlayedImage* combo = new OverlayedImage(rawBMP, shapes, this);
    
    BMPImageViewer *imageViewer = new BMPImageViewer(combo, this);
    BMPImageViewer *visionViewer = new BMPImageViewer(visionImage, this);

    QTabWidget* imageTabs = new QTabWidget();
    imageTabs->addTab(imageViewer, tr("Raw Image"));
    imageTabs->addTab(visionViewer, tr("Vision Image"));
    
    memoryManager->connectSlotToMObject(this, SLOT(update()), MIMAGE_ID);

    this->setCentralWidget(imageTabs);
    memoryManager->connectSlotToMObject(visionViewer,
					SLOT(updateView()), MIMAGE_ID);
    memoryManager->connectSlotToMObject(imageViewer,
					SLOT(updateView()), MIMAGE_ID);

    //corner ownership
    this->setCorner(Qt::TopRightCorner, Qt::RightDockWidgetArea);
    this->setCorner(Qt::BottomRightCorner, Qt::RightDockWidgetArea);

    std::vector<QTreeView> messageViewers; 
    for (MObject_ID id = FIRST_OBJECT_ID;
            id != LAST_OBJECT_ID; id++) {
        if (id == MVISION_ID) {
            QDockWidget* dockWidget = 
                   new QDockWidget("Offline Vision", this);
            MObjectViewer* view = new MObjectViewer(offlineMVision->getProtoMessage());
	    dockWidget->setWidget(view);
            this->addDockWidget(Qt::RightDockWidgetArea, dockWidget);
            memoryManager->connectSlotToMObject(view, SLOT(updateView()), id);
	}
        if (id != MIMAGE_ID && id != MVISION_ID) {
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

void VisionViewer::update(){
  imageTranscribe->acquireNewImage();
  sensors->updateVisionAngles();
  vision->notifyImage(sensors->getImage());
  offlineMVision->updateData();
  rawImage->mutable_image()->assign(reinterpret_cast<const char *>
				    (vision->thresh->thresholded),
				    AVERAGED_IMAGE_SIZE);

}

void VisionViewer::loadColorTable(){
  QString colorTablePath = QFileDialog::getOpenFileName(this, tr("Open Color Table"),
							"../../data/tables",
							tr("Table Files (*.mtb)"));
  imageTranscribe->initTable(colorTablePath.toStdString());

}


}
}
