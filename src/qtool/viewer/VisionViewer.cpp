
#include "VisionViewer.h"
#include "Camera.h"
#include "ClassHelper.h"
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
                 bottomRawImage(new proto::PRawImage()),
                 topRawImage(new proto::PRawImage())
{

    offlineMVision = shared_ptr<MVision> (new MVision(class_name<MVision>()));

    pose = shared_ptr<NaoPose> (new NaoPose(sensors));
    vision = shared_ptr<Vision> (new Vision(pose));

    imageTranscribe = OfflineImageTranscriber::ptr
        (new OfflineImageTranscriber(sensors,
                                     memoryManager->getMemory()->
                                     get<MRawImages>()));

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

    VisualInfoImage* shapes = new VisualInfoImage(offlineMVision);

    MRawImages::const_ptr rawImages = memoryManager->getMemory()->get<MRawImages>();

    FastYUVToBMPImage* rawTopBMP = new FastYUVToBMPImage(rawImages, Camera::TOP, this);
    FastYUVToBMPImage* rawBottomBMP = new FastYUVToBMPImage(rawImages, Camera::BOTTOM, this);


    OverlayedImage* combo = new OverlayedImage(rawBottomBMP, shapes, this);

    BMPImageViewer *bottomImageViewer = new BMPImageViewer(combo, this);
    BMPImageViewer *topImageViewer = new BMPImageViewer(rawTopBMP, this);

    CollapsibleImageViewer* bottomCIV = new
        CollapsibleImageViewer(bottomImageViewer,
                               "Bottom",
                               this);
    CollapsibleImageViewer* topCIV = new CollapsibleImageViewer(topImageViewer,
                                                                "Top",
                                                                this);

    QWidget* combinedRawImageView = new QWidget(this);

    QVBoxLayout* layout = new QVBoxLayout(combinedRawImageView);

    layout->addWidget(topCIV);
    layout->addWidget(bottomCIV);

    combinedRawImageView->setLayout(layout);

    bottomVisionView = new BMPImageViewerListener(bottomVisionImage,
                                                       this);
    connect(bottomVisionView, SIGNAL(mouseClicked(int, int, int, bool)),
            this, SLOT(pixelClicked(int, int, int, bool)));
    topVisionView = new BMPImageViewer(topVisionImage,
                                                      this);

    CollapsibleImageViewer* bottomVisCIV = new CollapsibleImageViewer(bottomVisionView, "Bottom", this);
    CollapsibleImageViewer* topVisCIV = new CollapsibleImageViewer(topVisionView, "Top", this);

    QWidget* visionImages = new QWidget(this);

    QVBoxLayout* visLayout = new QVBoxLayout(visionImages);

    visLayout->addWidget(topVisCIV);
    visLayout->addWidget(bottomVisCIV);

    visionImages->setLayout(visLayout);

    QTabWidget* imageTabs = new QTabWidget();
    imageTabs->addTab(combinedRawImageView, tr("Raw Images"));
    imageTabs->addTab(visionImages, tr("Vision Images"));

    memoryManager->connectSlot(this, SLOT(update()), "MRawImages");

    this->setCentralWidget(imageTabs);

    memoryManager->connectSlot(bottomImageViewer, SLOT(updateView()), "MRawImages");
    memoryManager->connectSlot(topImageViewer, SLOT(updateView()), "MRawImages");

    //corner ownership
    this->setCorner(Qt::TopRightCorner, Qt::RightDockWidgetArea);
    this->setCorner(Qt::BottomRightCorner, Qt::RightDockWidgetArea);

    Memory::const_ptr memory = memoryManager->getMemory();

    std::vector<QTreeView> messageViewers;

    QDockWidget* dockWidget = new QDockWidget("Offline Vision", this);
    offlineVisionView = new MObjectViewer(offlineMVision->getProtoMessage());
	dockWidget->setWidget(offlineVisionView);
    this->addDockWidget(Qt::RightDockWidgetArea, dockWidget);

    // Make sure one of the images is toggled off for small screens
    bottomCIV->toggle();
}

void VisionViewer::update(){

    //no useless computation
    if (!this->isVisible())
        return;

    imageTranscribe->acquireNewImage();
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

    offlineVisionView->updateView();
    topVisionView->updateView();
    bottomVisionView->updateView();
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
