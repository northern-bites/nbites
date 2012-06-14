/*
 * Empty QTool
 */

#include "EmptyQTool.h"
#include <iostream>
#include <QFileDialog>
#include <QKeyEvent>
#include <QDebug>

namespace qtool {

using data::DataManager;
QFile file(QString("./.geometry"));

EmptyQTool::EmptyQTool(const char* title) : QMainWindow(),
                                            toolTabs(new QTabWidget()),
                                            dataManager(new DataManager())
{
    this->setWindowTitle(tr(title));

	toolbar = new QToolBar();
    nextButton = new QPushButton(tr(">"));
    prevButton = new QPushButton(tr("<"));
	RWButton = new QPushButton(tr("<<<"));
    FFWButton = new QPushButton(tr(">>>"));
    recordButton = new QPushButton(tr("Rec"));
	currFrame = QString("   Current Frame: ");
	frameCounter = new QLabel(currFrame);
    scrollArea = new QScrollArea();

    scrollBarSize = new QSize(5, 35);
    tabStartSize = new QSize(toolTabs->size());

    connect(nextButton, SIGNAL(clicked()), this, SLOT(next()));
    connect(prevButton, SIGNAL(clicked()), this, SLOT(prev()));
	connect(FFWButton, SIGNAL(clicked()), this, SLOT(skipAhead()));
    connect(RWButton, SIGNAL(clicked()), this, SLOT(skipBack()));
    connect(recordButton, SIGNAL(clicked()), this, SLOT(record()));
	connect(this, SIGNAL(frameChanged()), this, SLOT(frameUpdate()));

    toolbar->addWidget(RWButton);
	toolbar->addWidget(prevButton);
    toolbar->addWidget(nextButton);
	toolbar->addWidget(FFWButton);
    toolbar->addWidget(recordButton);
	addframes = toolbar->addWidget(frameCounter);

    this->addToolBar(toolbar);

    if (file.open(QIODevice::ReadWrite)){
            QTextStream in(&file);
            geom = new QRect(in.readLine().toInt(), in.readLine().toInt(),
                             in.readLine().toInt(), in.readLine().toInt());
            file.close();
    }
    if((geom->width() == 0) && (geom->height() == 0)){
        geom = new QRect(75, 75, 1132, 958);
    }
    this->setGeometry(*geom);
}

EmptyQTool::~EmptyQTool() {
    if (file.open(QIODevice::ReadWrite)){
        QTextStream out(&file);
        out << this->pos().x() << "\n"
            << this->pos().y() << "\n"
            << this->width() << "\n"
            << this->height() << "\n";
    }
}

void EmptyQTool::next() {
    dataManager->getNext();
    emit frameChanged();
}

void EmptyQTool::prev() {
    dataManager->getPrev();
    emit frameChanged();
}

void EmptyQTool::skipAhead() {
    dataManager->getSkipAhead();
    emit frameChanged();
}

void EmptyQTool::skipBack() {
    dataManager->getSkipBack();
    emit frameChanged();
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

void EmptyQTool::frameUpdate() {
	toolbar->removeAction(addframes);
	currFrame = QString("   Current Frame: ");
	currFrame.append(QString::number(dataManager->getCurrFrame()));
	frameCounter = new QLabel(currFrame);
	addframes = toolbar->addWidget(frameCounter);
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
    case Qt::Key_H:
    case Qt::Key_F:
        skipAhead();
        break;
    case Qt::Key_L:
    case Qt::Key_A:
        skipAhead();
        break;
    default:
        QWidget::keyPressEvent(event);
    }
}

void EmptyQTool::resizeEvent(QResizeEvent* ev)
{
    QSize widgetSize = ev->size();
    if((widgetSize.width() > tabStartSize->width()) &&
       (widgetSize.height() > tabStartSize->height())) {
        toolTabs->resize(widgetSize-*scrollBarSize);
    }
    else {
//do nothing - scroll bars will kick in by themselves
    }
    QWidget::resizeEvent(ev);
}

}
