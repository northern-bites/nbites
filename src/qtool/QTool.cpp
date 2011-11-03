
#include "QTool.h"
#include <iostream>

namespace qtool {

using data::DataManager;
using data::DataLoader;
using colorcreator::ColorCreator;
using viewer::MemoryViewer;
using viewer::BallEKFViewer;
using viewer::FieldViewer;
using offline::OfflineViewer;

QTool::QTool() : QMainWindow(),
        toolTabs(new QTabWidget()),
        dataManager(new DataManager()),
        dataLoader(new DataLoader(dataManager)),
        colorCreator(new ColorCreator(dataManager)),
        memoryViewer(new MemoryViewer(dataManager->getMemory())),
        offlineViewer(new OfflineViewer(dataManager->getMemory())),
        ballEKFViewer(new BallEKFViewer(dataManager)),
        fieldViewer(new FieldViewer(dataManager)){

    this->setWindowTitle(tr("HackTool"));

    toolbar = new QToolBar();
    nextButton = new QPushButton(tr("&Next"));
    prevButton = new QPushButton(tr("&Previous"));

    connect(nextButton, SIGNAL(clicked()), this, SLOT(next()));
    connect(prevButton, SIGNAL(clicked()), this, SLOT(prev()));

    toolbar->addWidget(prevButton);
    toolbar->addWidget(nextButton);

    this->addToolBar(toolbar);

    this->setCentralWidget(toolTabs);

    toolTabs->addTab(colorCreator, tr("Color Creator"));
    toolTabs->addTab(dataLoader, tr("Data Loader"));
    toolTabs->addTab(memoryViewer, tr("Log Viewer"));
    toolTabs->addTab(offlineViewer, tr("Offline Viewer"));
    toolTabs->addTab(ballEKFViewer, tr("BallEKF Viewer"));
    toolTabs->addTab(fieldViewer, tr("Field Viewer"));

    dataManager->addSubscriber(colorCreator, man::memory::MIMAGE_ID);
}

QTool::~QTool() {
    delete colorCreator;
    delete dataLoader;
    delete toolTabs;
}

void QTool::next() {
    dataManager->getNext();
}

void QTool::prev() {
    dataManager->getPrev();
}

}
