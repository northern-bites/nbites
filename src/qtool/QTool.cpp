
#include "QTool.h"
#include <iostream>

namespace qtool {

using data::DataManager;
using data::DataLoader;
using colorcreator::ColorCreator;
using viewer::LogViewer;

QTool::QTool() : QMainWindow(),
        toolTabs(new QTabWidget()),
        colorScrollArea(new QScrollArea()),
        dataManager(new DataManager()),
        dataLoader(new DataLoader(dataManager)),
        colorCreator(new ColorCreator(dataManager)),
        logViewer(new LogViewer(dataManager)){

    this->setWindowTitle(tr("QTool"));

    this->setCentralWidget(toolTabs);

    colorScrollArea->setWidget(colorCreator);

    toolTabs->addTab(colorScrollArea, tr("Color Creator"));
    toolTabs->addTab(dataLoader, tr("Data Loader"));
    toolTabs->addTab(logViewer, tr("Log Viewer"));

    dataManager->addSubscriber(colorCreator, man::memory::MIMAGE_ID);
}

QTool::~QTool() {
    delete colorCreator;
    delete dataLoader;
    delete toolTabs;
}

}
