
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

    offlineMVision = MVision::ptr(new MVision());

    pose = shared_ptr<NaoPose> (new NaoPose(sensors));
    vision = shared_ptr<Vision> (new Vision(pose, offlineMVision));

    imageTranscribe = OfflineImageTranscriber::ptr
        (new OfflineImageTranscriber(sensors,
                                     memoryManager->getMemory()->
                                     get<MRawImages>()));

    bottomRawImage->set_width(AVERAGED_IMAGE_WIDTH);
    bottomRawImage->set_height(AVERAGED_IMAGE_HEIGHT);
    topRawImage->set_width(AVERAGED_IMAGE_WIDTH);
    topRawImage->set_height(AVERAGED_IMAGE_HEIGHT);

    bottomRawImage->mutable_image()->assign(AVERAGED_IMAGE_SIZE, 0);
    topRawImage->mutable_image()->assign(AVERAGED_IMAGE_SIZE, 0);

    QToolBar* toolBar = new QToolBar(this);
    QPushButton* loadTableButton = new QPushButton(tr("&Load Table"));
    connect(loadTableButton, SIGNAL(clicked()), this, SLOT(loadColorTable()));
    toolBar->addWidget(loadTableButton);
    this->addToolBar(toolBar);


    // QCheckBox* horizonDebug = new QCheckBox(tr("Horizon"));
    // connect(horizonDebug, SIGNAL(clicked()), this, SLOT(setHorizonDebug()));
    // toolBar->addWidget(horizonDebug);
    // QCheckBox* shootingDebug = new QCheckBox(tr("Shooting"));
    // connect(shootingDebug, SIGNAL(clicked()), this, SLOT(setShootingDebug()));
    // toolBar->addWidget(shootingDebug);
    // QCheckBox* openFieldDebug = new QCheckBox(tr("Open Field"));
    // connect(openFieldDebug, SIGNAL(clicked()), this, SLOT(setOpenFieldDebug()));
    // toolBar->addWidget(openFieldDebug);
    // QCheckBox* EdgeDetectionDebug = new QCheckBox(tr("Edge Detection"));
    // connect(EdgeDetectionDebug, SIGNAL(clicked()), this, SLOT(setEdgeDetectDebug()));
    // toolBar->addWidget(EdgeDetectionDebug);
    // QCheckBox* houghDebug = new QCheckBox(tr("Hough"));
    // connect(houghDebug, SIGNAL(clicked()), this, SLOT(setHoughDebug()));
    // toolBar->addWidget(houghDebug);
    // QCheckBox* robotsDebug = new QCheckBox(tr("Robots"));
    // connect(robotsDebug, SIGNAL(clicked()), this, SLOT(setRobotsDebug()));
    // toolBar->addWidget(robotsDebug);
    // QCheckBox* ballDebug = new QCheckBox(tr("Ball"));
    // connect(ballDebug, SIGNAL(clicked()), this, SLOT(setBallDebug()));
    // toolBar->addWidget(ballDebug);
    // QCheckBox* ballDistanceDebug = new QCheckBox(tr("Ball Distance"));
    // connect(ballDistanceDebug, SIGNAL(clicked()), this, SLOT(setBallDistDebug()));
    // toolBar->addWidget(ballDistanceDebug);
    // QCheckBox* crossDebug = new QCheckBox(tr("Cross"));
    // connect(crossDebug, SIGNAL(clicked()), this, SLOT(setCrossDebug()));
    // toolBar->addWidget(crossDebug);
    // QCheckBox* identCornersDebug = new QCheckBox(tr("Identify Corners"));
    // connect(identCornersDebug, SIGNAL(clicked()), this, SLOT(setIdentCornersDebug()));
    // toolBar->addWidget(identCornersDebug);
    // QCheckBox* dangerousBallDebug = new QCheckBox(tr("Dangerous Ball"));
    // connect(dangerousBallDebug, SIGNAL(clicked()), this, SLOT(setDangerousBallDebug()));
    // toolBar->addWidget(dangerousBallDebug);
    // QCheckBox* fieldEdgeDebug = new QCheckBox(tr("Field Edge"));
    // connect(fieldEdgeDebug, SIGNAL(clicked()), this, SLOT(setFieldEdgeDebug()));
    // toolBar->addWidget(fieldEdgeDebug);

    // QCheckBox* postPrintDebug = new QCheckBox(tr("Post Print"));
    // connect(postPrintDebug, SIGNAL(clicked()), this, SLOT(setPostPrintDebug()));
    // toolBar->addWidget(postPrintDebug);
    // QCheckBox* postDebug = new QCheckBox(tr("Post"));
    // connect(postDebug, SIGNAL(clicked()), this, SLOT(setPostDebug()));
    // toolBar->addWidget(postDebug);
    // QCheckBox* postLogicDebug = new QCheckBox(tr("Post Logic"));
    // connect(postDebug, SIGNAL(clicked()), this, SLOT(setPostLogicDebug()));
    // toolBar->addWidget(postLogicDebug);
    // QCheckBox* postCorrectDebug = new QCheckBox(tr("Post Correct"));
    // connect(postCorrectDebug, SIGNAL(clicked()), this, SLOT(setPostCorrectDebug()));
    // toolBar->addWidget(postCorrectDebug);
    // QCheckBox* postSanityDebug = new QCheckBox(tr("Post Sanity"));
    // connect(postSanityDebug, SIGNAL(clicked()), this, SLOT(setPostSanityDebug()));
    // toolBar->addWidget(postSanityDebug);
 

    // horizonD = shootD = openFieldD = edgeDetectD = houghD = robotsD = false;
    // ballD = ballDistD = crossD = identCornersD = dangerousBallD = fieldEdgeD = false;
    // postPrintD = postD = postLogicD = postCorrectD = postSanityD = false;
    // vertEdgeD = horEdgeD = secVertD = createLD = fitPointD = joinLD = false;
    // intersectLD = CcScanD = riskyCornerD = cornObjDistD = false;

