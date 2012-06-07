
#include "VisionViewer.h"
#include "Camera.h"
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
                 bottomRawImage(new proto::PImage()),
                 topRawImage(new proto::PImage())
{

    memoryManager->getMemory()->getMVisionSensors()->copyTo(sensors);
    pose = shared_ptr<NaoPose> (new NaoPose(sensors));
    vision = shared_ptr<Vision> (new Vision(pose));
    offlineMVision = shared_ptr<MVision> (new MVision(vision));

    imageTranscribe = OfflineImageTranscriber::ptr
        (new OfflineImageTranscriber(sensors,
                                     memoryManager->getMemory()->
                                     getMImage(Camera::TOP),
                                     memoryManager->getMemory()->
                                     getMImage(Camera::BOTTOM)));

    bottomRawImage->set_width(AVERAGED_IMAGE_WIDTH);
    bottomRawImage->set_height(AVERAGED_IMAGE_HEIGHT);
    topRawImage->set_width(AVERAGED_IMAGE_WIDTH);
    topRawImage->set_height(AVERAGED_IMAGE_HEIGHT);

    QToolBar* toolBar = new QToolBar(this);
    QPushButton* loadTableButton = new QPushButton(tr("&Load Table"));
    connect(loadTableButton, SIGNAL(clicked()), this, SLOT(loadColorTable()));
    toolBar->addWidget(loadTableButton);
    this->addToolBar(toolBar);

    QCheckBox* horizonDebug = new QCheckBox(tr("Horizon Debug"));
    connect(horizonDebug, SIGNAL(clicked()), this, SLOT(setHorizonDebug()));
    toolBar->addWidget(horizonDebug);
    QCheckBox* shootingDebug = new QCheckBox(tr("Shooting Debug"));
    connect(horizonDebug, SIGNAL(clicked()), this, SLOT(setShootingDebug()));
    toolBar->addWidget(shootingDebug);
    QCheckBox* openFieldDebug = new QCheckBox(tr("Open Field Debug"));
    connect(horizonDebug, SIGNAL(clicked()), this, SLOT(setOpenFieldDebug()));
    toolBar->addWidget(openFieldDebug);
    QCheckBox* EdgeDetectionDebug = new QCheckBox(tr("Edge Detection Debug"));
    connect(horizonDebug, SIGNAL(clicked()), this, SLOT(setEdgeDetectDebug()));
    toolBar->addWidget(EdgeDetectionDebug);
    QCheckBox* houghDebug = new QCheckBox(tr("Hough Debug"));
    connect(horizonDebug, SIGNAL(clicked()), this, SLOT(setHoughDebug()));
    toolBar->addWidget(houghDebug);
    QCheckBox* robotsDebug = new QCheckBox(tr("Robots Debug"));
    connect(horizonDebug, SIGNAL(clicked()), this, SLOT(setRobotsDebug()));
    toolBar->addWidget(robotsDebug);

    horizonD = false;
    shootD = false;
    openFieldD = false;
    edgeDetectD = false;
    houghD = false;
    robotsD = false;


    bottomVisionImage = new ThresholdedImage(bottomRawImage, this);
    topVisionImage = new ThresholdedImage(topRawImage, this);

    VisualInfoImage* shapesBottom = new VisualInfoImage(offlineMVision, Camera::BOTTOM);
    VisualInfoImage* shapesTop = new VisualInfoImage(offlineMVision, Camera::TOP);    

    FastYUVToBMPImage* rawBottomBMP = new FastYUVToBMPImage(memoryManager->
                                                      getMemory()->
                                                  getMImage(Camera::BOTTOM),
                                                      this);
    FastYUVToBMPImage* rawTopBMP = new FastYUVToBMPImage(memoryManager->
                                                         getMemory()->
                                                     getMImage(Camera::TOP),
                                                         this);

    OverlayedImage* comboBottom = new OverlayedImage(rawBottomBMP, shapesBottom, this);
    OverlayedImage* comboTop = new OverlayedImage(rawTopBMP, shapesTop, this);

    BMPImageViewer *bottomImageViewer = new BMPImageViewer(comboBottom, this);
    BMPImageViewer *topImageViewer = new BMPImageViewer(comboTop, this);

    CollapsibleImageViewer* bottomCIV = new
        CollapsibleImageViewer(bottomImageViewer,
                               "Bottom",
                               this);
    CollapsibleImageViewer* topCIV = new CollapsibleImageViewer(topImageViewer,
                                                                "Top",
                                                                this);

    QWidget* rawImages = new QWidget(this);

    QVBoxLayout* layout = new QVBoxLayout(rawImages);

    layout->addWidget(topCIV);
    layout->addWidget(bottomCIV);

    rawImages->setLayout(layout);

    BMPImageViewer *bottomVisViewer = new BMPImageViewerListener(bottomVisionImage,
                                                       this);
    connect(bottomVisViewer, SIGNAL(mouseClicked(int, int, int, bool)),
            this, SLOT(pixelClicked(int, int, int, bool)));
    BMPImageViewer *topVisViewer = new BMPImageViewer(topVisionImage,
                                                      this);

    CollapsibleImageViewer* bottomVisCIV = new
        CollapsibleImageViewer(bottomVisViewer,
                               "Bottom",
                               this);
    CollapsibleImageViewer* topVisCIV = new CollapsibleImageViewer(topVisViewer,
                                                                   "Top",
                                                                   this);

    QWidget* visionImages = new QWidget(this);

    QVBoxLayout* visLayout = new QVBoxLayout(visionImages);

    visLayout->addWidget(topVisCIV);
    visLayout->addWidget(bottomVisCIV);

    visionImages->setLayout(visLayout);

    QTabWidget* imageTabs = new QTabWidget();
    imageTabs->addTab(rawImages, tr("Raw Images"));
    imageTabs->addTab(visionImages, tr("Vision Images"));

    memoryManager->connectSlotToMObject(this, SLOT(update()),
                                        MBOTTOMIMAGE_ID);
    memoryManager->connectSlotToMObject(this, SLOT(update()),
                                        MTOPIMAGE_ID);

    this->setCentralWidget(imageTabs);
    memoryManager->connectSlotToMObject(bottomVisViewer,
					SLOT(updateView()), MBOTTOMIMAGE_ID);
    memoryManager->connectSlotToMObject(topVisViewer,
					SLOT(updateView()), MTOPIMAGE_ID);

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
        if (id == MVISION_ID) {
            QDockWidget* dockWidget = 
                   new QDockWidget("Offline Vision", this);
            MObjectViewer* view = new MObjectViewer(offlineMVision->getProtoMessage());
	    dockWidget->setWidget(view);
            this->addDockWidget(Qt::RightDockWidgetArea, dockWidget);
            memoryManager->connectSlotToMObject(view, SLOT(updateView()), id);
	}
        if (id != MTOPIMAGE_ID && id != MBOTTOMIMAGE_ID &&
            id != MVISION_ID) {
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

    // Make sure one of the images is toggled off for small screens
    bottomCIV->toggle();
}

void VisionViewer::update(){

    //no useless computation
    if (!this->isVisible())
        return;

    imageTranscribe->acquireNewImage();

    // update the vision body angles
    MImage::const_ptr mImage = memoryManager->getMemory()->getMImage(Camera::BOTTOM);
    std::vector<float> body_angles(mImage->get()->vision_body_angles().begin(),
                                   mImage->get()->vision_body_angles().end());

    if (body_angles.empty())
      sensors->updateVisionAngles();
    else
      sensors->setVisionBodyAngles(body_angles);

    vision->notifyImage(sensors->getImage(Camera::BOTTOM));
    offlineMVision->updateData();
    // Will need to get these to be diffent thresholded images but vision
    // appears to only threhold one at the moment!
    bottomRawImage->mutable_image()->assign(reinterpret_cast<const char *>
                                            (vision->thresh->thresholded),
                                            AVERAGED_IMAGE_SIZE);
    topRawImage->mutable_image()->assign(reinterpret_cast<const char *>
                                         (vision->thresh->thresholded),
                                         AVERAGED_IMAGE_SIZE);
}

void VisionViewer::pixelClicked(int x, int y, int brushSize, bool leftClick) {

    estimate pixEst = vision->pose->pixEstimate(x, y, 0.0f);
    std::cout << "x: " << x << "   y: " << y << std::endl;
    std::cout << pixEst << std::endl;
}

void VisionViewer::loadColorTable(){
  QString colorTablePath = QFileDialog::getOpenFileName(this, tr("Open Color Table"),
							"../../data/tables",
							tr("Table Files (*.mtb)"));
  imageTranscribe->initTable(colorTablePath.toStdString());

}

void VisionViewer::setHorizonDebug(){
  if (horizonD == false) horizonD = true;
  else horizonD = false;
  vision->thresh->setHorizonDebug(horizonD);
}
void VisionViewer::setShootingDebug(){
  if (shootD == false) shootD = true;
  else shootD = false;
  vision->thresh->setDebugShooting(shootD);
}
void VisionViewer::setOpenFieldDebug(){
  if (openFieldD == false) openFieldD = true;
  else openFieldD = false;
  vision->thresh->setDebugOpenField(openFieldD);
}
void VisionViewer::setEdgeDetectDebug(){
  if (edgeDetectD == false) edgeDetectD = true;
  else edgeDetectD = false;
  vision->thresh->setDebugEdgeDetection(edgeDetectD);
}
void VisionViewer::setHoughDebug(){
  if (houghD == false) houghD = true;
  else houghD = false;
  vision->thresh->setDebugHoughTransform(houghD);
}
void VisionViewer::setRobotsDebug(){
  if (robotsD == false) robotsD = true;
  else robotsD = false;
  vision->thresh->setDebugRobots(robotsD);
}


}
}
