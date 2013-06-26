#include "BallViewer.h"

#include <QtDebug>

namespace tool{
namespace ballview{

BallViewer::BallViewer(QWidget* parent):
    QWidget(parent),
    visionBallOut(base()),
    odometryOut(base()),
    localizationOut(base())
{
    ballPainter = new BallViewerPainter(this);

    mainLayout = new QVBoxLayout(this);

    //GUI
    zoomInButton = new QPushButton("+", this);
    zoomOutButton = new QPushButton("-", this);

    paintLogBox = new QCheckBox("Log", this);
    paintOfflineBox = new QCheckBox("Offline", this);

    field = new QHBoxLayout();
    field->addWidget(ballPainter);

    resizeLayout = new QHBoxLayout();
    resizeLayout->addWidget(zoomInButton);
    resizeLayout->addWidget(zoomOutButton);

    checkBoxes = new QHBoxLayout();
    checkBoxes->addWidget(paintLogBox);
    checkBoxes->addWidget(paintOfflineBox);

    // Connect the resize paintfield buttons
    connect(zoomInButton, SIGNAL(released()), ballPainter,
            SLOT(handleZoomIn()));
    connect(zoomOutButton, SIGNAL(released()), ballPainter,
            SLOT(handleZoomOut()));

    // Connect paint options
    connect(paintLogBox, SIGNAL(toggled(bool)), ballPainter,
            SLOT(paintLogAction(bool)));
    connect(paintOfflineBox, SIGNAL(toggled(bool)), ballPainter,
            SLOT(paintOfflineAction(bool)));

    mainLayout->addLayout(checkBoxes);
    mainLayout->addLayout(resizeLayout);
    mainLayout->addLayout(field);

    this->setLayout(mainLayout);

    //Setup offline balltrack
    trackMod.visionBallInput.wireTo(&visionBallOut, true);
    trackMod.odometryInput.wireTo(&odometryOut, true);
    trackMod.localizationInput.wireTo(&localizationOut, true);

    subdiagram.addModule(trackMod);
    subdiagram.addModule(ballListen);

    ballListen.ballIn.wireTo(&trackMod.ballLocationOutput, true);
}

void BallViewer::run_()
{
    odometryIn.latch();
    ballIn.latch();
    visionBallIn.latch();
    localizationIn.latch();
    // If have logs then paint them
    ballPainter->updateOdometry(odometryIn.message());
    ballPainter->updateFilteredBall(ballIn.message());

    // Run offline
    visionBallOut.setMessage(portals::Message<messages::VisionBall>(&visionBallIn.message()));
    odometryOut.setMessage(portals::Message<messages::RobotLocation>(&odometryIn.message()));
    localizationOut.setMessage(portals::Message<messages::RobotLocation>(&localizationIn.message()));

    subdiagram.run();

    ballPainter->updateOfflineFilteredBall(ballListen.ballIn.message());
}

} // namespace viewer
} // namespace tool
