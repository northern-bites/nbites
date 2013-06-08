#pragma once

#include <QtGui>

#include "RoboGrams.h"
#include "Camera.h"
#include "FieldConstants.h"
#include "image/ImageDisplayModule.h"
#include "VisionField.pb.h"
#include "RobotLocation.pb.h"
#include "PMotion.pb.h"

// from Man
#include "vision/Vision.h"
#include "CameraCalibrate.h"

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

protected slots:
    void switchCamera();
    void useGoaliePosition(bool checked);
    void useCenterPosition(bool checked);
    void useOtherPosition(bool checked);
    void useNewXValue(int value);
    void useNewYValue(int value);
    void useNewHValue(int value);
    void loadRobotParameters();

protected:
    virtual void run_();
    void updateOverlay();
    void turnOffOtherPosition();
    void turnOnOtherPosition();

private:
    man::vision::Vision vision;

    QImage makeOverlay(LineVector& expected);

	Camera::Type currentCamera;
    int currentX, currentY, currentH;

    QRadioButton goalie, center, other;
    QSpinBox setX, setY, setH;
    QLabel xLabel, yLabel, hLabel;
    QDoubleSpinBox rollBox, pitchBox;
    QLabel rollLabel, pitchLabel;
    QPushButton loadButton;
    QComboBox robotNames;

    image::OverlayDisplayModule topImage, bottomImage;

    QTabWidget images;

    QGridLayout layout;
    QWidget central;
};

}
}
