#include "RoboImageViewer.h"
#include "window.h"

//! [0]
const int IdRole = Qt::UserRole;
//! [0]

//! [1]
Window::Window(YUVImage r1, QWidget *parent) :
    r(r1),
    QWidget(parent)
{
    infoLabel = new QLabel(tr("Information"));
    roboImageViewer = new RoboImageViewer(r, infoLabel);

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


    connect(shapeComboBox, SIGNAL(activated(int)),
            this, SLOT(shapeChanged()));

    QGridLayout *mainLayout = new QGridLayout;
    mainLayout->setColumnStretch(0, 1);
    mainLayout->setColumnStretch(3, 1);
    mainLayout->addWidget(roboImageViewer, 0, 0, 1, 4);
    mainLayout->setRowMinimumHeight(1, 6);
    mainLayout->addWidget(shapeLabel, 2, 1, Qt::AlignRight);
    mainLayout->addWidget(shapeComboBox, 2, 2);
    mainLayout->addWidget(infoLabel, 3, 1, Qt::AlignCenter);

    setLayout(mainLayout);

    shapeChanged();

    setWindowTitle(tr("View Image"));
}

void Window::shapeChanged()
{
    RoboImageViewer::Shape shape = RoboImageViewer::Shape(shapeComboBox->itemData(
            shapeComboBox->currentIndex(), IdRole).toInt());
    roboImageViewer->setShape(shape);
}

