/**
 * Class responsible for holding a PaintField object and
 * interacting with the user and field (Controller!)
 *
 * @author EJ Googins
 * @date   April 2013
 */
#pragma once

#include <QtGui>
#include <QCheckBox>
#include <vector>

#include "RoboGrams.h"
//#include "RobotLocation.pb.h"
//#include "ParticleSwarm.pb.h"
#include "BallModel.pb.h"
#include "WorldModel.pb.h"

#include "SharedViewerPainter.h"

#include "sharedball/SharedBall.h"
#include "Common.h"
#include "NBMath.h"

//#include "localization/LocalizationModule.h"
//#include "OfflineLocListener.h"

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

    portals::OutPortal<messages::WorldModel>* worldsOut[NUM_PLAYERS_PER_TEAM];
    //note: only setting the following in worldModel that are used by sharedball
    // ball_on, my_uncert, my_x, my_y, my_h, timestamp, ball_dist, ball_bearing

protected slots:
//    void noLogError();
    void updateMessageInfo();
    void nikkiOutput();

protected:
    virtual void run_();

    SharedViewerPainter* fieldPainter;

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
//    QPushButton* nikkiButton;

    QPushButton* selectorsP[NUM_PLAYERS_PER_TEAM];
    QPushButton* selectorsB[NUM_PLAYERS_PER_TEAM];
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

    QMutex mutex;

    man::context::SharedBallModule wviewShared;

private:
    portals::RoboGram diagram;

};



} //namespace shared
} //namespace tool
