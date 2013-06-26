#include "BallViewer.h"

#include <QtDebug>

namespace tool{
namespace ballview{

BallViewer::BallViewer(QWidget* parent):
    QWidget(parent)
{
    ballPainter = new BallViewerPainter(this);

    mainLayout = new QVBoxLayout(this);

    //GUI
    zoomInButton = new QPushButton("+", this);
    zoomOutButton = new QPushButton("-", this);

    field = new QHBoxLayout();
    field->addWidget(ballPainter);

    resizeLayout = new QHBoxLayout();
    resizeLayout->addWidget(zoomInButton);
    resizeLayout->addWidget(zoomOutButton);

    // Connect the resize paintfield buttons
    connect(zoomInButton, SIGNAL(released()), ballPainter,
            SLOT(handleZoomIn()));
    connect(zoomOutButton, SIGNAL(released()), ballPainter,
            SLOT(handleZoomOut()));

    mainLayout->addLayout(resizeLayout);
    mainLayout->addLayout(field);

    this->setLayout(mainLayout);
}

void BallViewer::run_()
{
    odometryIn.latch();
    ballIn.latch();
    // If have logs then paint them
    ballPainter->updateOdometry(odometryIn.message());
    ballPainter->updateFilteredBall(ballIn.message());
}

} // namespace viewer
} // namespace tool
