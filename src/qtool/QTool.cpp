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

QTool::QTool() : EmptyQTool("QTOOL"),
                 dataLoader(new DataLoader(dataManager)),
                 colorCalibrate(new ColorCalibrate(dataManager, this)),
                 colorTableCreator(new ColorTableCreator(dataManager)),
                 memoryViewer(new MemoryViewer(dataManager)),
                 visionViewer(new VisionViewer(dataManager)),
                 offlineViewer(new OfflineViewer(dataManager->getMemory())),
                 ballEKFViewer(new BallEKFViewer(dataManager)),
                 fieldViewer(new FieldViewer(dataManager)),
                 overseerClient(new OverseerClient(dataManager, this))
{
    toolTabs->addTab(colorCalibrate, tr("Color Calibrate"));
    toolTabs->addTab(colorTableCreator, tr("Color Table Creator"));
    toolTabs->addTab(dataLoader, tr("Data Loader"));
    toolTabs->addTab(memoryViewer, tr("Log Viewer"));
    toolTabs->addTab(visionViewer, tr("Vision Viewer"));
    toolTabs->addTab(offlineViewer, tr("Offline Viewer"));
    toolTabs->addTab(ballEKFViewer, tr("BallEKF Viewer"));
    toolTabs->addTab(fieldViewer, tr("Field Viewer"));
    toolTabs->addTab(overseerClient, tr("Overseer"));
}

QTool::~QTool() {
}

}
