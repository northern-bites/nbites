#pragma once

#include "PMotion.pb.h"

namespace Kinematics {
/// Joint Name constants ///
enum JointName {
    HEAD_YAW = 0,
    HEAD_PITCH,
    // LARM,
    L_SHOULDER_PITCH,
    L_SHOULDER_ROLL,
    L_ELBOW_YAW,
    L_ELBOW_ROLL,
    // LLEG,
    L_HIP_YAW_PITCH,
    L_HIP_ROLL,
    L_HIP_PITCH,
    L_KNEE_PITCH,
    L_ANKLE_PITCH,
    L_ANKLE_ROLL,
    // RLEG,
    R_HIP_YAW_PITCH,
    R_HIP_ROLL,
    R_HIP_PITCH,
    R_KNEE_PITCH,
    R_ANKLE_PITCH,
    R_ANKLE_ROLL,
    // RARM,
    R_SHOULDER_PITCH,
    R_SHOULDER_ROLL,
    R_ELBOW_YAW,
    R_ELBOW_ROLL
};

/**
 * @brief Converts a JointAngles message to a vector of 
 *        float joint angle values indexed according to
 *        the Kinematics::JointName indices.
 * 
 * @return A vector of joint angles.
 */
static std::vector<float> toJointAngles(const messages::JointAngles& ja)
{
    std::vector<float> result(R_ELBOW_ROLL + 1, 0.0f);
    result[HEAD_YAW] = ja.head_yaw();
    result[HEAD_PITCH] = ja.head_pitch();
    result[L_SHOULDER_PITCH] = ja.l_shoulder_pitch();
    result[L_SHOULDER_ROLL] = ja.l_shoulder_roll();
    result[L_ELBOW_YAW] = ja.l_elbow_yaw();
    result[L_ELBOW_ROLL] = ja.l_elbow_roll();
    result[L_HIP_YAW_PITCH] = ja.l_hip_yaw_pitch();
    result[L_HIP_ROLL] = ja.l_hip_roll();
    result[L_HIP_PITCH] = ja.l_hip_pitch();
    result[L_KNEE_PITCH] = ja.l_knee_pitch();

    result[L_ANKLE_PITCH] = ja.l_ankle_pitch();
    result[L_ANKLE_ROLL] = ja.l_ankle_roll();
    result[R_HIP_YAW_PITCH] = ja.r_hip_yaw_pitch();
    result[R_HIP_ROLL] = ja.r_hip_roll();
    result[R_HIP_PITCH] = ja.r_hip_pitch();
    result[R_KNEE_PITCH] = ja.r_knee_pitch();
    result[R_ANKLE_PITCH] = ja.r_ankle_pitch();
    result[R_ANKLE_ROLL] = ja.r_ankle_roll();
    result[R_SHOULDER_PITCH] = ja.r_shoulder_pitch();
    result[R_SHOULDER_ROLL] = ja.r_shoulder_roll();
    result[R_ELBOW_YAW] = ja.r_elbow_yaw();
    result[R_ELBOW_ROLL] = ja.r_elbow_roll();

    return result;
}
}
