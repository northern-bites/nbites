
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

    ADD_DEBUG_CHECKBOX("Horizon", setDebugHorizon);
    ADD_DEBUG_CHECKBOX("Shooting", setDebugShooting);
    ADD_DEBUG_CHECKBOX("Open Field", setDebugOpenField);
    ADD_DEBUG_CHECKBOX("Edge Detection", setDebugEdgeDetection);
    ADD_DEBUG_CHECKBOX("Hough", setDebugHoughTransfrom);
    ADD_DEBUG_CHECKBOX("Robot Detection", setDebugRobots);
    ADD_DEBUG_CHECKBOX("Visual Line", setDebugVisualLines);
    ADD_DEBUG_CHECKBOX("Visual Corner", setDebugVisualCorners);
    ADD_DEBUG_CHECKBOX("Ball", setDebugBall);
    ADD_DEBUG_CHECKBOX("Ball Dist", setDebugBallDist);
    ADD_DEBUG_CHECKBOX("Cross", setCrossDebug);
    ADD_DEBUG_CHECKBOX("Identify Corners", setDebugIdentifyCorners);
    ADD_DEBUG_CHECKBOX("Field Edge", setDebugFieldEdge);
    ADD_DEBUG_CHECKBOX("Post Print", setPrintObjs);
    ADD_DEBUG_CHECKBOX("Post", setPostDebug);
    ADD_DEBUG_CHECKBOX("Post Correct", setCorrect);
    ADD_DEBUG_CHECKBOX("Post Sanity", setSanity);

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

    connect(this, SIGNAL(imagesUpdated()),
            bottomVisionView, SLOT(updateView()));

    topVisionView = new BMPImageViewer(topVisionImage, this);
    connect(this, SIGNAL(imagesUpdated()),
            topVisionView, SLOT(updateView()));
    connect(topVisionView, SIGNAL(mouseClicked(int, int, int, bool)),
            this, SLOT(pixelClicked(int, int, int, bool)));

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

#define SET_DEBUG(funcName, buttonName)                             \
    void VisionViewer::set##funcName(int state) {            \
        vision->thresh->set##funcName(state == Qt::Checked);   \
        update();                                                   \
    }

  SET_DEBUG(DebugHorizon, horizon);
  SET_DEBUG(DebugHoughTransform, hough);
  SET_DEBUG(DebugShooting, shoot);
  SET_DEBUG(DebugEdgeDetection, edgeDetect);
  SET_DEBUG(DebugOpenField, openField);
  SET_DEBUG(DebugRobots, robots);
  SET_DEBUG(DebugVisualLines, visualLines);
  SET_DEBUG(DebugVisualCorners, visualCorners);
#define POST_DEBUG(funcName, buttonName)                           \
  void VisionViewer::set##funcName(int state) {                    \
      vision->thresh->yellow->set##funcName(state == Qt::Checked); \
      update();                                                    \
  }
  POST_DEBUG(PrintObjs, postPrint);
  POST_DEBUG(PostDebug, post);
  POST_DEBUG(PostLogic, postLogic);
  POST_DEBUG(Correct, postCorrect);
  POST_DEBUG(Sanity, postSanity);

}
}
