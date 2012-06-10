
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

    bottomRawImage->mutable_image()->assign(AVERAGED_IMAGE_SIZE, 0);
    topRawImage->mutable_image()->assign(AVERAGED_IMAGE_SIZE, 0);

    QToolBar* toolBar = new QToolBar(this);
    QPushButton* loadTableButton = new QPushButton(tr("&Load Table"));
    connect(loadTableButton, SIGNAL(clicked()), this, SLOT(loadColorTable()));
    toolBar->addWidget(loadTableButton);
    this->addToolBar(toolBar);

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
    layout->setAlignment(Qt::AlignTop);

    layout->addWidget(topCIV);
    layout->addWidget(bottomCIV);

    rawImages->setLayout(layout);

    BMPImageViewer *bottomVisViewer = new BMPImageViewerListener(bottomVisionImage,
                                                       this);
    connect(bottomVisViewer, SIGNAL(mouseClicked(int, int, int, bool)),
            this, SLOT(pixelClicked(int, int, int, bool)));
    connect(this, SIGNAL(imagesUpdated()),
            bottomVisViewer, SLOT(updateView()));

    BMPImageViewer *topVisViewer = new BMPImageViewer(topVisionImage,
                                                      this);
    connect(this, SIGNAL(imagesUpdated()),
            topVisViewer, SLOT(updateView()));

    CollapsibleImageViewer* bottomVisCIV = new
        CollapsibleImageViewer(bottomVisViewer,
                               "Bottom",
                               this);
    CollapsibleImageViewer* topVisCIV = new CollapsibleImageViewer(topVisViewer,
                                                                   "Top",
                                                                   this);

    QWidget* visionImages = new QWidget(this);

    QVBoxLayout* visLayout = new QVBoxLayout(visionImages);
    visLayout->setAlignment(Qt::AlignTop);

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
}

void VisionViewer::update()
{
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

    vision->notifyImage(sensors->getImage(Camera::TOP), sensors->getImage(Camera::BOTTOM));
    offlineMVision->updateData();
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
