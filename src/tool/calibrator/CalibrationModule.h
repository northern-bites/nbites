#pragma once

#include <QtGui>

#include "RoboGrams.h"
#include "Camera.h"
#include "image/ImageDisplayModule.h"
#include "VisionField.pb.h"
#include "RobotLocation.pb.h"
#include "PMotion.pb.h"

// from Man
#include "vision/Vision.h"

namespace tool {
namespace calibrate {

class CalibrationModule : public QWidget,
                          public portals::Module
{
    Q_OBJECT;

public:
    CalibrationModule(QWidget *parent = 0);

    portals::InPortal<messages::JointAngles> jointsIn;
    portals::InPortal<messages::InertialState> inertialIn;

protected:
    virtual void run_();

protected slots:
    void imageTabSwitched(int i);

private:
    man::vision::Vision vision;

    //QImage makeOverlay(Camera::Type which);

	Camera::Type currentCamera;
    int currentX;
    int currentY;
    int currentH;

    QRadioButton goalie, center;
    // QRadioButton other;
    QSpinBox rollBox, pitchBox;
    QLabel rollLabel, pitchLabel;

    QGridLayout layout;
};

}
}
