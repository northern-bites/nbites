#include "SharedViewer.h"

#include <QtDebug>

namespace tool{
namespace sharer{

SharedViewer::SharedViewer(QWidget* parent):
    QWidget(parent),
    wviewShared(1),
    mutex()
{
    for (int i = 0; i < NUM_PLAYERS_PER_TEAM; ++i)
    {
        worldsOut[i] = new portals::OutPortal<messages::WorldModel>(base());
    }

    // set things about players we don't want to change
    ballBearing = 0.f;
    myUncert = 1.f;
//    would be used to determine what the user is trying to place on the field
//    lastClicked = -1;

    fieldPainter = new SharedViewerPainter(this);

    mainLayout = new QVBoxLayout(this);

    zoomInButton = new QPushButton("+", this);
    zoomOutButton = new QPushButton("-", this);
    updateButton = new QPushButton("Update", this);

    selectorsP[0] = new QLabel(tr("GOALIE:      "));
    selectorsP[1] = new QLabel(tr("L DEF:         "));
    selectorsP[2] = new QLabel(tr("R DEF:         "));
    selectorsP[3] = new QLabel(tr("CHASER:      "));
    selectorsP[4] = new QLabel(tr("STRIKER:     "));

    selectorsB[0] = new QLabel(tr("GOALIE BALL:     "));
    selectorsB[1] = new QLabel(tr("L DEF BALL:        "));
    selectorsB[2] = new QLabel(tr("R DEF BALL:        "));
    selectorsB[3] = new QLabel(tr("CHASER BALL:     "));
    selectorsB[4] = new QLabel(tr("STRIKER BALL:    "));

    // none of these buttons currently do anything... only change global variable
    // To use click-on-field method, you'll want buttons. Currently using labels
//    selectorsP[0] = new QPushButton("Goalie");
//    selectorsP[1] = new QPushButton("L Def");
//    selectorsP[2] = new QPushButton("R Def");
//    selectorsP[3] = new QPushButton("Chaser");
//    selectorsP[4] = new QPushButton("Striker");

//    selectorsB[0] = new QPushButton("Goalie Ball");
//    selectorsB[1] = new QPushButton("L Def Ball");
//    selectorsB[2] = new QPushButton("R Def Ball");
//    selectorsB[3] = new QPushButton("Chaser Ball");
//    selectorsB[4] = new QPushButton("Striker Ball");

    field = new QHBoxLayout();
    field->addWidget(fieldPainter);

    selectionLayout = new QHBoxLayout();
    currentPlayer = new QLabel(tr("GOALIE"));

    resizeLayout = new QHBoxLayout();
    updateLayout = new QHBoxLayout();

    resizeLayout->addWidget(zoomOutButton);
    resizeLayout->addWidget(zoomInButton);
    updateLayout->addWidget(updateButton);

    xLabel = new QLabel(tr("my x -> "));
    yLabel = new QLabel(tr("my y -> "));
    ballXLabel = new QLabel(tr("ball x -> "));
    ballYLabel = new QLabel(tr("ball y -> "));

    for (int i = 0; i < NUM_PLAYERS_PER_TEAM; i++) {
        playerEntries[i] = new QHBoxLayout(this);
        playerX[i] = new QLineEdit(tr("0.0"));
        playerY[i] = new QLineEdit(tr("0.0"));
        ballsX[i] = new QLineEdit(tr("0.0"));
        ballsY[i] = new QLineEdit(tr("0.0"));

        playerEntries[i]->addWidget(selectorsP[i]);
        playerEntries[i]->addWidget(xLabel);
        playerEntries[i]->addWidget(playerX[i]);
        playerEntries[i]->addWidget(yLabel);
        playerEntries[i]->addWidget(playerY[i]);
        playerEntries[i]->addWidget(selectorsB[i]);
        playerEntries[i]->addWidget(ballXLabel);
        playerEntries[i]->addWidget(ballsX[i]);
        playerEntries[i]->addWidget(ballYLabel);
        playerEntries[i]->addWidget(ballsY[i]);
    }

    // Connect update button
    connect(updateButton, SIGNAL(released()), this,
            SLOT(updateMessageInfo()));

    // Connect the resize paintfield buttons
    connect(zoomInButton, SIGNAL(released()), fieldPainter,
            SLOT(handleZoomIn()));
    connect(zoomOutButton, SIGNAL(released()), fieldPainter,
            SLOT(handleZoomOut()));

// These would be used to implement placing robots on the field with clicking
    // Connect all selection buttons
//    for (int i = 0; i < NUM_PLAYERS_PER_TEAM; i++) {
//        connect(selectorsP[i], SIGNAL(released()), &mapper,
//                SLOT(map()));
//        mapper.setMapping(selectorsP[i], i);
//        connect(selectorsB[i], SIGNAL(released()), &mapper,
//                SLOT(map()));
//        mapper.setMapping(selectorsB[i], i + NUM_PLAYERS_PER_TEAM);
//    }

//    connect(&mapper, SIGNAL(mapped(int)), this, SLOT(changeSelection(int)));

    mainLayout->addLayout(updateLayout);
    for (int i = 0; i < NUM_PLAYERS_PER_TEAM; i++) {
        mainLayout->addLayout(playerEntries[i]);
    }
    mainLayout->addLayout(resizeLayout);
    mainLayout->addLayout(field);

    this->setLayout(mainLayout);

    diagram.addModule(wviewShared);
    diagram.addModule(*this);

    sharedBallIn.wireTo(&wviewShared.sharedBallOutput, true);
    flipLocIn.wireTo(&wviewShared.sharedBallReset, true);

    for (int i = 0; i < NUM_PLAYERS_PER_TEAM; i++) {
        wviewShared.worldModelIn[i].wireTo(worldsOut[i], true);
    }
}

/*
 * Is called when the user presses the update button. Takes the text from the
 * screen and updates the world model info that will be passed through to the
 * shared ball.
 */
void SharedViewer::updateMessageInfo()
{
    mutex.lock();
    float ballDist, tMyH;
    float tMyX, tMyY, tBallX, tBallY;
    for (int i = 0; i < NUM_PLAYERS_PER_TEAM; i++) {
        tMyX = playerX[i]->text().toFloat();
        tMyY = playerY[i]->text().toFloat();
        tBallX = ballsX[i]->text().toFloat();
        tBallY = ballsY[i]->text().toFloat();

        if (tBallX-tMyX > 0) {
            tMyH = std::atan((tBallY-tMyY)/(tBallX-tMyX)) / TO_RAD;
        } else if (tBallX-tMyX < 0) {
            tMyH = std::atan((tBallY-tMyY)/(tBallX-tMyX)) / TO_RAD - 180;
        } else {
            tMyH = 0.f;
        }
        ballDist = std::sqrt((tMyX-tBallX)*(tMyX-tBallX)+(tMyY-tBallY)*(tMyY-tBallY));

        xCoord[i] = tMyX;
        yCoord[i] = tMyY;
        heading[i] = tMyH;
        ballDistance[i] = ballDist;

        if (xCoord[i] == 0 && yCoord[i] == 0) {
            ballOn[i] = false;
        } else {
            ballOn[i] = true;
        }
    }
    timeStamp = int(monotonic_micro_time());
    diagram.run();
    mutex.unlock();
}

/*
 * Sets the world model message to output and takes the old shared ball
 * message in, in addition to the flip robot message.
 */
void SharedViewer::run_()
{
    //set messages to the out portals latched to offline
    for (int i = 0; i < NUM_PLAYERS_PER_TEAM; i++) {
        portals::Message<messages::WorldModel> worldMessage(0);

        worldMessage.get()->set_ball_on(ballOn[i]);
        worldMessage.get()->set_ball_bearing(ballBearing);
        worldMessage.get()->set_my_uncert(myUncert);
        worldMessage.get()->set_timestamp(timeStamp);
        worldMessage.get()->set_my_x(xCoord[i]);
        worldMessage.get()->set_my_y(yCoord[i]);
        worldMessage.get()->set_my_h(heading[i]);
        worldMessage.get()->set_ball_dist(ballDistance[i]);
        worldsOut[i]->setMessage(worldMessage);

        fieldPainter->updateWithLocationInfo(xCoord[i], yCoord[i],
                                  heading[i], ballDistance[i], i);
    }

    sharedBallIn.latch();
    fieldPainter->updateWithSharedBallMessage(sharedBallIn.message());

    flipLocIn.latch();
    checkForFlip(flipLocIn.message());
}

/*
 * If we should flip, flip the robot that was specified in the shared
 * ball module (under the "uncert" value). Display these values on the
 * screen in text. If flipped the diagram is run again.
 */
void SharedViewer::checkForFlip(messages::RobotLocation msg) {
    if (lastReset != msg.timestamp()) {
        lastReset = msg.timestamp();
        int rNum = int(msg.uncert()); //we set this as the robot num
        if (rNum == 0) {
            std::cout<<"Uh oh I am 0! (Okay if it's the first time)"<<std::endl;
            return;
        }

        xCoord[rNum-1] = msg.x();
        yCoord[rNum-1] = msg.y();
        heading[rNum-1] = msg.h();

        QString xStr = QString::number(xCoord[rNum-1]);
        QString yStr = QString::number(yCoord[rNum-1]);
        float sinHB, cosHB;
        float hb = TO_RAD*heading[rNum-1];
        sincosf(hb, &sinHB, &cosHB);

        QString bxString = QString::number(xCoord[rNum-1]+ballDistance[rNum-1]*cosHB);
        QString byString = QString::number(yCoord[rNum-1]+ballDistance[rNum-1]*sinHB);

        playerX[rNum-1]->setText(xStr);
        playerY[rNum-1]->setText(yStr);
        ballsX[rNum-1]->setText(bxString);
        ballsY[rNum-1]->setText(byString);

        diagram.run();
    }
}

/*
 * Updates what robot or ball we will update when the user clicks on the
 * field. This is called when the user presses on one of the player/ball buttons.
 */
//void SharedViewer::changeSelection(int num)
//{
//    lastClicked = num;
//}

/*
 * *****INCOMPLETE!!!******
 * Takes the mouse click as input, checks if it is in a legal position
 * on the field, and then updates the coordinates of the appropriate
 * ball or robot.
 */
// void SharedViewer::mousePressEvent(QMouseEvent *event)
// {
//     if (field.contains(event->pos)) {
//         QPoint lastPoint = event->pos();
//         QPoint localP = lastPoint->mapTo(field, relativeMousePosition);
//         std::cout<<localP<<std::endl;
//     }
// }

} // namespace shared
} // namespace tool
