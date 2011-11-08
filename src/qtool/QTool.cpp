
#include "QTool.h"
#include <iostream>

namespace qtool {

using data::DataManager;
using data::DataLoader;
using colorcreator::ColorCreator;
using viewer::LogViewer;
using viewer::BallEKFViewer;
using viewer::FieldViewer;

QTool::QTool() : QMainWindow(),
        toolTabs(new QTabWidget()),
        dataManager(new DataManager()),
        dataLoader(new DataLoader(dataManager)),
        colorCreator(new ColorCreator(dataManager)),
	logViewer(new LogViewer(dataManager)),
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
    toolTabs->addTab(logViewer, tr("Log Viewer"));
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
