#include "CalibrationTool.h"

namespace tool {

CalibrationTool::CalibrationTool(const char* title) : EmptyTool(title),
                                                      calibrator()
{
    toolTabs->addTab(&calibrator, tr("Calibration"));

    connect(&diagram, SIGNAL(signalUnloggersReady()),
            this, SLOT(setUpModules()));
}

void CalibrationTool::setUpModules()
{
    if (diagram.connectToUnlogger<messages::YUVImage>(calibrator.topImageIn,
                                                      "top") &&
        diagram.connectToUnlogger<messages::YUVImage>(calibrator.bottomImageIn,
                                                      "bottom"))
    {
        diagram.addModule(calibrator);
    }
    else
    {
        std::cout << "Right now you can't use the calibrator without"
                  << " two image logs." << std::endl;
    }
}

}
