/**
 * Class that allows user to input coordinates of robots and ball, and
 * then displays shared ball. It asychronously sends these inputs in
 * the form of world model to the shared ball module, and receives the
 * shared ball message, and whether or not the robot is flipped. It will
 * flip the robot if necessary.
 * (Don't be scared to press update a few times to see all the data)
 *
 * NOTE: Because the shared ball module is called only once, and not for
 *       every robot, we used a hack to specify what robot needs to flip:
 *       set the uncert value in the "RobotLocation" message to the robot
 *       number (index in world message array). This means that to use this
 *       feature, you must uncomment the appropriate lines in the shared
 *       ball module.
 *
 * TODO: Would be nice to be able to click on the screen where you want to
 *       place the robot/ball. Only half-implemented here. (in comments)
 *
 * @author Megan Maher
 * @date   June 2014
 */
#pragma once

#include <QtGui>
#include <QCheckBox>
#include <vector>

#include "RoboGrams.h"
#include "BallModel.pb.h"
#include "WorldModel.pb.h"

#include "SharedViewerPainter.h"

#include "sharedball/SharedBall.h"
#include "Common.h"
#include "NBMath.h"

namespace tool{
namespace sharer{

class SharedViewer : public QWidget,
                     public portals::Module
{
    Q_OBJECT;

public:
    SharedViewer(QWidget* parent = 0);

    portals::InPortal<messages::RobotLocation> flipLocIn;
    portals::InPortal<messages::SharedBall> sharedBallIn;

    //note: only setting the following in worldModel that are used by sharedball
    // ball_on, my_uncert, my_x, my_y, my_h, timestamp, ball_dist, ball_bearing
    portals::OutPortal<messages::WorldModel>* worldsOut[NUM_PLAYERS_PER_TEAM];

protected slots:
    void updateMessageInfo();
    void checkForFlip(messages::RobotLocation);
//    void changeSelection(int num);

protected:
    virtual void run_();
//    not completed: would make it so we could click on field to place ball/robot
//    void mousePressEvent(QMouseEvent *event);

    SharedViewerPainter* fieldPainter;

    int lastClicked; //will be used for determining what to place on field

    QLabel* xLabel;
    QLabel* yLabel;
    QLabel* ballXLabel;
    QLabel* ballYLabel;

    QLineEdit* playerX[NUM_PLAYERS_PER_TEAM];
    QLineEdit* playerY[NUM_PLAYERS_PER_TEAM];
    QLineEdit* ballsX[NUM_PLAYERS_PER_TEAM];
    QLineEdit* ballsY[NUM_PLAYERS_PER_TEAM];

    QVBoxLayout* mainLayout;
    QHBoxLayout* playerEntries[NUM_PLAYERS_PER_TEAM];
    QHBoxLayout* field;
    QHBoxLayout* resizeLayout;
    QHBoxLayout* updateLayout;
    QHBoxLayout* selectionLayout;

    QPushButton* zoomInButton;
    QPushButton* zoomOutButton;
    QPushButton* updateButton;

// Would be used for click-on-field selecting
//    QPushButton* selectorsP[NUM_PLAYERS_PER_TEAM];
//    QPushButton* selectorsB[NUM_PLAYERS_PER_TEAM];
    QLabel* selectorsP[NUM_PLAYERS_PER_TEAM];
    QLabel* selectorsB[NUM_PLAYERS_PER_TEAM];
    QLabel* currentPlayer;

    float scaleFactor;

    float ballBearing;
    float myUncert;
    int timeStamp;

    float ballOn[NUM_PLAYERS_PER_TEAM];
    float xCoord[NUM_PLAYERS_PER_TEAM];
    float yCoord[NUM_PLAYERS_PER_TEAM];
    float heading[NUM_PLAYERS_PER_TEAM];
    float ballDistance[NUM_PLAYERS_PER_TEAM];

    long long lastReset;

    QSignalMapper mapper;

    QMutex mutex;

    man::context::SharedBallModule wviewShared;

private:
    portals::RoboGram diagram;
};

} //namespace shared
} //namespace tool
