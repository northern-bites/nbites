#include "ConnectTool.h"

namespace qtool {

using data::DataLoader;
using viewer::MemoryViewer;
using viewer::FieldViewer;
using overseer::OverseerClient;

ConnectTool::ConnectTool() : EmptyQTool("Robot Connection Tool"),
                             dataLoader(new DataLoader(dataManager)),
                             memoryViewer(new MemoryViewer(dataManager)),
                             fieldViewer(new FieldViewer(dataManager, this)),
                             overseerClient(new OverseerClient(dataManager,
                                                               this))
{
    toolTabs->addTab(dataLoader, tr("Data Loader"));
    toolTabs->addTab(memoryViewer, tr("Log Viewer"));
    toolTabs->addTab(fieldViewer, tr("Field Viewer"));
    toolTabs->addTab(overseerClient, tr("Overseer"));
}

ConnectTool::~ConnectTool() {
}

}
