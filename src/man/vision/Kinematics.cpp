#include "Kinematics.h"

#include "NBMath.h"

#include <iostream>

namespace man {
namespace vision {
    
Kinematics::Kinematics(bool topCamera)
    : needCompute(false),
      constants(topCamera),
      joints_(),
      tilt_(0),
      wz0_(0)
{}

void Kinematics::joints(const messages::JointAngles& newJoints)
{
    needCompute = true;
    joints_ = newJoints;
}

void Kinematics::compute()
{
    // Do not compute more than once
    if (!needCompute) return;
    needCompute = false;

    // Convert to radians
    double la = joints_.l_ankle_pitch();
    double lk = joints_.l_knee_pitch();
    double lh = joints_.l_hip_pitch();

    double ra = joints_.r_ankle_pitch();
    double rk = joints_.r_knee_pitch();
    double rh = joints_.r_hip_pitch();

    double h = joints_.head_pitch();

    // Compute kinematics for left and right leg
    std::pair<double, double> leftLeg = computeForLeg(la, lk, lh, h);
    std::pair<double, double> rightLeg = computeForLeg(ra, rk, rh, h);
    double leftTilt = leftLeg.first;
    double rightTilt = rightLeg.first;
    double leftCameraHeight = leftLeg.second;
    double rightCameraHeight = rightLeg.second;

    // Compute tilt and wz0 for whatever leg we are standing on
    if (leftCameraHeight > rightCameraHeight) {
        tilt_ = leftTilt;
        wz0_ = leftCameraHeight;
    } else {
        tilt_ = rightTilt;
        wz0_ = rightCameraHeight;
    }

    std::cout << "Tilt: " << tilt_*TO_DEG << std::endl;
    std::cout << "Height: " << wz0_ << std::endl << std::endl;
}

std::pair<double, double> Kinematics::computeForLeg(double anklePitch,
                                                    double kneePitch,
                                                    double hipPitch,
                                                    double neckPitch) const
{
    std::pair<double, double> tiltAndHeight;

    double tibiaPitch = anklePitch;
    double thighPitch = tibiaPitch + kneePitch;
    double torsoPitch = thighPitch + hipPitch;
    double headPitch = torsoPitch + neckPitch;

    double cameraTilt = 0.5 * M_PI - (headPitch + constants.opticalAxisOffset);
    double cameraHeight = constants.groundToAnkle +
                          cos(tibiaPitch) * constants.ankleToKnee +
                          cos(thighPitch) * constants.kneeToHip +
                          cos(torsoPitch) * constants.hipToNeck +
                          cos(headPitch) * constants.neckToCameraZ -
                          sin(headPitch) * constants.neckToCameraX;

    tiltAndHeight.first = cameraTilt;
    tiltAndHeight.second = cameraHeight;
    return tiltAndHeight;
}

}
}
