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
#include "FieldConstants.h"

namespace tool {
namespace calibrate {

typedef std::vector<boost::shared_ptr<man::vision::VisualLine> > LineVector;

class CalibrationModule : public QMainWindow,
                          public portals::Module
{
    Q_OBJECT;

public:
    CalibrationModule(QWidget *parent = 0);

    portals::InPortal<messages::JointAngles> jointsIn;
    portals::InPortal<messages::InertialState> inertialIn;
    portals::InPortal<messages::YUVImage>* topImageIn;
    portals::InPortal<messages::YUVImage>* bottomImageIn;

protected:
    virtual void run_();

private:
    man::vision::Vision vision;

    QImage makeOverlay(LineVector& expected);

	Camera::Type currentCamera;
    int currentX, currentY, currentH;

    QRadioButton goalie, center;
    // QRadioButton other;
    QSpinBox rollBox, pitchBox;
    QLabel rollLabel, pitchLabel;

    image::OverlayDisplayModule topImage, bottomImage;

    QTabWidget images;

    QGridLayout layout;
    QWidget central;
};

}
}
