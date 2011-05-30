#include "RoboImageViewer.h"
#include "window.h"

//! [0]
const int IdRole = Qt::UserRole;
//! [0]

//! [1]
Window::Window(QWidget *parent) :
    QWidget(parent),
    roboImage(new RoboImage(640, 480)),
    yuvImage(new YUVImage(roboImage)),
    pImage(new memory::proto::PImage())
{
    infoLabel = new QLabel(tr("Information"));
    roboImageViewer = new RoboImageViewer(yuvImage, infoLabel);

    shapeComboBox = new QComboBox;
    shapeComboBox->addItem(tr("Pixmap"), RoboImageViewer::Pixmap);
    shapeComboBox->addItem(tr("Blue"), RoboImageViewer::Blue);
    shapeComboBox->addItem(tr("Red"), RoboImageViewer::Red);
    shapeComboBox->addItem(tr("Green"), RoboImageViewer::Green);
    shapeComboBox->addItem(tr("Y"), RoboImageViewer::Y);
    shapeComboBox->addItem(tr("U"), RoboImageViewer::U);
    shapeComboBox->addItem(tr("V"), RoboImageViewer::V);
    shapeComboBox->addItem(tr("H"), RoboImageViewer::H);
    shapeComboBox->addItem(tr("S"), RoboImageViewer::S);
    shapeComboBox->addItem(tr("Z"), RoboImageViewer::Z);
    shapeComboBox->addItem(tr("Edge"), RoboImageViewer::EDGE);
    shapeComboBox->addItem(tr("W"), RoboImageViewer::WHEEL);

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

    fp = new memory::log::FileParser(pImage, currentDirectory.toStdString().data());
    fp->getNextMessage();
    roboImage->updateImage(pImage->image().data());
    yuvImage->updateFromRoboImage();
    this->repaint();
}

