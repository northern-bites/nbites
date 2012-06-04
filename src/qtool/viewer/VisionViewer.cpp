
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

    QCheckBox* horizonDebug = new QCheckBox(tr("Horizon"));
    connect(horizonDebug, SIGNAL(clicked()), this, SLOT(setHorizonDebug()));
    toolBar->addWidget(horizonDebug);
    QCheckBox* shootingDebug = new QCheckBox(tr("Shooting"));
    connect(shootingDebug, SIGNAL(clicked()), this, SLOT(setShootingDebug()));
    toolBar->addWidget(shootingDebug);
    QCheckBox* openFieldDebug = new QCheckBox(tr("Open Field"));
    connect(openFieldDebug, SIGNAL(clicked()), this, SLOT(setOpenFieldDebug()));
    toolBar->addWidget(openFieldDebug);
    QCheckBox* EdgeDetectionDebug = new QCheckBox(tr("Edge Detection"));
    connect(EdgeDetectionDebug, SIGNAL(clicked()), this, SLOT(setEdgeDetectDebug()));
    toolBar->addWidget(EdgeDetectionDebug);
    QCheckBox* houghDebug = new QCheckBox(tr("Hough"));
    connect(houghDebug, SIGNAL(clicked()), this, SLOT(setHoughDebug()));
    toolBar->addWidget(houghDebug);
    QCheckBox* robotsDebug = new QCheckBox(tr("Robots"));
    connect(robotsDebug, SIGNAL(clicked()), this, SLOT(setRobotsDebug()));
    toolBar->addWidget(robotsDebug);
    QCheckBox* ballDebug = new QCheckBox(tr("Ball"));
    connect(ballDebug, SIGNAL(clicked()), this, SLOT(setBallDebug()));
    toolBar->addWidget(ballDebug);
    QCheckBox* ballDistanceDebug = new QCheckBox(tr("Ball Distance"));
    connect(ballDistanceDebug, SIGNAL(clicked()), this, SLOT(setBallDistDebug()));
    toolBar->addWidget(ballDistanceDebug);
    QCheckBox* crossDebug = new QCheckBox(tr("Cross"));
    connect(crossDebug, SIGNAL(clicked()), this, SLOT(setCrossDebug()));
    toolBar->addWidget(crossDebug);
    QCheckBox* identCornersDebug = new QCheckBox(tr("Identify Corners"));
    connect(identCornersDebug, SIGNAL(clicked()), this, SLOT(setIdentCornersDebug()));
    toolBar->addWidget(identCornersDebug);
    QCheckBox* dangerousBallDebug = new QCheckBox(tr("Dangerous Ball"));
    connect(dangerousBallDebug, SIGNAL(clicked()), this, SLOT(setDangerousBallDebug()));
    toolBar->addWidget(dangerousBallDebug);
    QCheckBox* fieldEdgeDebug = new QCheckBox(tr("Field Edge"));
    connect(fieldEdgeDebug, SIGNAL(clicked()), this, SLOT(setFieldEdgeDebug()));
    toolBar->addWidget(fieldEdgeDebug);

    QCheckBox* postPrintDebug = new QCheckBox(tr("Post Print"));
    connect(postPrintDebug, SIGNAL(clicked()), this, SLOT(setPostPrintDebug()));
    toolBar->addWidget(postPrintDebug);
    QCheckBox* postDebug = new QCheckBox(tr("Post"));
    connect(postDebug, SIGNAL(clicked()), this, SLOT(setPostDebug()));
    toolBar->addWidget(postDebug);
    QCheckBox* postLogicDebug = new QCheckBox(tr("Post Logic"));
    connect(postDebug, SIGNAL(clicked()), this, SLOT(setPostLogicDebug()));
    toolBar->addWidget(postLogicDebug);
    QCheckBox* postCorrectDebug = new QCheckBox(tr("Post Correct"));
    connect(postCorrectDebug, SIGNAL(clicked()), this, SLOT(setPostCorrectDebug()));
    toolBar->addWidget(postCorrectDebug);
    QCheckBox* postSanityDebug = new QCheckBox(tr("Post Sanity"));
    connect(postSanityDebug, SIGNAL(clicked()), this, SLOT(setPostSanityDebug()));
    toolBar->addWidget(postSanityDebug);
    /*
    QCheckBox* vertEdgeDetectDebug = new QCheckBox(tr("Vertical Edge"));
    connect(vertEdgeDetectDebug, SIGNAL(clicked()), this, SLOT(setVertEdgeDetectDebug()));
    toolBar->addWidget(vertEdgeDetectDebug);
    QCheckBox* horEdgeDetectDebug = new QCheckBox(tr("Horizontal Edge"));
    connect(horEdgeDetectDebug, SIGNAL(clicked()), this, SLOT(setHorEdgeDetectDebug()));
    toolBar->addWidget(horEdgeDetectDebug);
    QCheckBox* secondVertEdgeDebug = new QCheckBox(tr("Second Vertical Edge"));
    connect(secondVertEdgeDebug, SIGNAL(clicked()), this, SLOT(setSecondVertDebug()));
    toolBar->addWidget(secondVertEdgeDebug);
    QCheckBox* createLinesDebug = new QCheckBox(tr("Create Lines"));
    connect(createLinesDebug, SIGNAL(clicked()), this, SLOT(setCreateLinesDebug()));
    toolBar->addWidget(createLinesDebug);
    QCheckBox* fitUnusedPointsDebug = new QCheckBox(tr("Fit Unused Points"));
    connect(fitUnusedPointsDebug, SIGNAL(clicked()), this, SLOT(setFitUnusedPointsDebug()));
    toolBar->addWidget(fitUnusedPointsDebug);
    QCheckBox* joinLinesDebug = new QCheckBox(tr("Join Lines"));
    connect(joinLinesDebug, SIGNAL(clicked()), this, SLOT(setJoinLinesDebug()));
    toolBar->addWidget(joinLinesDebug);
    QCheckBox* extendLinesDebug = new QCheckBox(tr("Extend Lines"));
    connect(extendLinesDebug, SIGNAL(clicked()), this, SLOT(setExtendLinesDebug()));
    toolBar->addWidget(extendLinesDebug);
    QCheckBox* intersectLinesDebug = new QCheckBox(tr("Intersect Lines"));
    connect(intersectLinesDebug, SIGNAL(clicked()), this, SLOT(setIntersectLinesDebug()));
    toolBar->addWidget(intersectLinesDebug);
    QCheckBox* CcScanDebug = new QCheckBox(tr("CcScan"));
    connect(CcScanDebug, SIGNAL(clicked()), this, SLOT(setCcScanDebug()));
    toolBar->addWidget(CcScanDebug);
    QCheckBox* riskyCornersDebug = new QCheckBox(tr("Risky Corners"));
    connect(riskyCornersDebug, SIGNAL(clicked()), this, SLOT(setRiskyCornersDebug()));
    toolBar->addWidget(riskyCornersDebug);
    QCheckBox* cornObjDistDebug = new QCheckBox(tr("Corner and Object Distances"));
    connect(cornObjDistDebug, SIGNAL(clicked()), this, SLOT(setCornObjDistDebug()));
    toolBar->addWidget(cornObjDistDebug);
*/

    horizonD = shootD = openFieldD = edgeDetectD = houghD = robotsD = false;
    ballD = ballDistD = crossD = identCornersD = dangerousBallD = fieldEdgeD = false;
    postPrintD = postD = postLogicD = postCorrectD = postSanityD = false;
    vertEdgeD = horEdgeD = secVertD = createLD = fitPointD = joinLD = false;
    intersectLD = CcScanD = riskyCornerD = cornObjDistD = false;

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
void VisionViewer::setBallDebug(){
  if (ballD == false) ballD = true;
  else ballD = false;
  vision->thresh->orange->setDebugBall(ballD);
}
void VisionViewer::setBallDistDebug(){
  if (ballDistD == false) ballDistD = true;
  else ballDistD = false;
  vision->thresh->orange->setDebugBallDistance(ballDistD);
}
void VisionViewer::setCrossDebug(){
  if (crossD == false) crossD = true;
  else crossD = false;
  vision->thresh->cross->setCrossDebug(crossD);
}
void VisionViewer::setIdentCornersDebug(){
  if (identCornersD == false) identCornersD = true;
  else identCornersD = false;
  vision->thresh->context->setDebugIdentifyCorners(identCornersD);
}
void VisionViewer::setFieldEdgeDebug(){
  if (fieldEdgeD == false) fieldEdgeD = true;
  else fieldEdgeD = false;
  vision->thresh->field->setDebugFieldEdge(robotsD);
}

void VisionViewer::setPostPrintDebug(){
  if (postPrintD == false) postPrintD = true;
  else postPrintD = false;
  vision->thresh->yellow->setPrintObjs(postPrintD);
}
void VisionViewer::setPostDebug(){
  if (postD == false) postD = true;
  else postD = false;
  vision->thresh->yellow->setPostDebug(postD);
}
void VisionViewer::setPostLogicDebug(){
  if (postLogicD == false) postLogicD = true;
  else postLogicD = false;
  vision->thresh->yellow->setPostLogic(postLogicD);
}
void VisionViewer::setPostCorrectDebug(){
  if (postCorrectD == false) postCorrectD = true;
  else postCorrectD = false;
  vision->thresh->yellow->setCorrect(postCorrectD);
}
void VisionViewer::setPostSanityDebug(){
  if (postSanityD == false) postSanityD = true;
  else postSanityD = false;
  vision->thresh->yellow->setSanity(postSanityD);
}


}
}
