#include "Tool.h"
#include <QTextStream>
#include <QFileDialog>
#include <QKeyEvent>
#include <QDebug>
#include <iostream>

namespace tool {

// This file saves the dimensions from the last use of the tool
QFile file(QString("./.geometry"));

Tool::Tool(const char* title) :
    QMainWindow(),
    diagram(),
    selector(),
    logView(this),
	tableCreator(this),
	visDispMod(this),
    fieldView(this),
  	colorCalibrate(this),
	topConverter(),
	bottomConverter(),
    toolTabs(new QTabWidget),
    toolbar(new QToolBar),
    nextButton(new QPushButton(tr(">"))),
    prevButton(new QPushButton(tr("<"))),
    recordButton(new QPushButton(tr("Rec"))),
	loadBtn(new QPushButton(tr("Load Table"))),
    scrollArea(new QScrollArea),
    scrollBarSize(new QSize(5, 35)),
    tabStartSize(new QSize(toolTabs->size()))
{
    // Set up the GUI and slots
    this->setWindowTitle(tr(title));

    //connect both fwd and prv button to the run slot
    connect(nextButton, SIGNAL(clicked()), &diagram, SLOT(runForward()));
    connect(prevButton, SIGNAL(clicked()), &diagram, SLOT(runBackward()));

    connect(&selector, SIGNAL(signalNewDataSet(std::vector<std::string>)),
            &diagram, SLOT(addUnloggers(std::vector<std::string>)));

    connect(&diagram, SIGNAL(signalNewDisplayWidget(QWidget*, std::string)),
            &logView, SLOT(newDisplayWidget(QWidget*, std::string)));

    connect(&diagram, SIGNAL(signalDeleteDisplayWidgets()),
            &logView, SLOT(deleteDisplayWidgets()));

    connect(&diagram, SIGNAL(signalUnloggersReady()),
            this, SLOT(setUpModules()));

	connect(&tableCreator, SIGNAL(tableChanges(std::vector<color::colorChanges>)), 
			this, SLOT(changeTableValues(std::vector<color::colorChanges>)));
	connect(&tableCreator, SIGNAL(tableUnChanges(std::vector<color::colorChanges>)), 
			this, SLOT(unChangeTableValues(std::vector<color::colorChanges>)));

    toolbar->addWidget(prevButton);
    toolbar->addWidget(nextButton);
    toolbar->addWidget(recordButton);

    toolTabs->addTab(&selector, tr("Data"));
    toolTabs->addTab(&logView, tr("Log View"));
	toolTabs->addTab(&tableCreator, tr("Color Creator"));
	toolTabs->addTab(&visDispMod, tr("Offline Vision"));
	toolTabs->addTab(&colorCalibrate, tr("Color Calibrator"));
    toolTabs->addTab(&fieldView, tr("FieldView"));
    toolTabs->addTab(&worldView, tr("World Viewer"));


    this->setCentralWidget(toolTabs);
    this->addToolBar(toolbar);

    // Figure out the appropriate dimensions for the window
    if (file.open(QIODevice::ReadWrite)){
            QTextStream in(&file);
            geometry = new QRect(in.readLine().toInt(), in.readLine().toInt(),
                                 in.readLine().toInt(), in.readLine().toInt());
            file.close();
    }
    // If we don't have dimensions, default to hard-coded values
    if((geometry->width() == 0) && (geometry->height() == 0)){
        geometry = new QRect(75, 75, 1000, 900);
    }
    this->setGeometry(*geometry);

	QToolBar* toolBar = new QToolBar(this);
    connect(loadBtn, SIGNAL(clicked()), this, SLOT(loadColorTable()));
	toolBar->addWidget(loadBtn);
	this->addToolBar(toolBar); 
	
	QPushButton* saveBtn = new QPushButton(tr("Save"));
	connect(saveBtn, SIGNAL(clicked()), this, SLOT(saveGlobalTable()));
	toolBar->addWidget(saveBtn);

	QPushButton* saveAsBtn = new QPushButton(tr("Save As"));
	connect(saveAsBtn, SIGNAL(clicked()), this, SLOT(saveAsGlobalTable()));
	toolBar->addWidget(saveAsBtn);

    loadLatestTable();
    //diagram.addModule(worldView);
}

Tool::~Tool() {
    // Write the current dimensions to the .geometry file for next use
    if (file.open(QIODevice::ReadWrite)){
        QTextStream out(&file);
        out << this->pos().x() << "\n"
            << this->pos().y() << "\n"
            << this->width() << "\n"
            << this->height() << "\n";
    }
}

void Tool::saveGlobalTable()
{

	if (loadBtn->text() == QString("Load Table")) { // no table loaded yet
		saveAsGlobalTable();
		return;
	}
	
	QString filename = loadBtn->text();
	globalColorTable.write(filename.toStdString());
    serializeTableName(filename);
}
void Tool::saveAsGlobalTable()
{

    QString base_directory = QString(NBITES_DIR) + "/data/tables";
    QString filename = QFileDialog::getSaveFileName(this,
					tr("Save Color Table to File"),
					base_directory + "/new_table.mtb",
					tr("Color Table files (*.mtb)"));
	globalColorTable.write(filename.toStdString());
	
	if (!filename.isEmpty()) {
		loadBtn->setText(filename);
        serializeTableName(filename);
    }
}

void Tool::loadColorTable()
{

    QString base_directory = QString(NBITES_DIR) + "/data/tables";
    QString filename = QFileDialog::getOpenFileName(this,
                    tr("Load Color Table from File"),
                    base_directory,
                    tr("Color Table files (*.mtb)"));
    globalColorTable.read(filename.toStdString());

	topConverter.loadTable(globalColorTable.getTable());
    bottomConverter.loadTable(globalColorTable.getTable());
	
	if (!filename.isEmpty()) {
		loadBtn->setText(filename);
        serializeTableName(filename);
    }

}

void Tool::loadLatestTable()
{
    QFile file("../../data/tables/latestTable.dat");
    file.open(QIODevice::ReadOnly);
    QDataStream in(&file);
    QString filename;
    in >> filename;
    if (!filename.isEmpty()) {
        globalColorTable.read(filename.toStdString());
        topConverter.loadTable(globalColorTable.getTable());
        bottomConverter.loadTable(globalColorTable.getTable());
        loadBtn->setText(filename);
    }
}

void Tool::serializeTableName(QString latestTableName)
{
    QFile file("../../data/tables/latestTable.dat");
    file.open(QIODevice::WriteOnly);
    QDataStream out(&file);
    out << latestTableName;
}

void Tool::changeTableValues(std::vector<color::colorChanges> tableAdjustments)
{
	for (int i = 0; i < tableAdjustments.size(); i++) {
		byte y = tableAdjustments[i].y;
		byte u = tableAdjustments[i].u;
		byte v = tableAdjustments[i].v;
		byte col = tableAdjustments[i].color;
		globalColorTable.setColor(y, u, v, col);
	}
	topConverter.changeTable(globalColorTable.getTable());
	bottomConverter.changeTable(globalColorTable.getTable());

}
	
void Tool::unChangeTableValues(std::vector<color::colorChanges> tableAdjustments)
{
	for (int i = 0; i < tableAdjustments.size(); i++) {
		byte y = tableAdjustments[i].y;
		byte u = tableAdjustments[i].u;
		byte v = tableAdjustments[i].v;
		byte col = tableAdjustments[i].color;
		globalColorTable.unSetColor(y, u, v, col);
	}
	topConverter.changeTable(globalColorTable.getTable());
	bottomConverter.changeTable(globalColorTable.getTable());
}

void Tool::setUpModules()
{
    diagram.connectToUnlogger<messages::YUVImage>(topConverter.imageIn, "top");
    diagram.connectToUnlogger<messages::YUVImage>(bottomConverter.imageIn, "bottom");

	diagram.connectToUnlogger<messages::YUVImage>(topConverter.imageIn, "top");
	diagram.connectToUnlogger<messages::YUVImage>(bottomConverter.imageIn, "bottom");
	diagram.addModule(topConverter);
	diagram.addModule(bottomConverter);
	topConverter.loadTable(globalColorTable.getTable());
    bottomConverter.loadTable(globalColorTable.getTable());


	diagram.addModule(visDispMod);
	diagram.connectToUnlogger<messages::YUVImage>(visDispMod.topImageIn,
												  "top");
	diagram.connectToUnlogger<messages::YUVImage>(visDispMod.bottomImageIn,
												  "bottom");
	if (diagram.connectToUnlogger<messages::JointAngles>(visDispMod.joints_in,
														 "joints") &&
		diagram.connectToUnlogger<messages::InertialState>(visDispMod.inerts_in,
														   "inertials"))
	{
		// All is well
	}
	else {
		std::cout << "Warning: Joints and Inertials not logged in this file.\n";
		portals::Message<messages::JointAngles> joints(0);
		portals::Message<messages::InertialState> inertials(0);
		visDispMod.joints_in.setMessage(joints);
		visDispMod.inerts_in.setMessage(inertials);
	}
	visDispMod.tTImage_in.wireTo(&topConverter.thrImage, true);
	visDispMod.tYImage_in.wireTo(&topConverter.yImage, true);
	visDispMod.tUImage_in.wireTo(&topConverter.uImage, true);
	visDispMod.tVImage_in.wireTo(&topConverter.vImage, true);
	
	visDispMod.bTImage_in.wireTo(&bottomConverter.thrImage, true);
	visDispMod.bYImage_in.wireTo(&bottomConverter.yImage, true);
	visDispMod.bUImage_in.wireTo(&bottomConverter.uImage, true);
	visDispMod.bVImage_in.wireTo(&bottomConverter.vImage, true);
	
	/** Color Table Creator Tab **/
    if (diagram.connectToUnlogger<messages::YUVImage>(tableCreator.topImageIn,
                                                      "top") &&
        diagram.connectToUnlogger<messages::YUVImage>(tableCreator.bottomImageIn,
                                                      "bottom"))
    {
        diagram.addModule(tableCreator);
		tableCreator.topThrIn.wireTo(&topConverter.thrImage, true);
		tableCreator.botThrIn.wireTo(&bottomConverter.thrImage, true);
    }
    else
    {
        std::cout << "Right now you can't use the color table creator without"
                  << " two image logs." << std::endl;
    }

    /** Color Calibrate Tab **/
    if (diagram.connectToUnlogger<messages::YUVImage>(colorCalibrate.topImageIn,
                                                      "top") &&
        diagram.connectToUnlogger<messages::YUVImage>(colorCalibrate.bottomImageIn,
                                                      "bottom"))
    {
        diagram.addModule(colorCalibrate);
    }
    else
    {
        std::cout << "Right now you can't use the color calibrator without"
                  << " two image logs." << std::endl;
    }


    /** FieldViewer Tab **/
    // Should add field view
    bool shouldAddFieldView = false;
    if(diagram.connectToUnlogger<messages::RobotLocation>(fieldView.locationIn,
                                                          "location"))
    {
        fieldView.confirmLocationLogs(true);
        shouldAddFieldView = true;
    }
    else
    {
        std::cout << "Warning: location wasn't logged in this file" << std::endl;
    }
    if(diagram.connectToUnlogger<messages::RobotLocation>(fieldView.odometryIn,
                                                          "odometry"))
    {
        fieldView.confirmOdometryLogs(true);
        shouldAddFieldView = true;
    }
    else
    {
        std::cout << "Warning: odometry wasn't logged in this file" << std::endl;
    }

    if(diagram.connectToUnlogger<messages::ParticleSwarm>(fieldView.particlesIn,
                                                          "particleSwarm"))
    {
        fieldView.confirmParticleLogs(true);
        shouldAddFieldView = true;
    }
    else
    {
        std::cout << "Warning: Particles weren't logged in this file" << std::endl;
    }
    if(diagram.connectToUnlogger<messages::VisionField>(fieldView.observationsIn,
                                                        "observations"))
    {
        fieldView.confirmObsvLogs(true);
        shouldAddFieldView = true;
    }
    else
    {
        std::cout << "Warning: Observations weren't logged in this file" << std::endl;
    }
    if(shouldAddFieldView)
        diagram.addModule(fieldView);


}

// Keyboard control
void Tool::keyPressEvent(QKeyEvent * event)
{
    switch (event->key()) {
    case Qt::Key_N:
        diagram.runForward();
        break;
    case Qt::Key_P:
        diagram.runBackward();
        break;
    default:
        QWidget::keyPressEvent(event);
    }
}

// Provides scrollbars appropriately if the window gets too small
void Tool::resizeEvent(QResizeEvent* ev)
{
    QSize widgetSize = ev->size();
    if((widgetSize.width() > tabStartSize->width()) ||
       (widgetSize.height() > tabStartSize->height())) {
        toolTabs->resize(widgetSize-*scrollBarSize);
    }
    QWidget::resizeEvent(ev);
}

}
