#include "FieldViewer.h"

#include <QtDebug>

namespace tool{
namespace viewer{

FieldViewer::FieldViewer(QWidget* parent):
    QWidget(parent),
    haveParticleLogs(false),
    haveLocationLogs(false),
    haveVisionFieldLogs(false)
{
    fieldPainter = new FieldViewerPainter(this);

    mainLayout = new QVBoxLayout(this);

    //GUI
    particleViewBox = new QCheckBox("Particle Viewer",this);
    locationViewBox = new QCheckBox("Location Viewer", this);
    robotFieldViewBox = new QCheckBox("Robot Field Viewer",this);

    zoomInButton = new QPushButton("+", this);
    zoomOutButton = new QPushButton("-", this);

    field = new QHBoxLayout();
    field->addWidget(fieldPainter);

    checkBoxes = new QHBoxLayout();
    resizeLayout = new QHBoxLayout();

    checkBoxes->addWidget(particleViewBox);
    checkBoxes->addWidget(locationViewBox);
    checkBoxes->addWidget(robotFieldViewBox);

    resizeLayout->addWidget(zoomInButton);
    resizeLayout->addWidget(zoomOutButton);

    // Connect checkbox interface with slots in the painter
    // Assume no unloggers, hookup if proven wrong
    connect(particleViewBox, SIGNAL(toggled(bool)), this,
            SLOT(noLogError()));
    connect(locationViewBox, SIGNAL(toggled(bool)), this,
            SLOT(noLogError()));
    connect(robotFieldViewBox, SIGNAL(toggled(bool)), this,
            SLOT(noLogError()));

    // Connect the resize paintfield buttons
    connect(zoomInButton, SIGNAL(released()), fieldPainter,
            SLOT(handleZoomIn()));
    connect(zoomOutButton, SIGNAL(released()), fieldPainter,
            SLOT(handleZoomOut()));

    mainLayout->addLayout(checkBoxes);
    mainLayout->addLayout(resizeLayout);
    mainLayout->addLayout(field);

    this->setLayout(mainLayout);
}

void FieldViewer::confirmParticleLogs(bool haveLogs)
{
    haveParticleLogs = haveLogs;
    if(haveLogs) {
        connect(particleViewBox, SIGNAL(toggled(bool)), fieldPainter,
                SLOT(paintParticleAction(bool)));

        disconnect(particleViewBox, SIGNAL(toggled(bool)), this,
                   SLOT(noLogError()));
    }
    else {
        disconnect(particleViewBox, SIGNAL(toggled(bool)), fieldPainter,
                SLOT(paintParticleAction(bool)));

        connect(particleViewBox, SIGNAL(toggled(bool)), this,
                   SLOT(noLogError()));
    }
}

void FieldViewer::confirmOdometryLogs(bool haveLogs)
{
    haveOdometryLogs = haveLogs;
}

void FieldViewer::confirmLocationLogs(bool haveLogs)
{
    haveLocationLogs = haveLogs;
    if(haveLogs) {
        connect(locationViewBox, SIGNAL(toggled(bool)), fieldPainter,
                SLOT(paintLocationAction(bool)));

        disconnect(locationViewBox, SIGNAL(toggled(bool)), this,
                   SLOT(noLogError()));
    }
    else {
        disconnect(locationViewBox, SIGNAL(toggled(bool)), fieldPainter,
                SLOT(paintLocationAction(bool)));

        connect(locationViewBox, SIGNAL(toggled(bool)), this,
                   SLOT(noLogError()));
    }
}

void FieldViewer::confirmObsvLogs(bool haveLogs)
{
    haveVisionFieldLogs = haveLogs;
    if(haveLogs) {
        connect(robotFieldViewBox, SIGNAL(toggled(bool)), fieldPainter,
                SLOT(paintObsvAction(bool)));

        disconnect(robotFieldViewBox, SIGNAL(toggled(bool)), this,
                   SLOT(noLogError()));
    }
    else {
        disconnect(robotFieldViewBox, SIGNAL(toggled(bool)), fieldPainter,
                SLOT(paintObsvAction(bool)));

        connect(robotFieldViewBox, SIGNAL(toggled(bool)), this,
                   SLOT(noLogError()));
    }
}

void FieldViewer::noLogError()
{
    qDebug() << "Sorry, the opened log file does not include those logs";
}

void FieldViewer::run_()
{
    if (haveLocationLogs) {
        locationIn.latch();
        fieldPainter->updateWithLocationMessage(locationIn.message());
    }

    if (haveParticleLogs) {
        particlesIn.latch();
        fieldPainter->updateWithParticleMessage(particlesIn.message());
    }

    if (haveVisionFieldLogs) {
        observationsIn.latch();
        fieldPainter->updateWithObsvMessage(observationsIn.message());
    }

}

} // namespace viewer
} // namespace tool
