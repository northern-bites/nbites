#include "CalibrationModule.h"
#include <iostream>

namespace tool {
namespace calibrate {

CalibrationModule::CalibrationModule(QWidget *parent) :
    QMainWindow(parent),
    central(this),
    goalie("Goalie Postition", this),
    center("Center Field Position", this),
    rollBox(this),
    pitchBox(this),
    rollLabel(tr("Roll")),
    pitchLabel(tr("Pitch"))
{
    images.addTab(&topImage, "TOP");
    images.addTab(&bottomImage, "BOTTOM");

    layout.addWidget(&images, 0, 0, 4, 1);
    layout.addWidget(&goalie, 0, 1);
    layout.addWidget(&center, 1, 1);
    layout.addWidget(&rollBox, 2, 1);
    layout.addWidget(&rollLabel, 2, 2);
    layout.addWidget(&pitchBox, 3, 1);
    layout.addWidget(&pitchLabel, 3, 2);

    central.setLayout(&layout);
    setCentralWidget(&central);
}

void CalibrationModule::run_()
{
    jointsIn.latch();
    inertialIn.latch();

    LineVector lines = vision.getExpectedLines(currentCamera,
                                               jointsIn.message(),
                                               inertialIn.message(),
                                               currentX,
                                               currentY,
                                               currentH);
}

void CalibrationModule::imageTabSwitched(int i)
{
    if (i == images.indexOf(&topImage))
    {
        currentCamera = Camera::TOP;
    }
    else
    {
        currentCamera = Camera::BOTTOM;
    }
}

QImage CalibrationModule::makeOverlay(LineVector expected)
{
    QImage lineImage(320, 240, QImage::Format_ARGB32);
    lineImage.fill(qRgba(0, 0, 0, 0));
    QPainter painter(&lineImage);
    painter.setPen(QColor(246, 15, 15));

    for(LineVector::iterator i = expected.begin();
        i != expected.end(); i++)
    {
        painter.drawLine(lines->top_expected_line(i).start_x(),
                         lines->top_expected_line(i).start_y(),
                         lines->top_expected_line(i).end_x(),
                         lines->top_expected_line(i).end_y());
    }

    return lineImage;
}

}
}
