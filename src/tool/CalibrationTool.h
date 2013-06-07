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

protected:
    calibrate::CalibrationModule calibrator;
};

}
