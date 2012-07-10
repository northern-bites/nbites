#include "QTool.h"

#include "viewer/RobotField.h"
#include "viewer/FieldViewer.h"

namespace qtool {

using data::DataManager;
using data::DataLoader;
using colorcreator::ColorCalibrate;
using colorcreator::ColorTableCreator;
using viewer::MemoryViewer;
using viewer::VisionViewer;
using viewer::BallEKFViewer;
using viewer::ParticleViewer;
using viewer::FieldViewer;
using viewer::RobotField;
using offline::OfflineViewer;
using overseer::OverseerClient;
  //using viewer::GraphViewer;

QTool::QTool() : EmptyQTool("QTOOL"),
                 dataLoader(new DataLoader(dataManager)),
                 colorCalibrate(new ColorCalibrate(dataManager, this)),
                 colorTableCreator(new ColorTableCreator(dataManager)),
                 memoryViewer(new MemoryViewer(dataManager)),
                 visionViewer(new VisionViewer(dataManager)),
                 offlineViewer(new OfflineViewer(dataManager->getMemory())),
                 ballEKFViewer(new BallEKFViewer(dataManager)),
                 particleViewer(new ParticleViewer(dataManager)),
                 overseerClient(new OverseerClient(dataManager, this))
{
    toolTabs->addTab(colorCalibrate, tr("Color Calibrate"));
    toolTabs->addTab(colorTableCreator, tr("Color Table Creator"));
    toolTabs->addTab(dataLoader, tr("Data Loader"));
    toolTabs->addTab(memoryViewer, tr("Log Viewer"));
    toolTabs->addTab(visionViewer, tr("Vision Viewer"));
    toolTabs->addTab(offlineViewer, tr("Offline Viewer"));
    toolTabs->addTab(ballEKFViewer, tr("BallEKF Viewer"));
    toolTabs->addTab(particleViewer, tr("Particle Viewer"));
    toolTabs->addTab(new RobotField(dataManager, this), tr("Robot Field"));
    toolTabs->addTab(new FieldViewer(dataManager, this), tr("Field Viewer"));
    toolTabs->addTab(overseerClient, tr("Overseer"));
    //toolTabs->addTab(graphViewer, tr("Graph Viewer"));

	scrollArea->setWidget(toolTabs);
	scrollArea->resize(toolTabs->size());
	this->setCentralWidget(scrollArea);

	tabStartSize = new QSize(toolTabs->size());
}


QTool::~QTool() {
}
}

