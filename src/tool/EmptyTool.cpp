#include "EmptyTool.h"
#include <QTextStream>
#include <QFileDialog>
#include <QKeyEvent>
#include <QDebug>
#include <iostream>

namespace tool {

// This file saves the dimensions from the last use of the tool
QFile file(QString("./.geometry"));

EmptyTool::EmptyTool(const char* title) :
    QMainWindow(),
    diagram(),
    selector(),
    logView(this),
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
        geometry = new QRect(75, 75, 1132, 958);
    }
    this->setGeometry(*geometry);
}

EmptyTool::~EmptyTool() {
    // Write the current dimensions to the .geometry file for next use
    if (file.open(QIODevice::ReadWrite)){
        QTextStream out(&file);
        out << this->pos().x() << "\n"
            << this->pos().y() << "\n"
            << this->width() << "\n"
            << this->height() << "\n";
    }
}

// Keyboard control
void EmptyTool::keyPressEvent(QKeyEvent * event)
{
    switch (event->key()) {
    case Qt::Key_N:
        diagram.runForward();
        break;
    case Qt::Key_P:
        diagram.runBackward();
        break;
    case Qt::Key_M:
        for(int i = 0; i < 5; i++)
        {
            diagram.runForward();
        }
        break;
    case Qt::Key_BracketLeft:
        for(int i = 0; i < 5; i++)
        {
            diagram.runBackward();
        }
        break;
    default:
        QWidget::keyPressEvent(event);
    }
}

// Provides scrollbars appropriately if the window gets too small
void EmptyTool::resizeEvent(QResizeEvent* ev)
{
    QSize widgetSize = ev->size();
    if((widgetSize.width() > tabStartSize->width()) ||
       (widgetSize.height() > tabStartSize->height())) {
        toolTabs->resize(widgetSize-*scrollBarSize);
    }
    QWidget::resizeEvent(ev);
}

void EmptyTool::setUpModules() {
}

}
