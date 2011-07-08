
#include "QTool.h"
#include <iostream>

namespace qtool {

using data::DataManager;
using data::DataLoader;
using colorcreator::ColorCreator;
using viewer::LogViewer;

QTool::QTool() : QMainWindow(),
        toolTabs(new QTabWidget()),
        dataManager(new DataManager()),
        dataLoader(new DataLoader(dataManager)),
        colorCreator(new ColorCreator(dataManager)),
        logViewer(new LogViewer(dataManager)){

    this->setWindowTitle(tr("HackTool"));

    this->setCentralWidget(toolTabs);

    toolTabs->addTab(colorCreator, tr("Color Creator"));
    toolTabs->addTab(dataLoader, tr("Data Loader"));
    toolTabs->addTab(logViewer, tr("Log Viewer"));

    dataManager->addSubscriber(colorCreator, data::NEW_IMAGE);
}

QTool::~QTool() {
    delete colorCreator;
    delete dataLoader;
    delete toolTabs;
}

}
