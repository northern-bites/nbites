/*
 * Empty QTool
 */

#include "EmptyQTool.h"
#include <iostream>
#include <QFileDialog>
#include <QKeyEvent>

namespace qtool {

using data::DataManager;

EmptyQTool::EmptyQTool(const char* title) : QMainWindow(),
                                            toolTabs(new QTabWidget()),
                                            dataManager(new DataManager())
{
    this->setWindowTitle(tr(title));

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
}

EmptyQTool::~EmptyQTool() {
}

void EmptyQTool::next() {
    dataManager->getNext();
}

void EmptyQTool::prev() {
    dataManager->getPrev();
}

void EmptyQTool::record() {
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

void EmptyQTool::keyPressEvent(QKeyEvent * event)
{
    switch (event->key()) {
    case Qt::Key_J:
    case Qt::Key_D:
    case Qt::Key_N:
        next();
        break;
    case Qt::Key_K:
    case Qt::Key_S:
    case Qt::Key_P:
        prev();
        break;
    default:
        QWidget::keyPressEvent(event);
    }
}

}
