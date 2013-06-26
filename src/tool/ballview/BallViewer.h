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

namespace tool{
namespace ballview{

class BallViewer : public QWidget,
                    public portals::Module
{
    Q_OBJECT;

public:
    BallViewer(QWidget* parent = 0);

public:
    portals::InPortal<messages::RobotLocation> odometryIn;
    portals::InPortal<messages::FilteredBall>      ballIn;

protected:
    virtual void run_();

protected:
    BallViewerPainter* ballPainter;

    QVBoxLayout* mainLayout;
    QHBoxLayout* field;
    QHBoxLayout* resizeLayout;

    QPushButton* zoomInButton;
    QPushButton* zoomOutButton;

    float scaleFactor;
};



}
}
