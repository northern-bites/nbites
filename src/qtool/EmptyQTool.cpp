/*
 * Empty QTool
 */

#include "EmptyQTool.h"
#include <iostream>
#include <QFileDialog>
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
    recordButton = new QPushButton(tr("Rec"));
	scrollArea = new QScrollArea();

	scrollBarSize = new QSize(5, 35);
	tabStartSize = new QSize(toolTabs->size());

    connect(nextButton, SIGNAL(clicked()), this, SLOT(next()));
    connect(prevButton, SIGNAL(clicked()), this, SLOT(prev()));
    connect(recordButton, SIGNAL(clicked()), this, SLOT(record()));

    toolbar->addWidget(prevButton);
    toolbar->addWidget(nextButton);
    toolbar->addWidget(recordButton);

    this->addToolBar(toolbar);

	if (file.open(QIODevice::ReadWrite)){
			QTextStream in(&file);
			geom = new QRect(in.readLine().toInt(), in.readLine().toInt(),
							 in.readLine().toInt(), in.readLine().toInt());
	}
	this->setGeometry(*geom);
	file.close();
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

void EmptyQTool::resizeEvent(QResizeEvent* ev){
	QSize widgetSize = ev->size();
	if((widgetSize.width() > tabStartSize->width()) &&
	   (widgetSize.height() > tabStartSize->height())) {
		toolTabs->resize(widgetSize-*scrollBarSize);
	}
	else {
		qDebug()<<"QTool x or y dimension at minimum size, redrawing with scroll bars";
	}
	QWidget::resizeEvent(ev);
}

}
