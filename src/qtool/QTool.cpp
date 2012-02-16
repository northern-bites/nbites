
#include "QTool.h"
#include <iostream>
#include <QFileDialog>

namespace qtool {

using data::DataManager;
using data::DataLoader;
using colorcreator::ColorCalibrate;
using viewer::MemoryViewer;
using viewer::BallEKFViewer;
using viewer::FieldViewer;
using offline::OfflineViewer;

QTool::QTool() : QMainWindow(),
        toolTabs(new QTabWidget()),
<<<<<<< HEAD
 	colorScrollArea(new QScrollArea()),
=======
        colorScrollArea(new QScrollArea()),
>>>>>>> newBots
        dataManager(new DataManager()),
        dataLoader(new DataLoader(dataManager)),
        colorCalibrate(new ColorCalibrate(dataManager)),
        memoryViewer(new MemoryViewer(dataManager)),
        offlineViewer(new OfflineViewer(dataManager->getMemory())),
        ballEKFViewer(new BallEKFViewer(dataManager)),
        fieldViewer(new FieldViewer(dataManager)) {

<<<<<<< HEAD
    this->setWindowTitle(tr("The New Tool of Awesome"));

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
=======
    this->setWindowTitle(tr("QTool"));
>>>>>>> newBots

    this->setCentralWidget(toolTabs);

    colorScrollArea->setWidget(colorCreator);

    toolTabs->addTab(colorScrollArea, tr("Color Creator"));
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
