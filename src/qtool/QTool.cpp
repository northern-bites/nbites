
#include "QTool.h"
#include <iostream>
#include <QFileDialog>

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
        memoryViewer(new MemoryViewer(dataManager)),
        offlineViewer(new OfflineViewer(dataManager->getMemory())),
        ballEKFViewer(new BallEKFViewer(dataManager)),
        fieldViewer(new FieldViewer(dataManager)) {

    this->setWindowTitle(tr("HackTool"));

    toolbar = new QToolBar();
    nextButton = new QPushButton(tr(">"));
    prevButton = new QPushButton(tr("<"));
    recordButton = new QPushButton(tr("Rec"));

    connect(nextButton, SIGNAL(clicked()), this, SLOT(next()));
    connect(prevButton, SIGNAL(clicked()), this, SLOT(prev()));
    connect(recordButton, SIGNAL(clicked()), this, SLOT(record()));

    toolbar->addWidget(prevButton);
    toolbar->addWidget(nextButton);
    toolbar->addWidget(recordButton);

    this->addToolBar(toolbar);

    this->setCentralWidget(toolTabs);

    toolTabs->addTab(colorCreator, tr("Color Creator"));
    toolTabs->addTab(dataLoader, tr("Data Loader"));
    toolTabs->addTab(memoryViewer, tr("Log Viewer"));
    toolTabs->addTab(offlineViewer, tr("Offline Viewer"));
    toolTabs->addTab(ballEKFViewer, tr("BallEKF Viewer"));
    toolTabs->addTab(fieldViewer, tr("Field Viewer"));
    toolTabs->addTab(new remote::RobotSelect, tr("remote"));

    dataManager->connectSlotToMObject(colorCreator,
            SLOT(updatedImage()), man::memory::MIMAGE_ID);
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

void QTool::record() {
    QString path = QFileDialog::getExistingDirectory(this);
    dataManager->startRecordingToPath(path.toStdString());
}

}
