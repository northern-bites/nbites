#include "renderarea.h"
#include "window.h"

//! [0]
const int IdRole = Qt::UserRole;
//! [0]

//! [1]
Window::Window(RoboImage r1, QWidget *parent) :
    r(r1),
    QWidget(parent)
{
    infoLabel = new QLabel(tr("Information"));
    renderArea = new RenderArea(r, infoLabel);

    shapeComboBox = new QComboBox;
    shapeComboBox->addItem(tr("Pixmap"), RenderArea::Pixmap);
    shapeComboBox->addItem(tr("Blue"), RenderArea::Blue);
    shapeComboBox->addItem(tr("Red"), RenderArea::Red);
    shapeComboBox->addItem(tr("Green"), RenderArea::Green);
    shapeComboBox->addItem(tr("Y"), RenderArea::Y);
    shapeComboBox->addItem(tr("U"), RenderArea::U);
    shapeComboBox->addItem(tr("V"), RenderArea::V);
    shapeComboBox->addItem(tr("H"), RenderArea::H);
    shapeComboBox->addItem(tr("S"), RenderArea::S);
    shapeComboBox->addItem(tr("Z"), RenderArea::Z);
    shapeComboBox->addItem(tr("Edge"), RenderArea::EDGE);
    shapeComboBox->addItem(tr("W"), RenderArea::WHEEL);

    shapeLabel = new QLabel(tr("&View:"));
    shapeLabel->setBuddy(shapeComboBox);


    connect(shapeComboBox, SIGNAL(activated(int)),
            this, SLOT(shapeChanged()));

    QGridLayout *mainLayout = new QGridLayout;
    mainLayout->setColumnStretch(0, 1);
    mainLayout->setColumnStretch(3, 1);
    mainLayout->addWidget(renderArea, 0, 0, 1, 4);
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
    RenderArea::Shape shape = RenderArea::Shape(shapeComboBox->itemData(
            shapeComboBox->currentIndex(), IdRole).toInt());
    renderArea->setShape(shape);
}

