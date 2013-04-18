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

    mainLayout = new QHBoxLayout(this);

    //GUI
    particleViewBox = new QCheckBox("Particle Viewer",this);
    locationViewBox = new QCheckBox("Location Viewer", this);
    robotFieldViewBox = new QCheckBox("Robot Field Viewer",this);
    selector4 = new QCheckBox("test4", this);
    selector5= new QCheckBox("test5",this);
    selector6 = new QCheckBox("test6", this);
    selector7 = new QCheckBox("test7",this);
    selector8 = new QCheckBox("test8", this);

    field = new QHBoxLayout();
    field->addWidget(fieldPainter);

    checkBoxes = new QVBoxLayout();

    checkBoxes->addWidget(particleViewBox);
    checkBoxes->addWidget(locationViewBox);
    checkBoxes->addWidget(robotFieldViewBox);
    checkBoxes->addWidget(selector4);
    checkBoxes->addWidget(selector5);
    checkBoxes->addWidget(selector6);
    checkBoxes->addWidget(selector7);
    checkBoxes->addWidget(selector8);

    // Connect checkbox interface with slots in the painter
    // Assume no unloggers, hookup if proven wrong
    connect(particleViewBox, SIGNAL(toggled(bool)), this,
            SLOT(noLogError()));
    connect(locationViewBox, SIGNAL(toggled(bool)), this,
            SLOT(noLogError()));

    mainLayout->addLayout(field);
    mainLayout->addLayout(checkBoxes);

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

}

} // namespace viewer
} // namespace tool
