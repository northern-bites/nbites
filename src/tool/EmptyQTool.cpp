/*
 * Empty QTool
 */

#include "EmptyQTool.h"
#include <QTextStream>
#include <QFileDialog>
#include <QKeyEvent>
#include <QDebug>
#include <iostream>

namespace tool {

QFile file(QString("./.geometry"));

EmptyQTool::EmptyQTool(const char* title) :
    QMainWindow(),
    toolTabs(new QTabWidget),
    toolbar(new QToolBar),
    nextButton(new QPushButton(tr(">"))),
    prevButton(new QPushButton(tr("<"))),
    recordButton(new QPushButton(tr("Rec"))),
    scrollArea(new QScrollArea),
    scrollBarSize(new QSize(5, 35)),
    tabStartSize(new QSize(toolTabs->size()))

{
    this->setWindowTitle(tr(title));

    connect(nextButton, SIGNAL(clicked()), this, SLOT(next()));
    connect(prevButton, SIGNAL(clicked()), this, SLOT(prev()));
    connect(recordButton, SIGNAL(clicked()), this, SLOT(record()));


    toolbar->addWidget(prevButton);
    toolbar->addWidget(nextButton);
    toolbar->addWidget(recordButton);

    this->addToolBar(toolbar);

    if (file.open(QIODevice::ReadWrite)){
            QTextStream in(&file);
            geometry = new QRect(in.readLine().toInt(), in.readLine().toInt(),
                                 in.readLine().toInt(), in.readLine().toInt());
            file.close();
    }
    if((geometry->width() == 0) && (geometry->height() == 0)){
        geometry = new QRect(75, 75, 1132, 958);
    }
    this->setGeometry(*geometry);
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
    std::cout << "NEXT!" << std::endl;
}

void EmptyQTool::prev() {
    std::cout << "PREV!" << std::endl;
}


void EmptyQTool::record() {
    std::cout << "RECORD!" << std::endl;
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

void EmptyQTool::resizeEvent(QResizeEvent* ev)
{
    QSize widgetSize = ev->size();
    if((widgetSize.width() > tabStartSize->width()) ||
       (widgetSize.height() > tabStartSize->height())) {
        toolTabs->resize(widgetSize-*scrollBarSize);
    }
    QWidget::resizeEvent(ev);
}

}
