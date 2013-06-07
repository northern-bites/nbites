#include "CalibrationModule.h"

namespace tool {
namespace calibrate {

CalibrationModule::CalibrationModule(QWidget *parent) : QMainWindow(parent),
                                                        bottomImageIn(0),
                                                        topImageIn(0),
                                                        imageTabs(this)
{
    bottomImageIn = &bottomDisplay.imageIn;
    topImageIn = &topDisplay.imageIn;

    connect(&imageTabs, SIGNAL(currentChanged(int)),
            this, SLOT(imageTabSwitched()));
}

void CalibrationModule::run_()
{
    jointsIn.latch();
    inertialIn.latch();

    std::vector<boost::shared_ptr<man::vision::VisualLine> > lines =
        vision.getExpectedLines(currentCamera,
                                jointsIn.message(),
                                inertialIn.message(),
                                currentX,
                                currentY,
                                currentH);
}

void CalibrationModule::imageTabSwitched()
{
    if (imageTabs.currentWidget() == &topDisplay)
    {
        currentCamera = Camera::TOP;
    }
    if (imageTabs.currentWidget() == &bottomDisplay)
    {
        currentCamera = Camera::BOTTOM;
    }
}

}
}
