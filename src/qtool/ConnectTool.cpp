#include "ConnectTool.h"

#include "viewer/RobotField.h"

namespace qtool {

using data::DataLoader;
using viewer::MemoryViewer;
using viewer::ParticleViewer;
using viewer::RobotField;
using overseer::OverseerClient;

ConnectTool::ConnectTool() : EmptyQTool("Robot Connection Tool"),
                             dataLoader(new DataLoader(dataManager)),
                             memoryViewer(new MemoryViewer(dataManager)),
                             fieldViewer(new ParticleViewer(dataManager)),
                             overseerClient(new OverseerClient(dataManager,
                                                               this))
{
    toolTabs->addTab(dataLoader, tr("Data Loader"));
    toolTabs->addTab(memoryViewer, tr("Log Viewer"));
    toolTabs->addTab(new RobotField(dataManager, this), tr("Robot Field"));
    toolTabs->addTab(fieldViewer, tr("Particle Viewer"));
    toolTabs->addTab(overseerClient, tr("Overseer"));

	scrollArea->setWidget(toolTabs);
	scrollArea->resize(toolTabs->size());
	this->setCentralWidget(scrollArea);
	tabStartSize = new QSize(toolTabs->size());
}

ConnectTool::~ConnectTool() {
}

}
