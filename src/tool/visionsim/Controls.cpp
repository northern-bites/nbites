#include "Controls.h"
#include <QGridLayout>
#include <QLabel>
#include "WorldConstants.h"
#include "ImageConstants.h"

namespace tool{
namespace visionsim{

Controls::Controls(QWidget * parent) : QWidget(parent),
                                               robotX(this),
                                               robotY(this),
                                               robotH(this),
                                               headYaw(this),
                                               headPitch(this),
                                               ballX(this),
                                               ballY(this)
{
    QGridLayout* layout = new QGridLayout(this);

    // Robot X
    QLabel* xLabel = new QLabel(tr("Robot X"));
    robotX.setRange(GREEN_LEFT_EDGE_X, GREEN_RIGHT_EDGE_X);
    robotX.setSingleStep(10);
    layout->addWidget(xLabel,  0, 0);
    layout->addWidget(&robotX, 0, 1);

    connect(&robotX, SIGNAL(valueChanged(int)), this,
            SLOT(sendRobotInfo()));

    // Robot Y
    QLabel* yLabel = new QLabel(tr("Robot Y"));
    robotY.setRange(GREEN_BOTTOM_Y, GREEN_TOP_Y);
    robotY.setSingleStep(10);
    layout->addWidget(yLabel,  1, 0);
    layout->addWidget(&robotY, 1, 1);

    connect(&robotY, SIGNAL(valueChanged(int)), this,
            SLOT(sendRobotInfo()));

    // Robot H
    QLabel* hLabel = new QLabel(tr("Robot Heading"));
    robotH.setRange(-180, 180);
    layout->addWidget(hLabel,  2, 0);
    layout->addWidget(&robotH, 2, 1);

    connect(&robotH, SIGNAL(valueChanged(int)), this,
            SLOT(sendRobotInfo()));

    // Head yaw
    QLabel* yawLabel = new QLabel(tr("Head Yaw"));
    headYaw.setRange((int)MIN_HEAD_YAW, (int)MAX_HEAD_YAW);
    layout->addWidget(yawLabel, 3, 0);
    layout->addWidget(&headYaw, 3, 1);

    connect(&headYaw, SIGNAL(valueChanged(int)), this,
            SLOT(sendHeadInfo()));

    // Head pitch
    QLabel* pitchLabel = new QLabel(tr("Head Pitch"));
    headPitch.setRange((int)MIN_HEAD_PITCH, (int)MAX_HEAD_PITCH);
    layout->addWidget(pitchLabel, 4, 0);
    layout->addWidget(&headPitch, 4, 1);

    connect(&headPitch, SIGNAL(valueChanged(int)), this,
            SLOT(sendHeadInfo()));

    // Ball X
    QLabel* xBallLabel = new QLabel(tr("Ball X"));
    ballX.setRange(GREEN_LEFT_EDGE_X, GREEN_RIGHT_EDGE_X);
    ballX.setSingleStep(10);
    layout->addWidget(xBallLabel, 5, 0);
    layout->addWidget(&ballX,     5, 1);

    connect(&ballX, SIGNAL(valueChanged(int)), this,
            SLOT(sendBallInfo()));

    // Ball Y
    QLabel* yBallLabel = new QLabel(tr("Ball Y"));
    ballY.setRange(GREEN_BOTTOM_Y, GREEN_TOP_Y);
    ballY.setSingleStep(10);
    layout->addWidget(yBallLabel, 6, 0);
    layout->addWidget(&ballY,     6, 1);

    connect(&ballY, SIGNAL(valueChanged(int)), this,
            SLOT(sendBallInfo()));

    this->setLayout(layout);
}

void Controls::sendRobotInfo()
{
    emit( robotMoved((float)robotX.value(), (float)robotY.value(),
                     (float)robotH.value()));
}

void Controls::sendHeadInfo()
{
    emit( headMoved((float)headYaw.value(), (float)headPitch.value()));
}

void Controls::sendBallInfo()
{
    emit( ballMoved((float)ballX.value(), (float)ballY.value()));
}

}
}
