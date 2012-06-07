#include "QTool.h"

namespace qtool {

using data::DataManager;
using data::DataLoader;
using colorcreator::ColorCalibrate;
using colorcreator::ColorTableCreator;
using viewer::MemoryViewer;
using viewer::VisionViewer;
using viewer::BallEKFViewer;
using viewer::FieldViewer;
using offline::OfflineViewer;
using overseer::OverseerClient;
  //using viewer::GraphViewer;

<<<<<<< HEAD
QTool::QTool() : QMainWindow(),
        toolTabs(new QTabWidget()),
        dataManager(new DataManager()),
        dataLoader(new DataLoader(dataManager)),
        colorCalibrate(new ColorCalibrate(dataManager)),
        colorTableCreator(new ColorTableCreator(dataManager)),
        memoryViewer(new MemoryViewer(dataManager)),
       	visionViewer(new VisionViewer(dataManager)),
        offlineViewer(new OfflineViewer(dataManager->getMemory())),
        ballEKFViewer(new BallEKFViewer(dataManager)),
        fieldViewer(new FieldViewer(dataManager)),
        overseerClient(new OverseerClient(dataManager, this))
		 //	 graphViewer(new GraphViewer(dataManager, this))			
{

    this->setWindowTitle(tr("QTOOL"));

    toolbar = new QToolBar();
    nextButton = new QPushButton(tr(">"));
    prevButton = new QPushButton(tr("<"));
    recordButton = new QPushButton(tr("Rec"));

    connect(nextButton, SIGNAL(clicked()), this, SLOT(next()));
    connect(prevButton, SIGNAL(clicked()), this, SLOT(prev()));
    connect(recordButton, SIGNAL(clicked()), this, SLOT(record()));

    toolbar->addWidget(prevButton);
    toolbar->addWidget(nextButton);
    toolbar->addWidget(recordButton);

    this->addToolBar(toolbar);

    this->setCentralWidget(toolTabs);

=======
QTool::QTool() : EmptyQTool("QTOOL"),
                 dataLoader(new DataLoader(dataManager)),
                 colorCalibrate(new ColorCalibrate(dataManager)),
                 colorTableCreator(new ColorTableCreator(dataManager)),
                 memoryViewer(new MemoryViewer(dataManager)),
                 visionViewer(new VisionViewer(dataManager)),
                 offlineViewer(new OfflineViewer(dataManager->getMemory())),
                 ballEKFViewer(new BallEKFViewer(dataManager)),
                 fieldViewer(new FieldViewer(dataManager)),
                 overseerClient(new OverseerClient(dataManager, this))
{
>>>>>>> bmende/twoPics
    toolTabs->addTab(colorCalibrate, tr("Color Calibrate"));
    toolTabs->addTab(colorTableCreator, tr("Color Table Creator"));
    toolTabs->addTab(dataLoader, tr("Data Loader"));
    toolTabs->addTab(memoryViewer, tr("Log Viewer"));
    toolTabs->addTab(visionViewer, tr("Vision Viewer"));
    toolTabs->addTab(offlineViewer, tr("Offline Viewer"));
    toolTabs->addTab(ballEKFViewer, tr("BallEKF Viewer"));
    toolTabs->addTab(fieldViewer, tr("Field Viewer"));
    toolTabs->addTab(overseerClient, tr("Overseer"));
    //toolTabs->addTab(graphViewer, tr("Graph Viewer"));
}

QTool::~QTool() {
}

}
