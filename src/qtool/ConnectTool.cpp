#include "ConnectTool.h"

#include "viewer/RobotField.h"
#include "viewer/FieldViewer.h"

namespace qtool {

using data::DataLoader;
using viewer::MemoryViewer;
using viewer::ParticleViewer;
using viewer::RobotField;
using viewer::FieldViewer;
using overseer::OverseerClient;

ConnectTool::ConnectTool() : EmptyQTool("Robot Connection Tool"),
                             dataLoader(new DataLoader(dataManager)),
                             memoryViewer(new MemoryViewer(dataManager)),
                             particleViewer(new ParticleViewer(dataManager)),
                             overseerClient(new OverseerClient(dataManager,
                                                               this))
{
    toolTabs->addTab(dataLoader, tr("Data Loader"));
    toolTabs->addTab(memoryViewer, tr("Log Viewer"));
    toolTabs->addTab(new RobotField(dataManager, this), tr("Robot Field"));
    toolTabs->addTab(particleViewer, tr("Particle Viewer"));
    toolTabs->addTab(new FieldViewer(dataManager, this), tr("Field Viewer"));
    toolTabs->addTab(overseerClient, tr("Overseer"));

	scrollArea->setWidget(toolTabs);
	scrollArea->resize(toolTabs->size());
	this->setCentralWidget(scrollArea);
	tabStartSize = new QSize(toolTabs->size());
}

ConnectTool::~ConnectTool() {
}

}
