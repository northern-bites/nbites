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
    // set up stuff

    diagram.runForward();
}

}
