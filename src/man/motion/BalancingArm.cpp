
#include <iostream>

#include "BalancingArm.h"

BalancingArm::BalancingArm(Kinematics::ChainID id, COMPreview::ptr com)
    : chain(id),
      comPreview(com)
{
    id == Kinematics::LARM_CHAIN ? leftArm = true: leftArm = false;
}

BalancingArm::~BalancingArm() {

}

// arm joint order: S_Pitch(Y), S_Roll(Z), Elbow_Yaw(X), E_Roll(Z)
std::vector<float> BalancingArm::getNextJoints() {
    std::vector<float> angles;
    for (int id = 0; id <= ELBOW_ROLL; ++id)
	angles.push_back(getJoint(id));

    return angles;
}

/**
 *
 */
float BalancingArm::shoulderPitch() {
    return static_cast<float>(5000 * comPreview->DX());
}

float BalancingArm::shoulderRoll() {
    return 0.0f; // temporary
}

/**
 * controls the twist of the wrist joint, likely has little effect on stability
 */
float BalancingArm::elbowYaw() {
    return 0.0f;
}

/**
 * Stabilize L/R by bringing elbow roll in
 */
float BalancingArm::elbowRoll() {
    if (leftArm && comPreview->Y() > 0)
	return -1.3f;
    else if  (!leftArm && comPreview->Y() < 0)
	return 1.3f;
    else
	return 0.0f;
}

float BalancingArm::getJoint(int id) {
    switch (id) {
    case SHOULDER_PITCH:
	return shoulderPitch();
    case SHOULDER_ROLL:
	return shoulderRoll();
    case ELBOW_YAW:
	return elbowYaw();
    case ELBOW_ROLL:
	return elbowRoll();
    default:
	return 0.0f;
    }
}
