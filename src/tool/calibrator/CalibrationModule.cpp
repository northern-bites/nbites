#include "CalibrationModule.h"
#include <iostream>

namespace tool {
namespace calibrate {

CalibrationModule::CalibrationModule(QWidget *parent) :
    QMainWindow(parent),
    topImageIn(0),
    bottomImageIn(0),
    currentCamera(Camera::TOP),
    // Start with goalie position by default
    currentX(FIELD_WHITE_LEFT_SIDELINE_X),
    currentY(CENTER_FIELD_Y),
    currentH(HEADING_RIGHT),
    position(tr("Robot Postition Controls")),
    parameters(tr("Parameter Controls")),
    goalie("Goalie Postition", this),
    center("Center Field Position", this),
    other("Other Position", this),
    setX(this),
    setY(this),
    setH(this),
    xLabel("X Position", this),
    yLabel("Y Position", this),
    hLabel("Heading", this),
    rollBox(this),
    pitchBox(this),
    rollLabel(tr("Roll")),
    pitchLabel(tr("Pitch")),
    loadButton(tr("Load Calibration Values")),
    robotNames(this),
    central(this)
{
    images.addTab(&topImage, "TOP");
    images.addTab(&bottomImage, "BOTTOM");

    topImageIn = &topImage.imageIn;
    bottomImageIn = &bottomImage.imageIn;

    //image tabs
    layout.addWidget(&images, 0, 0, 15, 1);

    // position control
    layout.addWidget(&position, 0, 1);
    layout.addWidget(&goalie, 1, 1);
    layout.addWidget(&center, 2, 1);
    layout.addWidget(&other, 3, 1, 3, 1);
    layout.addWidget(&setX, 3, 2);
    layout.addWidget(&xLabel, 3, 3);
    layout.addWidget(&setY, 4, 2);
    layout.addWidget(&yLabel, 4, 3);
    layout.addWidget(&setH, 5, 2);
    layout.addWidget(&hLabel, 5, 3);

    goalie.setChecked(true);
    turnOffOtherPosition();

    setX.setRange(FIELD_GREEN_LEFT_SIDELINE_X,
                  FIELD_GREEN_RIGHT_SIDELINE_X);
    setY.setRange(FIELD_GREEN_BOTTOM_SIDELINE_Y,
                  FIELD_GREEN_TOP_SIDELINE_Y);
    setH.setRange(-HEADING_LEFT,
                  HEADING_LEFT);

    // roll/pitch correction control
    layout.addWidget(&parameters, 6, 1);
    layout.addWidget(&rollBox, 7, 1);
    layout.addWidget(&rollLabel, 7, 2);
    layout.addWidget(&pitchBox, 8, 1);
    layout.addWidget(&pitchLabel, 8, 2);

    // robot selection
    layout.addWidget(&loadButton, 9, 1);
    layout.addWidget(&robotNames, 9, 2);

    robotNames.addItem("");
    robotNames.addItem("river");
    robotNames.addItem("mal");
    robotNames.addItem("wash");
    robotNames.addItem("zoe");
    robotNames.addItem("jayne");
    robotNames.addItem("inara");
    robotNames.addItem("vera");
    robotNames.addItem("simon");
    robotNames.addItem("kaylee");

    rollBox.setSingleStep(0.01);
    pitchBox.setSingleStep(0.01);
    rollBox.setRange(-2.0, 2.0);
    pitchBox.setRange(-2.0, 2.0);

    central.setLayout(&layout);
    setCentralWidget(&central);

    connect(&goalie, SIGNAL(toggled(bool)),
            this, SLOT(useGoaliePosition(bool)));
    connect(&center, SIGNAL(toggled(bool)),
            this, SLOT(useCenterPosition(bool)));
    connect(&other, SIGNAL(toggled(bool)),
            this, SLOT(useOtherPosition(bool)));
    connect(&images, SIGNAL(currentChanged(int)),
            this, SLOT(switchCamera()));
    connect(&loadButton, SIGNAL(clicked(bool)),
            this, SLOT(loadRobotParameters()));
    connect(&pitchBox, SIGNAL(valueChanged(double)),
            this, SLOT(updateParameters()));
    connect(&rollBox, SIGNAL(valueChanged(double)),
            this, SLOT(updateParameters()));
}

void CalibrationModule::switchCamera()
{
    if (images.currentWidget() == &topImage)
    {
        currentCamera = Camera::TOP;
    }
    else
    {
        currentCamera = Camera::BOTTOM;
    }

    float tmpPitch, tmpRoll;

    tmpPitch = pitchBox.value();
    tmpRoll = rollBox.value();

    pitchBox.setValue(backupPitch);
    rollBox.setValue(backupRoll);

    backupPitch = tmpPitch;
    backupRoll = tmpRoll;

    updateOverlay();
}

void CalibrationModule::useGoaliePosition(bool checked)
{
    if (!checked) return;

    currentX = FIELD_WHITE_LEFT_SIDELINE_X;
    currentY = CENTER_FIELD_Y;
    currentH = HEADING_RIGHT;

    turnOffOtherPosition();

    updateOverlay();
}

void CalibrationModule::useCenterPosition(bool checked)
{
    if (!checked) return;

    currentX = CENTER_FIELD_X;
    currentY = CENTER_FIELD_Y;
    currentH = HEADING_RIGHT;

    turnOffOtherPosition();

    updateOverlay();
}

void CalibrationModule::useOtherPosition(bool checked)
{
    if (!checked) return;

    currentX = setX.value();
    currentY = setY.value();
    currentH = setH.value();

    turnOnOtherPosition();

    updateOverlay();
}

void CalibrationModule::useNewXValue(int value)
{
    if (!other.isChecked()) return;

    currentX = value;

    updateOverlay();
}

void CalibrationModule::useNewYValue(int value)
{
    if (!other.isChecked()) return;

    currentY = value;

    updateOverlay();
}

void CalibrationModule::useNewHValue(int value)
{
    if (!other.isChecked()) return;

    currentH = value;

    updateOverlay();
}

void CalibrationModule::loadRobotParameters()
{
    std::string name = robotNames.currentText().toStdString();
    CameraCalibrate::UpdateByName(name);

    std::cout << "Loaded parameters for " << name << "!" << std::endl;

    float* params = CameraCalibrate::getCurrentParameters(currentCamera);
    rollBox.setValue(params[CameraCalibrate::ROLL]);
    pitchBox.setValue(params[CameraCalibrate::PITCH]);

    updateOverlay();
}

void CalibrationModule::updateParameters()
{
    float paramsTop[CameraCalibrate::NUM_PARAMS];
    float paramsBottom[CameraCalibrate::NUM_PARAMS];

    if (currentCamera == Camera::TOP)
    {
        paramsTop[CameraCalibrate::ROLL] = rollBox.value();
        paramsTop[CameraCalibrate::PITCH] = pitchBox.value();
        paramsBottom[CameraCalibrate::ROLL] = backupRoll;
        paramsBottom[CameraCalibrate::PITCH] = backupPitch;
    }
    else
    {
        paramsBottom[CameraCalibrate::ROLL] = rollBox.value();
        paramsBottom[CameraCalibrate::PITCH] = pitchBox.value();
        paramsTop[CameraCalibrate::ROLL] = backupRoll;
        paramsTop[CameraCalibrate::PITCH] = backupPitch;
    }

    CameraCalibrate::UpdateWithParams(paramsTop, paramsBottom);

    updateOverlay();
}

void CalibrationModule::turnOffOtherPosition()
{
    disconnect(&setX, SIGNAL(valueChanged(int)),
               this, SLOT(useNewXValue(int)));
    disconnect(&setY, SIGNAL(valueChanged(int)),
               this, SLOT(useNewYValue(int)));
    disconnect(&setH, SIGNAL(valueChanged(int)),
               this, SLOT(useNewHValue(int)));

    setX.setEnabled(false);
    setY.setEnabled(false);
    setH.setEnabled(false);
}

void CalibrationModule::turnOnOtherPosition()
{
    connect(&setX, SIGNAL(valueChanged(int)),
               this, SLOT(useNewXValue(int)));
    connect(&setY, SIGNAL(valueChanged(int)),
               this, SLOT(useNewYValue(int)));
    connect(&setH, SIGNAL(valueChanged(int)),
               this, SLOT(useNewHValue(int)));

    setX.setEnabled(true);
    setY.setEnabled(true);
    setH.setEnabled(true);
}

void CalibrationModule::run_()
{
    jointsIn.latch();
    inertialIn.latch();

    updateOverlay();
}

void CalibrationModule::updateOverlay()
{
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
        topImage.reset();
    }
    else
    {
        bottomImage.setOverlay(makeOverlay(lines));
        bottomImage.run();
        bottomImage.reset();
    }
}

QImage CalibrationModule::makeOverlay(LineVector& expected)
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
