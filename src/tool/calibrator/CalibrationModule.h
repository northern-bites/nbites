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

class CalibrationModule : public QMainWindow,
                          public portals::Module
{
    Q_OBJECT;

public:
    CalibrationModule(QWidget *parent = 0);

    // To the displays
    portals::InPortal<messages::YUVImage>* bottomImageIn;
    portals::InPortal<messages::YUVImage>* topImageIn;

    portals::InPortal<messages::JointAngles> jointsIn;
    portals::InPortal<messages::InertialState> inertialIn;

protected:
    virtual void run_();

protected slots:
    void imageTabSwitched();

private:
    man::vision::Vision vision;

    portals::RoboGram subdiagram;

    //QImage makeOverlay(Camera::Type which);

	QTabWidget imageTabs;
	Camera::Type currentCamera;
    int currentX;
    int currentY;
    int currentH;

	image::OverlayDisplayModule topDisplay;
    image::OverlayDisplayModule bottomDisplay;
};

}
}
