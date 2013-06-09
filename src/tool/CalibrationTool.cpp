#include "CalibrationTool.h"

namespace tool {

CalibrationTool::CalibrationTool(const char* title) : EmptyTool(title),
                                                      calibrator()
{
    connect(&diagram, SIGNAL(signalUnloggersReady()),
            this, SLOT(setUpModules()));

    toolTabs->addTab(&calibrator, tr("Calibration"));
}

void CalibrationTool::setUpModules()
{
    if (diagram.connectToUnlogger<messages::YUVImage>(*calibrator.topImageIn,
                                                      "top"))
    {
        calibrator.enableTopImage(true);
    }
    else
    {
        calibrator.enableTopImage(false);
    }

    if (diagram.connectToUnlogger<messages::YUVImage>(
            *calibrator.bottomImageIn, "bottom"))
    {
        calibrator.enableBottomImage(true);
    }
    else
    {
        calibrator.enableBottomImage(false);
    }

    if (diagram.connectToUnlogger<messages::JointAngles>(calibrator.jointsIn,
                                                     "joints") &&
        diagram.connectToUnlogger<messages::InertialState>(
            calibrator.inertialIn))
    {
        calibrator.enableCalibration(true);
    }
    else
    {
        calibrator.enableCalibration(false);
    }

    diagram.addModule(calibrator);

    diagram.runForward();
}

}