#define ADD_DEBUG_CHECKBOX(text, func) {            \
        QCheckBox* debug = new QCheckBox(tr(text)); \
        connect(debug, SIGNAL(stateChanged(int)),   \
                this, SLOT(func(int)));             \
        toolBar->addWidget(debug);                  \
    }

    ADD_DEBUG_CHECKBOX("Horizon Debug", setHorizonDebug);
    ADD_DEBUG_CHECKBOX("Shooting Debug", setShootingDebug);
    ADD_DEBUG_CHECKBOX("Open Field Debug", setOpenFieldDebug);
    ADD_DEBUG_CHECKBOX("Edge Detection Debug", setEdgeDetectionDebug);
    ADD_DEBUG_CHECKBOX("Hough Debug", setHoughTransformDebug);
    ADD_DEBUG_CHECKBOX("Robot Detection Debug", setRobotsDebug);
    ADD_DEBUG_CHECKBOX("Visual Line Debug", setVisualLinesDebug);
    ADD_DEBUG_CHECKBOX("Visual Corner Debug", setVisualCornersDebug);

    bottomVisionImage = new ThresholdedImage(bottomRawImage, this);
    topVisionImage = new ThresholdedImage(topRawImage, this);

    VisualInfoImage* shapesBottom = new VisualInfoImage(offlineMVision, Camera::BOTTOM);
    VisualInfoImage* shapesTop = new VisualInfoImage(offlineMVision, Camera::TOP);    

    MRawImages::const_ptr rawImages = memoryManager->getMemory()->get<MRawImages>();

    FastYUVToBMPImage* rawTopBMP = new FastYUVToBMPImage(rawImages, Camera::TOP, this);
    FastYUVToBMPImage* rawBottomBMP = new FastYUVToBMPImage(rawImages, Camera::BOTTOM, this);

    OverlayedImage* comboBottom = new OverlayedImage(rawBottomBMP, shapesBottom, this);
    OverlayedImage* comboTop = new OverlayedImage(rawTopBMP, shapesTop, this);

    BMPImageViewer *bottomImageViewer = new BMPImageViewer(comboBottom, this);
    BMPImageViewer *topImageViewer = new BMPImageViewer(comboTop, this);

    connect(this, SIGNAL(imagesUpdated()), bottomImageViewer, SLOT(updateView()));
    connect(this, SIGNAL(imagesUpdated()), topImageViewer, SLOT(updateView()));

    memoryManager->connectSlot(bottomImageViewer, SLOT(updateView()), "MRawImages");
    memoryManager->connectSlot(topImageViewer, SLOT(updateView()), "MRawImages");

    CollapsibleImageViewer* bottomCIV = new
            CollapsibleImageViewer(bottomImageViewer, "Bottom", this);
    CollapsibleImageViewer* topCIV = new
            CollapsibleImageViewer(topImageViewer, "Top", this);

    QWidget* combinedRawImageView = new QWidget(this);

    QVBoxLayout* layout = new QVBoxLayout(combinedRawImageView);
    layout->setAlignment(Qt::AlignTop);

    layout->addWidget(topCIV);
    layout->addWidget(bottomCIV);

    combinedRawImageView->setLayout(layout);

    bottomVisionView = new BMPImageViewerListener(bottomVisionImage, this);
    connect(bottomVisionView, SIGNAL(mouseClicked(int, int, int, bool)),
            this, SLOT(pixelClicked(int, int, int, bool)));

    connect(this, SIGNAL(imagesUpdated()),
            bottomVisionView, SLOT(updateView()));

    topVisionView = new BMPImageViewer(topVisionImage, this);
    connect(this, SIGNAL(imagesUpdated()),
            topVisionView, SLOT(updateView()));

    CollapsibleImageViewer* bottomVisCIV = new CollapsibleImageViewer(bottomVisionView, "Bottom", this);
    CollapsibleImageViewer* topVisCIV = new CollapsibleImageViewer(topVisionView, "Top", this);

    QWidget* visionImages = new QWidget(this);

    QVBoxLayout* visLayout = new QVBoxLayout(visionImages);
    visLayout->setAlignment(Qt::AlignTop);

    visLayout->addWidget(topVisCIV);
    visLayout->addWidget(bottomVisCIV);

    visionImages->setLayout(visLayout);

    QTabWidget* imageTabs = new QTabWidget();
    imageTabs->addTab(combinedRawImageView, tr("Raw Images"));
    imageTabs->addTab(visionImages, tr("Vision Images"));

    memoryManager->connectSlot(this, SLOT(update()), "MRawImages");

    this->setCentralWidget(imageTabs);

    //corner ownership
    this->setCorner(Qt::TopRightCorner, Qt::RightDockWidgetArea);
    this->setCorner(Qt::BottomRightCorner, Qt::RightDockWidgetArea);

    Memory::const_ptr memory = memoryManager->getMemory();

    std::vector<QTreeView> messageViewers;

    QDockWidget* dockWidget = new QDockWidget("Offline Vision", this);
    offlineVisionView = new MObjectViewer(offlineMVision, this);
	dockWidget->setWidget(offlineVisionView);
	connect(this, SIGNAL(imagesUpdated()), offlineVisionView, SLOT(updateView()));
    this->addDockWidget(Qt::RightDockWidgetArea, dockWidget);

    dockWidget = new QDockWidget("Image data", this);
    MObjectViewer* imageDataView = new MObjectViewer(
            memoryManager->getMemory()->get<MRawImages>(), this);
    dockWidget->setWidget(imageDataView);
    memoryManager->connectSlot(imageDataView, SLOT(updateView()), "MRawImages");
    this->addDockWidget(Qt::RightDockWidgetArea, dockWidget);

    // Make sure one of the images is toggled off for small screens
    bottomCIV->toggle();
}

