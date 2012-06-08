#include "QTool.h"
#include <iostream>
#include <QFileDialog>

namespace qtool {

	using data::DataManager;
	using data::DataLoader;
	using colorcreator::ColorCalibrate;
	using colorcreator::ColorTableCreator;
	using viewer::MemoryViewer;
	using viewer::VisionViewer;
	using viewer::BallEKFViewer;
	using viewer::FieldViewer;
	using offline::OfflineViewer;
	using overseer::OverseerClient;

	QFile file(QString("./.geometry"));

	QTool::QTool() : QMainWindow(),
					 toolTabs(new QTabWidget()),
					 dataManager(new DataManager()),
					 dataLoader(new DataLoader(dataManager)),
					 colorCalibrate(new ColorCalibrate(dataManager)),
					 colorTableCreator(new ColorTableCreator(dataManager)),
					 memoryViewer(new MemoryViewer(dataManager)),
					 visionViewer(new VisionViewer(dataManager)),
					 offlineViewer(new OfflineViewer(dataManager->getMemory())),
					 ballEKFViewer(new BallEKFViewer(dataManager)),
					 fieldViewer(new FieldViewer(dataManager)),
					 overseerClient(new OverseerClient(dataManager, this)) {

		this->setWindowTitle(tr("QTOOL"));

		toolbar = new QToolBar();
		scrollArea = new QScrollArea();
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
		toolTabs->addTab(visionViewer, tr("Vision Viewer"));
		toolTabs->addTab(offlineViewer, tr("Offline Viewer"));
		toolTabs->addTab(ballEKFViewer, tr("BallEKF Viewer"));
		toolTabs->addTab(fieldViewer, tr("Field Viewer"));
		toolTabs->addTab(overseerClient, tr("Overseer"));

		scrollArea->setWidget(toolTabs);
		scrollArea->resize(toolTabs->size());
		this->setCentralWidget(scrollArea);

		if (file.open(QIODevice::ReadWrite)){
			QTextStream in(&file);
			geom = new QRect(in.readLine().toInt(), in.readLine().toInt(),
							 in.readLine().toInt(), in.readLine().toInt());
		}
		this->setGeometry(*geom);
		file.close();
	}

	QTool::~QTool() {
		if (file.open(QIODevice::ReadWrite)){
			QTextStream out(&file);
			out << this->pos().x() << endl
				<< this->pos().y() << endl
				<< this->width() << endl
				<< this->height() << endl;
		}
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

	void QTool::resizeEvent(QResizeEvent* ev){
		QSize widgetSize = ev->size();
		toolTabs->resize(widgetSize);
		qDebug()<<widgetSize<<toolTabs->size();
		this->setCentralWidget(scrollArea);
		QWidget::resizeEvent(ev);
	}
}

