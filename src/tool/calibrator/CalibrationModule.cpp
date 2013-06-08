#include "CalibrationModule.h"
#include <iostream>

namespace tool {
namespace calibrate {

CalibrationModule::CalibrationModule(QWidget *parent) :
    QMainWindow(parent),
    topImageIn(0),
    bottomImageIn(0),
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

    topImageIn = &topImage.imageIn;
    bottomImageIn = &bottomImage.imageIn;

    connect(&images, SIGNAL(currentChanged(int)),
            this, SLOT(imageTabSwitched(int)));

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

    currentX = FIELD_WHITE_LEFT_SIDELINE_X;
    currentY = CENTER_FIELD_Y;
    currentH = HEADING_RIGHT;

    LineVector lines = vision.getExpectedLines(currentCamera,
                                               jointsIn.message(),
                                               inertialIn.message(),
                                               currentX,
                                               currentY,
                                               currentH);

    if (currentCamera == Camera::TOP)
    {
        topImage.setOverlay(makeOverlay(lines));
        topImage.run();
        std::cout << "Running TOP" << std::endl;
    }
    else
    {
        bottomImage.setOverlay(makeOverlay(lines));
        bottomImage.run();
        std::cout << "Running BOTTOM" << std::endl;
    }
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
        painter.drawLine((*i)->getStartpoint().x,
                         (*i)->getStartpoint().y,
                         (*i)->getEndpoint().x,
                         (*i)->getEndpoint().y);
    }

    return lineImage;
}

}
}
