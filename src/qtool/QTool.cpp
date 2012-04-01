#include "QTool.h"
#include <iostream>
#include <QFileDialog>

namespace qtool {

using data::DataManager;
using data::DataLoader;
using colorcreator::ColorCalibrate;
using colorcreator::ColorTableCreator;
using viewer::MemoryViewer;
using viewer::BallEKFViewer;
using viewer::FieldViewer;
using offline::OfflineViewer;

QTool::QTool() : QMainWindow(),
        toolTabs(new QTabWidget()),
        dataManager(new DataManager()),
        dataLoader(new DataLoader(dataManager)),
        colorCalibrate(new ColorCalibrate(dataManager)),
        colorTableCreator(new ColorTableCreator(dataManager)),
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

    toolTabs->addTab(colorCalibrate, tr("Color Calibrate"));
    toolTabs->addTab(colorTableCreator, tr("Color Table Creator"));
    toolTabs->addTab(dataLoader, tr("Data Loader"));
    toolTabs->addTab(memoryViewer, tr("Log Viewer"));
    toolTabs->addTab(offlineViewer, tr("Offline Viewer"));
    toolTabs->addTab(ballEKFViewer, tr("BallEKF Viewer"));
    toolTabs->addTab(fieldViewer, tr("Field Viewer"));
}

QTool::~QTool() {
}

void QTool::next() {
    dataManager->getNext();
}

void QTool::prev() {
    dataManager->getPrev();
}

void QTool::record() {
    if (dataManager->isRecording()) {
        dataManager->stopRecording();
        recordButton->setText("Rec");
    } else {
        QString path = QFileDialog::getExistingDirectory(this, "Choose folder",
                QString(NBITES_DIR) + "/data/logs");
        if (!path.isEmpty()) {
            dataManager->startRecordingToPath(path.toStdString());
            recordButton->setText("Stop");
        }
    }
}

}
