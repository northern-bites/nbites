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
    enabled(tr("Load A Log!")),
    position(tr("Robot Position Controls")),
    parameters(tr("Parameter Controls")),
    goalie("Goalie Position", this),
    center("Center Field Position", this),
    cross("Field Cross Position", this),
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
    side(this),
    central(this)
{
    images.addTab(&topImage, "TOP");
    images.addTab(&bottomImage, "BOTTOM");

    topImageIn = &topImage.imageIn;
    bottomImageIn = &bottomImage.imageIn;

    //image tabs
    mainLayout.addWidget(&images);

    for (int i = 0; i < 4; i++)
    {
        divider[i].setFrameShape(QFrame::HLine);
        divider[i].setFrameShadow(QFrame::Sunken);
    }

    // position control
    sideLayout.addWidget(&enabled, 0, 0);
    sideLayout.addWidget(&divider[0], 1, 0, 1, 3);
    sideLayout.addWidget(&position, 2, 0);
    sideLayout.addWidget(&divider[1], 3, 0, 1, 3);
    sideLayout.addWidget(&goalie, 4, 0);
    sideLayout.addWidget(&center, 5, 0);
    sideLayout.addWidget(&cross, 6, 0);
    sideLayout.addWidget(&other, 7, 0, 3, 1);
    sideLayout.addWidget(&setX, 7, 1);
    sideLayout.addWidget(&xLabel, 7, 2);
    sideLayout.addWidget(&setY, 8, 1);
    sideLayout.addWidget(&yLabel, 8, 2);
    sideLayout.addWidget(&setH, 9, 1);
    sideLayout.addWidget(&hLabel, 9, 2);

    goalie.setChecked(true);
    turnOffOtherPosition();

    setX.setRange(FIELD_GREEN_LEFT_SIDELINE_X,
                  FIELD_GREEN_RIGHT_SIDELINE_X);
    setY.setRange(FIELD_GREEN_BOTTOM_SIDELINE_Y,
                  FIELD_GREEN_TOP_SIDELINE_Y);
    setH.setRange(-HEADING_LEFT,
                  HEADING_LEFT);

    // roll/pitch correction control
    sideLayout.addWidget(&divider[2], 10, 0, 1, 3);
    sideLayout.addWidget(&parameters, 11, 0);
    sideLayout.addWidget(&divider[3], 12, 0, 1, 3);
    sideLayout.addWidget(&rollBox, 13, 0);
    sideLayout.addWidget(&rollLabel, 13, 1);
    sideLayout.addWidget(&pitchBox, 14, 0);
    sideLayout.addWidget(&pitchLabel, 14, 1);

    // robot selection
    sideLayout.addWidget(&loadButton, 15, 0);
    sideLayout.addWidget(&robotNames, 15, 1);

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

    side.setLayout(&sideLayout);
    side.setMaximumHeight(380);

    mainLayout.addWidget(&side, 0, Qt::AlignTop);

    central.setLayout(&mainLayout);
    setCentralWidget(&central);

    connect(&goalie, SIGNAL(toggled(bool)),
            this, SLOT(useGoaliePosition(bool)));
    connect(&center, SIGNAL(toggled(bool)),
            this, SLOT(useCenterPosition(bool)));
    connect(&cross, SIGNAL(toggled(bool)),
            this, SLOT(useCrossPosition(bool)));
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

void CalibrationModule::enableTopImage(bool use)
{
    images.setTabEnabled(images.indexOf(&topImage), use);
}

void CalibrationModule::enableBottomImage(bool use)
{
    images.setTabEnabled(images.indexOf(&bottomImage), use);
}

void CalibrationModule::enableCalibration(bool use)
{
    side.setEnabled(use);

    if (use)
    {
        enabled.setText(tr("Calibrating!"));
    }
    else
    {
        enabled.setText(tr("DISABLED - Missing joints or inertials"));
    }
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

void CalibrationModule::useCrossPosition(bool checked)
{
    if (!checked) return;

    currentX = LANDMARK_YELLOW_GOAL_CROSS_X;
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
    disconnect(&pitchBox, SIGNAL(valueChanged(double)),
               this, SLOT(updateParameters()));
    disconnect(&rollBox, SIGNAL(valueChanged(double)),
               this, SLOT(updateParameters()));

    std::string name = robotNames.currentText().toStdString();
    CameraCalibrate::UpdateByName(name);

    float* params = CameraCalibrate::getCurrentParameters(currentCamera);

    rollBox.setValue(params[CameraCalibrate::ROLL]);
    pitchBox.setValue(params[CameraCalibrate::PITCH]);

    updateOverlay();

    connect(&pitchBox, SIGNAL(valueChanged(double)),
            this, SLOT(updateParameters()));
    connect(&rollBox, SIGNAL(valueChanged(double)),
            this, SLOT(updateParameters()));
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
    if (side.isEnabled())
    {
        jointsIn.latch();
        inertialIn.latch();
    }

    updateOverlay();
}

void CalibrationModule::updateOverlay()
{
    LineVector lines;

    if (side.isEnabled())
    {
        lines = vision.getExpectedLines(currentCamera,
                                        jointsIn.message(),
                                        inertialIn.message(),
                                        currentX,
                                        currentY,
                                        currentH*TO_RAD);
    }

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
