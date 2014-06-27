#include "SharedViewer.h"

#include <QtDebug>

namespace tool{
namespace sharer{

SharedViewer::SharedViewer(QWidget* parent):
    QWidget(parent),
    mutex()
//    locMod(),
//    locListen(),
{
    for (int i = 0; i < NUM_PLAYERS_PER_TEAM; ++i)
    {
        worldsOut[i] = new portals::OutPortal<messages::WorldModel>(base());
    }

    // set things about players we don't want to change
    ballBearing = 0.f;
    myUncert = 1.f;
    timeStamp = 1;

    fieldPainter = new SharedViewerPainter(this);

    mainLayout = new QVBoxLayout(this);

    zoomInButton = new QPushButton("+", this);
    zoomOutButton = new QPushButton("-", this);
    updateButton = new QPushButton("Update", this);
//    nikkiButton = new QPushButton("Nikki Button", this);

    selectorsP[0] = new QPushButton("Goalie");
    selectorsP[1] = new QPushButton("L Def");
    selectorsP[2] = new QPushButton("R Def");
    selectorsP[3] = new QPushButton("Chaser");
    selectorsP[4] = new QPushButton("Striker");

    selectorsB[0] = new QPushButton("Goalie Ball");
    selectorsB[1] = new QPushButton("L Def Ball");
    selectorsB[2] = new QPushButton("R Def Ball");
    selectorsB[3] = new QPushButton("Chaser Ball");
    selectorsB[4] = new QPushButton("Striker Ball");

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

//    connect(nikkiButton, SIGNAL(released()), this,
//            SLOT(nikkiOutput()));

    // Connect the resize paintfield buttons
    connect(zoomInButton, SIGNAL(released()), fieldPainter,
            SLOT(handleZoomIn()));
    connect(zoomOutButton, SIGNAL(released()), fieldPainter,
            SLOT(handleZoomOut()));

    mainLayout->addLayout(updateLayout);
    for (int i = 0; i < NUM_PLAYERS_PER_TEAM; i++) {
        mainLayout->addLayout(playerEntries[i]);
    }
//    mainLayout->addWidget(nikkiButton);
    mainLayout->addLayout(resizeLayout);
    mainLayout->addLayout(field);

    this->setLayout(mainLayout);

    diagram.addModule(wviewShared);
    diagram.addModule(*this);

    sharedBallIn.wireTo(&wviewShared.sharedBallOutput, true);
//    flipLocIn.wireTo(&wviewShared.sharedBallReset, true);

    for (int i = 0; i < NUM_PLAYERS_PER_TEAM; i++) {
        wviewShared.worldModelIn[i].wireTo(worldsOut[i], true);
    }
}

void SharedViewer::nikkiOutput() {
    std::cout<<"HELLO NIKKI!"<<std::endl;
}

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
    diagram.run();
    mutex.unlock();
}

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

//    flipLocIn.latch();
//    checkForFlip(flipLocIn.message());
}

} // namespace shared
} // namespace tool
