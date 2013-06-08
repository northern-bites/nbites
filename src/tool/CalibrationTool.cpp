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
    diagram.connectToUnlogger<messages::YUVImage>(*calibrator.topImageIn,
                                                  "top");
    diagram.connectToUnlogger<messages::YUVImage>(*calibrator.bottomImageIn,
                                                  "bottom");

    diagram.connectToUnlogger<messages::JointAngles>(calibrator.jointsIn,
                                                     "joints");
    diagram.connectToUnlogger<messages::InertialState>(calibrator.inertialIn);

    diagram.addModule(calibrator);

    diagram.runForward();
}

}
