#include "OfflineTool.h"

#include "data/DataLoader.h"
#include "viewer/MemoryViewer.h"
#include "offline/OfflineViewer.h"

namespace qtool {

using data::DataLoader;
using viewer::MemoryViewer;
using offline::OfflineViewer;

OfflineTool::OfflineTool() : EmptyQTool("Offline TOOL")
{

    toolTabs->addTab(new DataLoader(dataManager, this), tr("Data Loader"));
    toolTabs->addTab(new MemoryViewer(dataManager, this), tr("Log Viewer"));
    toolTabs->addTab(new OfflineViewer(dataManager, this), tr("Offline Viewer"));
    toolTabs->addTab(new MemoryViewer(dataManager, this), tr("Offline Viewer"));

	scrollArea->setWidget(toolTabs);
	scrollArea->resize(toolTabs->size());
	this->setCentralWidget(scrollArea);
	tabStartSize = new QSize(toolTabs->size());
}

OfflineTool::~OfflineTool() {
}

}
