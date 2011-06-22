#include "coloredit.h"

ColorEdit::ColorEdit(Colors c1, YUVImage r1, QWidget *parent) :
    color(c1),
    r(r1),
    QWidget(parent)
{
    hMin = new QSlider(Qt::Horizontal, this);
    connect(hMin, SIGNAL(valueChanged(int)), this, SLOT(grabHMin(int)) );
    hMax = new QSlider(Qt::Horizontal, this);
    connect(hMax, SIGNAL(valueChanged(int)), this, SLOT(grabHMax(int)) );
    zMin = new QSlider(Qt::Horizontal, this);
    connect(zMin, SIGNAL(valueChanged(int)), this, SLOT(grabZMin(int)) );
    zMax = new QSlider(Qt::Horizontal, this);
    connect(zMax, SIGNAL(valueChanged(int)), this, SLOT(grabZMax(int)) );
    sMin = new QSlider(Qt::Horizontal, this);
    connect(sMin, SIGNAL(valueChanged(int)), this, SLOT(grabSMin(int)) );
    sMax = new QSlider(Qt::Horizontal, this);
    connect(sMax, SIGNAL(valueChanged(int)), this, SLOT(grabSMax(int)) );
    QLabel *hMinLabel = new QLabel("H Lower Bound");

    QGridLayout *mainLayout = new QGridLayout;
    mainLayout->setColumnStretch(0, 1);
    mainLayout->setColumnStretch(3, 1);
    mainLayout->addWidget(hMin, 0, 0, 1, 4);
    mainLayout->addWidget(hMax, 1, 0, 1, 4);
    mainLayout->addWidget(zMin, 2, 0, 1, 4);
    mainLayout->addWidget(zMax, 3, 0, 1, 4);
    mainLayout->addWidget(sMin, 4, 0, 1, 4);
    mainLayout->addWidget(sMax, 5, 0, 1, 4);

    setLayout(mainLayout);

    setWindowTitle(tr("View Image"));
    show();
}

void ColorEdit::grabHMin(int x)
{
    if (hLow != x)
    {
        hLow = x;
        //emit valueChanged(x);
    }
}

void ColorEdit::grabHMax(int x)
{

}


void ColorEdit::grabZMin(int x)
{

}


void ColorEdit::grabZMax(int x)
{

}


void ColorEdit::grabSMin(int x)
{

}


void ColorEdit::grabSMax(int x)
{

}

