#pragma once

#include "EmptyTool.h"

#include "calibrator/CalibrationModule.h"

namespace tool {

class CalibrationTool : public EmptyTool
{
    Q_OBJECT;

public:
    CalibrationTool(const char* title = "The Calibrator");

public slots:
    void setUpModules();
    void runForward();
    void runBackward();

protected:
    calibrate::CalibrationModule calibrator;
	image::OverlayDisplayModule topDisplay;
    image::OverlayDisplayModule bottomDisplay;
    QTabWidget images;
};

}
