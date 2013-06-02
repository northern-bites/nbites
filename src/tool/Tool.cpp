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
    topConverter(Camera::TOP),
    bottomConverter(Camera::BOTTOM),
    toolTabs(new QTabWidget),
    toolbar(new QToolBar),
    nextButton(new QPushButton(tr(">"))),
    prevButton(new QPushButton(tr("<"))),
    recordButton(new QPushButton(tr("Rec"))),
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

    toolbar->addWidget(prevButton);
    toolbar->addWidget(nextButton);
    toolbar->addWidget(recordButton);

    toolTabs->addTab(&selector, tr("Data"));
    toolTabs->addTab(&logView, tr("Log View"));
    toolTabs->addTab(&tableCreator, tr("Color Creator"));
    toolTabs->addTab(&visDispMod, tr("Offline Vision"));
    toolTabs->addTab(&fieldView, tr("FieldView"));

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
    QPushButton* loadBtn = new QPushButton("Load Table", this);
    connect(loadBtn, SIGNAL(clicked()), this, SLOT(loadColorTable()));
    toolBar->addWidget(loadBtn);
    this->addToolBar(toolBar);

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

void Tool::loadColorTable()
{

    QString base_directory = QString(NBITES_DIR) + "/data/tables";
    QString filename = QFileDialog::getOpenFileName(this,
                    tr("Load Color Table from File"),
                    base_directory,
                    tr("Color Table files (*.mtb)"));
    globalColorTable.read(filename.toStdString());
    topConverter.initTable(globalColorTable.getTable());
    bottomConverter.initTable(globalColorTable.getTable());


}

void Tool::setUpModules()
{
    diagram.connectToUnlogger<messages::YUVImage>(topConverter.imageIn, "top");
    diagram.connectToUnlogger<messages::YUVImage>(bottomConverter.imageIn, "bottom");

    diagram.addModule(topConverter);
    diagram.addModule(bottomConverter);
    topConverter.initTable(globalColorTable.getTable());
    bottomConverter.initTable(globalColorTable.getTable());



    diagram.addModule(visDispMod);
    diagram.connectToUnlogger<messages::YUVImage>(visDispMod.topImageIn,
                                                  "top");
    diagram.connectToUnlogger<messages::YUVImage>(visDispMod.bottomImageIn,
                                                  "bottom");
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
    }
    else
    {
        std::cout << "Right now you can't use the color table creator without"
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
        std::cout << "Warning: Particles were'nt logged in this file" << std::endl;
    }
    if(diagram.connectToUnlogger<messages::VisionField>(fieldView.observationsIn,
                                                        "observations"))
    {
        fieldView.confirmObsvLogs(true);
        shouldAddFieldView = true;
    }
    else
    {
        std::cout << "Warning: Observations were'nt logged in this file" << std::endl;
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
