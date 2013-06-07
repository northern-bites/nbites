#include "CalibrationTool.h"

namespace tool {

CalibrationTool::CalibrationTool(const char* title) : EmptyTool(title),
                                                      calibrator()
{
    connect(&diagram, SIGNAL(signalUnloggersReady()),
            this, SLOT(setUpModules()));

    disconnect(nextButton, SIGNAL(clicked()), &diagram, SLOT(runForward()));
    disconnect(prevButton, SIGNAL(clicked()), &diagram, SLOT(runBackward()));


    QMainWindow* calibrationTab = new QMainWindow(this);
    QWidget* calibrationWidget = new QWidget(calibrationTab);
    QHBoxLayout* calibrationLayout = new QHBoxLayout();

    images.addTab(&topDisplay, tr("Top"));
    images.addTab(&bottomDisplay, tr("Bottom"));

    connect(&images, SIGNAL(currentChanged(int)),
            &calibrator, SLOT(imageTabSwitched(int)));

    calibrationLayout->addWidget(&images);
    calibrationLayout->addWidget(&calibrator);

    calibrationWidget->setLayout(calibrationLayout);
    calibrationTab->setCentralWidget(calibrationWidget);

    toolTabs->addTab(calibrationTab, tr("Calibration"));
}

void CalibrationTool::setUpModules()
{
    if (diagram.connectToUnlogger<messages::YUVImage>(topDisplay.imageIn,
                                                     "top"))
    {
        diagram.addModule(topDisplay);
        images.setTabEnabled(0, true);
    }
    else
    {
        images.setTabEnabled(0, false);
    }

    if (diagram.connectToUnlogger<messages::YUVImage>(bottomDisplay.imageIn,
                                                     "bottom"))
    {
        diagram.addModule(bottomDisplay);
        images.setTabEnabled(1, true);
    }
    else
    {
        images.setTabEnabled(1, false);
    }

    if (diagram.connectToUnlogger<messages::JointAngles>(calibrator.jointsIn,
                                                     "joints") &&
        diagram.connectToUnlogger<messages::InertialState>(
            calibrator.inertialIn))
    {
        diagram.addModule(calibrator);
        calibrator.setDisabled(false);
    }
    else
    {
        calibrator.setDisabled(true);
    }

    diagram.runForward();
}

}
