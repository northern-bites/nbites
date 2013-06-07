#pragma once

#include <QtGui>

#include "RoboGrams.h"
#include "Camera.h"
#include "image/ImageDisplayModule.h"

// from Man
#include "vision/Vision.h"

namespace tool {
namespace calibrate {

class CalibrationLineWrapperModule : public portals::Module
{
    portals::InPortal<Camera::Type> cameraIn;
    portals::OutPortal<messages::CalibrateLines> linesOut;

protected:
    virtual void run_();

private:
    man::vision::VisionModule vision;
};

class CalibrationModule : public QMainWindow,
                          public portals::Module
{
    Q_OBJECT;

public:
    CalibrationModule(QWidget *parent = 0);

    portals::InPortal<messages::YUVImage> bottomImageIn;
    portals::InPortal<messages::YUVImage> topImageIn;

protected:
    virtual void run_();

private:
    portals::RoboGram subdiagram;

    QImage makeOverlay(Camera::Type which);

	QTabWidget* imageTabs;
	Camera::Type currentCamera;

	image::OverlayDisplayModule topDisplay;
    image::OverlayDisplayModule bottomDisplay;
};

}
}
