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
#include "RobotLocation.pb.h"
#include "BallModel.pb.h"

#include "BallViewerPainter.h"
#include "balltrack/BallTrackModule.h"
#include "OfflineBallListener.h"

namespace tool{
namespace ballview{

class BallViewer : public QWidget,
                    public portals::Module
{
    Q_OBJECT;

public:
    BallViewer(QWidget* parent = 0);

    void setupOffline();

public:
    portals::InPortal<messages::RobotLocation>  odometryIn;
    portals::InPortal<messages::FilteredBall>   ballIn;
    portals::InPortal<messages::VisionBall>     visionBallIn;
    portals::InPortal<messages::RobotLocation>  localizationIn;

    portals::OutPortal<messages::VisionBall>    visionBallOut;
    portals::OutPortal<messages::RobotLocation> odometryOut;
    portals::OutPortal<messages::RobotLocation> localizationOut;

    portals::InPortal<messages::FilteredBall>   offlineBallIn;

protected:
    virtual void run_();

protected:
    man::balltrack::BallTrackModule trackMod;
    tool::OfflineBallListener ballListen;
    portals::RoboGram subdiagram;

    BallViewerPainter* ballPainter;

    QVBoxLayout* mainLayout;
    QHBoxLayout* field;

    QHBoxLayout* resizeLayout;
    QPushButton* zoomInButton;
    QPushButton* zoomOutButton;

    QHBoxLayout* checkBoxes;
    QCheckBox* paintLogBox;
    QCheckBox* paintOfflineBox;

    QHBoxLayout* ballStateLayout;
    QLabel* logState;
    QLabel* offState;

    float scaleFactor;
};



}
}
