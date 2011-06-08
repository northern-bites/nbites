#include "RoboImageViewer.h"
#include "window.h"

//! [0]
const int IdRole = Qt::UserRole;
//! [0]

//! [1]
Window::Window(QWidget *parent) :
    QWidget(parent),
    roboImage(new RoboImage(640, 480)),
    pImage(new memory::proto::PImage())
{
    infoLabel = new QLabel(tr("Information"));
    roboImageViewer = new RoboImageViewer(roboImage, infoLabel);

    shapeComboBox = new QComboBox;
    shapeLabel = new QLabel(tr("&View:"));
    shapeLabel->setBuddy(shapeComboBox);

    QGridLayout *mainLayout = new QGridLayout;
    mainLayout->setColumnStretch(0, 1);
    mainLayout->setColumnStretch(3, 1);
    mainLayout->addWidget(roboImageViewer, 0, 0, 1, 4);
    mainLayout->setRowMinimumHeight(1, 6);
    mainLayout->addWidget(shapeLabel, 2, 1, Qt::AlignRight);
    mainLayout->addWidget(shapeComboBox, 2, 2);
    mainLayout->addWidget(infoLabel, 3, 1, Qt::AlignCenter);

    setLayout(mainLayout);

    setWindowTitle(tr("View Image"));

    QString currentDirectory = QFileDialog::getOpenFileName(this, tr("Open Image"),
    		"/home",
    		tr("Image Files (*.log)"));

    fp = new memory::log::MessageParser(pImage, currentDirectory.toStdString().data());
    fp->getNextMessage();
    roboImage->updateImage(pImage->image().data());
    roboImageViewer->updateBitmap();
    this->repaint();
}