void VisionViewer::update()
{
    //no useless computation
    if (!this->isVisible())
        return;

    imageTranscribe->acquireNewImage();

    vision->notifyImage(sensors->getImage(Camera::TOP), sensors->getImage(Camera::BOTTOM));
    // Will need to get these to be diffent thresholded images but vision
    // appears to only threhold one at the moment!
    bottomRawImage->mutable_image()->assign(reinterpret_cast<const char *>
                                            (vision->thresh->thresholdedBottom),
                                            AVERAGED_IMAGE_SIZE);
    topRawImage->mutable_image()->assign(reinterpret_cast<const char *>
                                         (vision->thresh->thresholded),
                                         AVERAGED_IMAGE_SIZE);
    emit imagesUpdated();
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
  update();
}


// void VisionViewer::setHorizonDebug(){
//   if (horizonD == false) horizonD = true;
//   else horizonD = false;
//   vision->thresh->setHorizonDebug(horizonD);
// }
// void VisionViewer::setShootingDebug(){
//   if (shootD == false) shootD = true;
//   else shootD = false;
//   vision->thresh->setDebugShooting(shootD);
// }
// void VisionViewer::setOpenFieldDebug(){
//   if (openFieldD == false) openFieldD = true;
//   else openFieldD = false;
//   vision->thresh->setDebugOpenField(openFieldD);
// }
// void VisionViewer::setEdgeDetectDebug(){
//   if (edgeDetectD == false) edgeDetectD = true;
//   else edgeDetectD = false;
//   vision->thresh->setDebugEdgeDetection(edgeDetectD);
// }
// void VisionViewer::setHoughDebug(){
//   if (houghD == false) houghD = true;
//   else houghD = false;
//   vision->thresh->setDebugHoughTransform(houghD);
// }
// void VisionViewer::setRobotsDebug(){
//   if (robotsD == false) robotsD = true;
//   else robotsD = false;
//   vision->thresh->setDebugRobots(robotsD);
// }
// void VisionViewer::setBallDebug(){
//   if (ballD == false) ballD = true;
//   else ballD = false;
//   vision->thresh->orange->setDebugBall(ballD);
// }
// void VisionViewer::setBallDistDebug(){
//   if (ballDistD == false) ballDistD = true;
//   else ballDistD = false;
//   vision->thresh->orange->setDebugBallDistance(ballDistD);
// }
// void VisionViewer::setCrossDebug(){
//   if (crossD == false) crossD = true;
//   else crossD = false;
//   vision->thresh->cross->setCrossDebug(crossD);
// }
// void VisionViewer::setIdentCornersDebug(){
//   if (identCornersD == false) identCornersD = true;
//   else identCornersD = false;
//   vision->thresh->context->setDebugIdentifyCorners(identCornersD);
// }
// void VisionViewer::setFieldEdgeDebug(){
//   if (fieldEdgeD == false) fieldEdgeD = true;
//   else fieldEdgeD = false;
//   vision->thresh->field->setDebugFieldEdge(robotsD);
// }

