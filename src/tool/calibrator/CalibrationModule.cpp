#include "CalibrationModule.h"
#include <iostream>

namespace tool {
namespace calibrate {

CalibrationModule::CalibrationModule(QWidget *parent) :
    goalie("Goalie Postition", this),
    center("Center Field Position", this),
    rollBox(this),
    pitchBox(this),
    rollLabel(tr("Roll")),
    pitchLabel(tr("Pitch"))
{
    layout.addWidget(&goalie, 0, 0, 1, 2);
    layout.addWidget(&center, 1, 0, 1, 2);
    layout.addWidget(&rollBox, 2, 0);
    layout.addWidget(&rollLabel, 2, 1);
    layout.addWidget(&pitchBox, 3, 0);
    layout.addWidget(&pitchLabel, 3, 1);

    setLayout(&layout);
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

void CalibrationModule::imageTabSwitched(int i)
{
    if (i == 0)
    {
        currentCamera = Camera::TOP;
    }
    else if (i == 1)
    {
        currentCamera = Camera::BOTTOM;
    }
    else
    {
        std::cout << "What is going on with your image tabs?" << std::endl;
    }
}

}
}