// void VisionViewer::setPostPrintDebug(){
//   if (postPrintD == false) postPrintD = true;
//   else postPrintD = false;
//   vision->thresh->yellow->setPrintObjs(postPrintD);
// }
// void VisionViewer::setPostDebug(){
//   if (postD == false) postD = true;
//   else postD = false;
//   vision->thresh->yellow->setPostDebug(postD);
// }
// void VisionViewer::setPostLogicDebug(){
//   if (postLogicD == false) postLogicD = true;
//   else postLogicD = false;
//   vision->thresh->yellow->setPostLogic(postLogicD);
// }
// void VisionViewer::setPostCorrectDebug(){
//   if (postCorrectD == false) postCorrectD = true;
//   else postCorrectD = false;
//   vision->thresh->yellow->setCorrect(postCorrectD);
// }
// void VisionViewer::setPostSanityDebug(){
//   if (postSanityD == false) postSanityD = true;
//   else postSanityD = false;
//   vision->thresh->yellow->setSanity(postSanityD);
// }
#define SET_DEBUG(funcName, buttonName)                             \
    void VisionViewer::set##funcName##Debug(int state) {            \
        vision->thresh->setDebug##funcName(state == Qt::Checked);   \
        update();                                                   \
    }

SET_DEBUG(Horizon, horizon);
SET_DEBUG(HoughTransform, hough);
SET_DEBUG(Shooting, shoot);
SET_DEBUG(EdgeDetection, edgeDetect);
SET_DEBUG(OpenField, openField);
SET_DEBUG(Robots, robots);
SET_DEBUG(VisualLines, visualLines);
SET_DEBUG(VisualCorners, visualCorners);

}
}
