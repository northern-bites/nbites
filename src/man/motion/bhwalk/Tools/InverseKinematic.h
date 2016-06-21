/**
 * @file InverseKinematic.h
 * @author Alexander Härtl
 * @author jeff
 */

#pragma once

#include "Tools/Math/Vector2.h"
#include "Tools/Math/Pose3D.h"
#include "Representations/Configuration/RobotDimensions.h"
#include "Representations/Configuration/JointCalibration.h"
#include "Representations/Configuration/CameraCalibration.h"
#include "Tools/Range.h"
#include "Tools/Debugging/Asserts.h"

#include <iostream>


class InverseKinematic
{
public:
  /**
  * The method calculates the joint angles for the legs of the robot from a Pose3DBH for each leg
  * @param positionLeft The desired position (translation + rotation) of the left foots ankle point
  * @param positionRight The desired position (translation + rotation) of the right foots ankle point
  * @param jointData The instance of JointDataBH where the resulting joints are written into
  * @param robotDimensions The Robot Dimensions needed for calculation
  * @param ratio The ratio of
  * @return Whether the target position was reachable or not (if the given target position is not reachable the computation proceeds using the closest reachable position near the target)
  */
  static bool calcLegJointsOld(const Pose3DBH& positionLeft, const Pose3DBH& positionRight, JointDataBH& jointData, const RobotDimensionsBH& robotDimensions, float ratio = 0.5f)
  {
    bool reachable = true;
    if(!calcLegJoints(positionLeft, jointData, true, robotDimensions))
      reachable = false;
    if(!calcLegJoints(positionRight, jointData, false, robotDimensions))
      reachable = false;
    RangeBH<> clipping(0.0f, 1.0f);
    ratio = clipping.limit(ratio);
    // the hip joints of both legs must be equal, so it is computed as weighted mean and the foot positions are
    // recomputed with fixed joint0 and left open foot rotation (as possible failure)
    float joint0 = jointData.angles[JointDataBH::LHipYawPitch] * ratio + jointData.angles[JointDataBH::RHipYawPitch] * (1 - ratio);
    if(!calcLegJoints(positionLeft, jointData, joint0, true, robotDimensions))
      reachable = false;
    if(!calcLegJoints(positionRight, jointData, joint0, false, robotDimensions))
      reachable = false;
    return reachable;
  }

  /**
  * This method is an optimized version \c calcLegJointsOld.
  * It calculates the joint angles for the legs of the robot from a Pose3DBH for each leg.
  * @param positionLeft The desired position (translation + rotation) of the left foots ankle point
  * @param positionRight The desired position (translation + rotation) of the right foots ankle point
  * @param jointData The instance of JointDataBH where the resulting joints are written into
  * @param robotDimensions The Robot Dimensions needed for calculation
  * @param ratio The ratio of
  * @return Whether the target position was reachable or not (if the given target position is not reachable the computation proceeds using the closest reachable position near the target)
  */
  static bool calcLegJoints(const Pose3DBH& positionLeft, const Pose3DBH& positionRight, JointDataBH& jointData, const RobotDimensionsBH& robotDimensions, float ratio = 0.5f)
  {
    static const RotationMatrixBH rotPi_4 = RotationMatrixBH::fromRotationX(pi_4);
    static const RotationMatrixBH rotMinusPi_4 = RotationMatrixBH::fromRotationX(-pi_4);
    static const RangeBH<> ratioClipping(0.0f, 1.0f);
    static const RangeBH<> cosClipping(-1.0f, 1.0f);

    ratio = ratioClipping.limit(ratio);

    const Pose3DBH lTarget0 = Pose3DBH(rotMinusPi_4).translate(0.f, robotDimensions.lengthBetweenLegs * -0.5f, 0.f).conc(positionLeft);
    const Pose3DBH rTarget0 = Pose3DBH(rotPi_4).translate(0.f, robotDimensions.lengthBetweenLegs * 0.5f, 0.f).conc(positionRight);
    const Vector3BH<> lFootToHip = lTarget0.rotation.transpose() * (-lTarget0.translation);
    const Vector3BH<> rFootToHip = rTarget0.rotation.transpose() * (-rTarget0.translation);
    const float lMinusJoint5 = std::atan2(lFootToHip.y, lFootToHip.z);
    const float rJoint5 = std::atan2(rFootToHip.y, rFootToHip.z);
    const float lMinusBetaAndJoint4 = -std::atan2(lFootToHip.x, std::sqrt(sqrBH(lFootToHip.y) + sqrBH(lFootToHip.z)));
    const float rMinusBetaAndJoint4 = -std::atan2(rFootToHip.x, std::sqrt(sqrBH(rFootToHip.y) + sqrBH(rFootToHip.z)));
    const Vector3BH<> lHipRotationC1 = lTarget0.rotation * RotationMatrixBH::fromRotationX(-lMinusJoint5).rotateY(-lMinusBetaAndJoint4).c1;
    const Vector3BH<> rHipRotationC1 = rTarget0.rotation * RotationMatrixBH::fromRotationX(-rJoint5).rotateY(-rMinusBetaAndJoint4).c1;
    const float lMinusJoint0 = std::atan2(-lHipRotationC1.x, lHipRotationC1.y);
    const float rJoint0 = std::atan2(-rHipRotationC1.x, rHipRotationC1.y);
    const float lJoint0Combined = -lMinusJoint0 * ratio + rJoint0 * (1.f - ratio);

    const Pose3DBH lTarget1 = Pose3DBH(RotationMatrixBH::fromRotationZ(lJoint0Combined)).conc(lTarget0);
    const Pose3DBH rTarget1 = Pose3DBH(RotationMatrixBH::fromRotationZ(-lJoint0Combined)).conc(rTarget0);
    const Vector3BH<>& lHipToFoot = lTarget1.translation;
    const Vector3BH<>& rHipToFoot = rTarget1.translation;
    const float lMinusPi_4MinusJoint1 = -std::atan2(-lHipToFoot.y, -lHipToFoot.z);
    const float rPi_4AndJoint1 = -std::atan2(-rHipToFoot.y, -rHipToFoot.z);
    const float lJoint2MinusAlpha = std::atan2(-lHipToFoot.x, std::sqrt(sqrBH(lHipToFoot.y) + sqrBH(lHipToFoot.z)) * -sgnBH(lHipToFoot.z));
    const float rJoint2MinusAlpha = std::atan2(-rHipToFoot.x, std::sqrt(sqrBH(rHipToFoot.y) + sqrBH(rHipToFoot.z)) * -sgnBH(rHipToFoot.z));
    const Vector3BH<> lFootRotationC2 = RotationMatrixBH::fromRotationY(-lJoint2MinusAlpha).rotateX(-lMinusPi_4MinusJoint1) * lTarget1.rotation.c2;
    const Vector3BH<> rFootRotationC2 = RotationMatrixBH::fromRotationY(-rJoint2MinusAlpha).rotateX(-rPi_4AndJoint1) * rTarget1.rotation.c2;
    const float h1 = robotDimensions.upperLegLength;
    const float h2 = robotDimensions.lowerLegLength;
    const float hl = lTarget1.translation.abs();
    const float hr = rTarget1.translation.abs();
    const float h1Sqr = h1 * h1;
    const float h2Sqr = h2 * h2;
    const float hlSqr = hl * hl;
    const float hrSqr = hr * hr;
    const float lCosMinusAlpha = (h1Sqr + hlSqr - h2Sqr) / (2.f * h1 * hl);
    const float rCosMinusAlpha = (h1Sqr + hrSqr - h2Sqr) / (2.f * h1 * hr);
    const float lCosMinusBeta = (h2Sqr + hlSqr - h1Sqr) / (2.f * h2 * hl);
    const float rCosMinusBeta = (h2Sqr + hrSqr - h1Sqr) / (2.f * h2 * hr);
    const float lAlpha = -std::acos(cosClipping.limit(lCosMinusAlpha));
    const float rAlpha = -std::acos(cosClipping.limit(rCosMinusAlpha));
    const float lBeta = -std::acos(cosClipping.limit(lCosMinusBeta));
    const float rBeta = -std::acos(cosClipping.limit(rCosMinusBeta));

    jointData.angles[JointDataBH::LHipYawPitch + 0] = lJoint0Combined;
    jointData.angles[JointDataBH::LHipYawPitch + 1] = -(lMinusPi_4MinusJoint1 + pi_4);
    jointData.angles[JointDataBH::LHipYawPitch + 2] = lJoint2MinusAlpha + lAlpha;
    jointData.angles[JointDataBH::LHipYawPitch + 3] = -lAlpha - lBeta;
    jointData.angles[JointDataBH::LHipYawPitch + 4] = std::atan2(lFootRotationC2.x, lFootRotationC2.z) + lBeta;
    jointData.angles[JointDataBH::LHipYawPitch + 5] = -std::asin(-lFootRotationC2.y);

    jointData.angles[JointDataBH::RHipYawPitch + 0] = lJoint0Combined;
    jointData.angles[JointDataBH::RHipYawPitch + 1] = rPi_4AndJoint1 - pi_4;
    jointData.angles[JointDataBH::RHipYawPitch + 2] = rJoint2MinusAlpha + rAlpha;
    jointData.angles[JointDataBH::RHipYawPitch + 3] = -rAlpha - rBeta;
    jointData.angles[JointDataBH::RHipYawPitch + 4] = std::atan2(rFootRotationC2.x, rFootRotationC2.z) + rBeta;
    jointData.angles[JointDataBH::RHipYawPitch + 5] = std::asin(-rFootRotationC2.y);
    /*
    // test foot joint angles to reach hip
    Vector3BH<> lFit = RotationMatrixBH::fromRotationX(-lMinusJoint5) * RotationMatrixBH::fromRotationY(-lMinusBetaAndJoint4) * Vector3BH<>(0.f, 0.f, lFootToHip.abs()) - lFootToHip;
    Vector3BH<> rFit = RotationMatrixBH::fromRotationX(-rJoint5) * RotationMatrixBH::fromRotationY(-rMinusBetaAndJoint4) * Vector3BH<>(0.f, 0.f, rFootToHip.abs()) - rFootToHip;
    ASSERT(abs(lFit.x) < 0.01f && abs(lFit.y) < 0.01f && abs(lFit.z) < 0.01f);
    ASSERT(abs(rFit.x) < 0.01f && abs(rFit.y) < 0.01f && abs(rFit.z) < 0.01f);

    // test hip joint angles to reach target position
    Vector3BH<> lFit2 = RotationMatrixBH::fromRotationX(lMinusPi_4MinusJoint1) * RotationMatrixBH::fromRotationY(lJoint2MinusAlpha) * Vector3BH<>(0.f, 0.f, -lHipToFoot.abs()) - lHipToFoot;
    Vector3BH<> rFit2 = RotationMatrixBH::fromRotationX(rPi_4AndJoint1) * RotationMatrixBH::fromRotationY(rJoint2MinusAlpha) * Vector3BH<>(0.f, 0.f, -rHipToFoot.abs()) - rHipToFoot;
    ASSERT(abs(lFit2.x) < 0.01f && abs(lFit2.y) < 0.01f && abs(lFit2.z) < 0.01f);
    ASSERT(abs(rFit2.x) < 0.01f && abs(rFit2.y) < 0.01f && abs(rFit2.z) < 0.01f);

    // compare resulting joint angles with ali's implementation
    JointDataBH testJointData;
    bool reachable = true;
    if(!calcLegJoints(positionLeft, testJointData, true, robotDimensions))
      reachable = false;
    if(!calcLegJoints(positionRight, testJointData, false, robotDimensions))
      reachable = false;
    float testLJoint0 = testJointData.angles[JointDataBH::LHipYawPitch + 0];
    float testRJoint0 = testJointData.angles[JointDataBH::RHipYawPitch + 0];
    float joint0 = testJointData.angles[JointDataBH::LHipYawPitch] * ratio + testJointData.angles[JointDataBH::RHipYawPitch] * (1 - ratio);
    if(!calcLegJoints(positionLeft, testJointData, joint0, true, robotDimensions))
      reachable = false;
    if(!calcLegJoints(positionRight, testJointData, joint0, false, robotDimensions))
      reachable = false;
    for(int i = 0; i <= 5; ++i)
    {
      ASSERT(abs(jointData.angles[JointDataBH::LHipYawPitch + i] - testJointData.angles[JointDataBH::LHipYawPitch + i]) < 0.01f);
      ASSERT(abs(jointData.angles[JointDataBH::RHipYawPitch + i] - testJointData.angles[JointDataBH::RHipYawPitch + i]) < 0.01f);
    }

    // compare resulting poses with requested pose
    const RotationMatrixBH lFootRotation = RotationMatrixBH::fromRotationY(-lJoint2MinusAlpha).rotateX(-lMinusPi_4MinusJoint1) * lTarget1.rotation;
    const RotationMatrixBH rFootRotation = RotationMatrixBH::fromRotationY(-rJoint2MinusAlpha).rotateX(-rPi_4AndJoint1) * rTarget1.rotation;
    float lFailure = -atan2(lFootRotation.c0.y, lFootRotation.c1.y);
    float rFailure = atan2(rFootRotation.c0.y, rFootRotation.c1.y);
    MassCalibrationBH massCali;
    RobotModelBH testRM(jointData, robotDimensions, massCali);
    Vector3BH<> lFit3 = testRM.limbs[MassCalibrationBH::footLeft].translation - positionLeft.translation;
    Vector3BH<> lFit4 = RotationMatrixBH(testRM.limbs[MassCalibrationBH::footLeft].rotation).rotateZ(-lFailure).getAngleAxis() - positionLeft.rotation.getAngleAxis();
    ASSERT(!reachable || (abs(lFit3.x) < 0.01f && abs(lFit3.y) < 0.01f && abs(lFit3.z) < 0.01f));
    ASSERT(abs(lFit4.x) < 0.01f && abs(lFit4.y) < 0.01f && abs(lFit4.z) < 0.01f);
    Vector3BH<> rFit3 = testRM.limbs[MassCalibrationBH::footRight].translation - positionRight.translation;
    Vector3BH<> rFit4 = RotationMatrixBH(testRM.limbs[MassCalibrationBH::footRight].rotation).rotateZ(rFailure).getAngleAxis() - positionRight.rotation.getAngleAxis();
    ASSERT(!reachable || (abs(rFit3.x) < 0.01f && abs(rFit3.y) < 0.01f && abs(rFit3.z) < 0.01f));
    ASSERT(abs(rFit4.x) < 0.01f && abs(rFit4.y) < 0.01f && abs(rFit4.z) < 0.01f);
    */
    const float maxLen = h1 + h2;
    return hl < maxLen && hr < maxLen;
  }

  /**
  * The method calculates the joint angles of one leg of the robot from a Pose3DBH
  * @param position The desired position (translation + rotation) of the foots ankle point
  * @param jointData The instance of JointDataBH where the resulting joints are written into
  * @param left Determines if the left or right leg is calculated
  * @param robotDimensions The Robot Dimensions needed for calculation
  * @return Whether the target position was reachable or not (if the given target position is not reachable the computation proceeds using the closest reachable position near the target)
  */
  static bool calcLegJoints(const Pose3DBH& position, JointDataBH& jointData, bool left, const RobotDimensionsBH& robotDimensions)
  {
    Pose3DBH target(position);
    JointDataBH::Joint firstJoint(left ? JointDataBH::LHipYawPitch : JointDataBH::RHipYawPitch);
    int sign(left ? -1 : 1);
    target.translation.y += robotDimensions.lengthBetweenLegs / 2.f * sign; // translate to origin of leg
    // rotate by 45° around origin for Nao
    // calculating sqrt(2) is faster than calculating the resp. rotation matrix with getRotationX()
    static const float sqrt2_2 = std::sqrt(2.0f) * 0.5f;
    RotationMatrixBH rotationX_pi_4 = RotationMatrixBH(Vector3BH<>(1, 0, 0), Vector3BH<>(0, sqrt2_2, sqrt2_2 * sign), Vector3BH<>(0, sqrt2_2 * -sign, sqrt2_2));
    target.translation = rotationX_pi_4 * target.translation;
    target.rotation = rotationX_pi_4 * target.rotation;

    target = target.invert(); // invert pose to get position of body relative to foot

    // use geometrical solution to compute last three joints
    float length = target.translation.abs();
    float sqrLength = length * length;
    float upperLeg = robotDimensions.upperLegLength;
    float sqrUpperLeg = upperLeg * upperLeg;
    float lowerLeg = robotDimensions.lowerLegLength;
    float sqrLowerLeg = lowerLeg * lowerLeg;
    float cosLowerLeg = (sqrLowerLeg + sqrLength - sqrUpperLeg) / (2 * lowerLeg * length);
    float cosKnee = (sqrUpperLeg + sqrLowerLeg - sqrLength) / (2 * upperLeg * lowerLeg);

    // clip for the case of unreachable position
    const RangeBH<> clipping(-1.0f, 1.0f);
    bool reachable = true;
    if(!clipping.isInside(cosKnee) || clipping.isInside(cosLowerLeg))
    {
      cosKnee = clipping.limit(cosKnee);
      cosLowerLeg = clipping.limit(cosLowerLeg);
      reachable = false;
    }
    float joint3 = pi - std::acos(cosKnee); // implicitly solve discrete ambiguousness (knee always moves forward)
    float joint4 = -std::acos(cosLowerLeg);
    joint4 -= std::atan2(target.translation.x, Vector2BH<>(target.translation.y, target.translation.z).abs());
    float joint5 = std::atan2(target.translation.y, target.translation.z) * sign;

    // calulate rotation matrix before hip joints
    RotationMatrixBH hipFromFoot;
    hipFromFoot.rotateX(joint5 * -sign);
    hipFromFoot.rotateY(-joint4 - joint3);

    // compute rotation matrix for hip from rotation before hip and desired rotation
    RotationMatrixBH hip = hipFromFoot.invert() * target.rotation;

    // compute joints from rotation matrix using theorem of euler angles
    // see http://www.geometrictools.com/Documentation/EulerAngles.pdf
    // this is possible because of the known order of joints (z, x, y seen from body resp. y, x, z seen from foot)
    float joint1 = std::asin(-hip[2].y) * -sign;
    joint1 -= pi_4; // because of the 45°-rotational construction of the Nao legs
    float joint2 = -std::atan2(hip[2].x, hip[2].z);
    float joint0 = std::atan2(hip[0].y, hip[1].y) * -sign;

    // set computed joints in jointData
    jointData.angles[firstJoint + 0] = joint0;
    jointData.angles[firstJoint + 1] = joint1;
    jointData.angles[firstJoint + 2] = joint2;
    jointData.angles[firstJoint + 3] = joint3;
    jointData.angles[firstJoint + 4] = joint4;
    jointData.angles[firstJoint + 5] = joint5;

    return reachable;
  }

  /**
  * The method calculates the joint angles of one leg of the Nao from a Pose3DBH with a fixed first joint
  * This is necessary because the Nao has mechanically connected hip joints, hence not every
  * combination of foot positions can be reached and has to be recalculated with equal joint0 for both legs
  * the rotation of the foot around the z-axis through the ankle-point is left open as "failure"
  * @param position The desired position (translation + rotation) of the foots ankle point
  * @param jointData The instance of JointDataBH where the resulting joints are written into
  * @param joint0 Fixed value for joint0 of the respective leg
  * @param left Determines if the left or right leg is calculated
  * @param robotDimensions The Robot Dimensions needed for calculation
  * @return Whether the target position was reachable or not (if the given target position is not reachable the computation proceeds using the closest reachable position near the target)
  */
  static bool calcLegJoints(const Pose3DBH& position, JointDataBH& jointData, float joint0, bool left, const RobotDimensionsBH& robotDimensions)
  {
    Pose3DBH target(position);
    JointDataBH::Joint firstJoint(left ? JointDataBH::LHipYawPitch : JointDataBH::RHipYawPitch);
    const int sign(left ? -1 : 1);
    target.translation.y += robotDimensions.lengthBetweenLegs / 2 * sign; // translate to origin of leg
    target = Pose3DBH().rotateZ(joint0 * -sign).rotateX(pi_4 * sign).conc(target); // compute residual transformation with fixed joint0

    // use cosine theorem and arctan to compute first three joints
    float length = target.translation.abs();
    float sqrLength = length * length;
    float upperLeg = robotDimensions.upperLegLength;
    float sqrUpperLeg = upperLeg * upperLeg;
    float lowerLeg = robotDimensions.lowerLegLength;
    float sqrLowerLeg = lowerLeg * lowerLeg;
    float cosUpperLeg = (sqrUpperLeg + sqrLength - sqrLowerLeg) / (2 * upperLeg * length);
    float cosKnee = (sqrUpperLeg + sqrLowerLeg - sqrLength) / (2 * upperLeg * lowerLeg);
    // clip for the case that target position is not reachable
    const RangeBH<> clipping(-1.0f, 1.0f);
    bool reachable = true;
    if(!clipping.isInside(cosKnee) || clipping.isInside(upperLeg))
    {
      cosKnee = clipping.limit(cosKnee);
      cosUpperLeg = clipping.limit(cosUpperLeg);
      reachable = false;
    }
    float joint1 = target.translation.z == 0.0f ? 0.0f : std::atan(target.translation.y / -target.translation.z) * sign;
    float joint2 = -std::acos(cosUpperLeg);
    joint2 -= std::atan2(target.translation.x, Vector2BH<>(target.translation.y, target.translation.z).abs() * -sgnBH(target.translation.z));
    float joint3 = pi - std::acos(cosKnee);
    RotationMatrixBH beforeFoot = RotationMatrixBH().rotateX(joint1 * sign).rotateY(joint2 + joint3);
    joint1 -= pi_4; // because of the strange hip of Nao

    // compute joints from rotation matrix using theorem of euler angles
    // see http://www.geometrictools.com/Documentation/EulerAngles.pdf
    // this is possible because of the known order of joints (y, x, z) where z is left open and is seen as failure
    RotationMatrixBH foot = beforeFoot.invert() * target.rotation;
    float joint5 = std::asin(-foot[2].y) * -sign * -1;
    float joint4 = -std::atan2(foot[2].x, foot[2].z) * -1;
    //float failure = atan2(foot[0].y, foot[1].y) * sign;

    // set computed joints in jointData
    jointData.angles[firstJoint + 0] = joint0;
    jointData.angles[firstJoint + 1] = joint1;
    jointData.angles[firstJoint + 2] = joint2;
    jointData.angles[firstJoint + 3] = joint3;
    jointData.angles[firstJoint + 4] = joint4;
    jointData.angles[firstJoint + 5] = joint5;

    return reachable;
  }

public:
  static bool calcArmJoints(const Pose3DBH& left, const Pose3DBH& right, JointDataBH& targetJointData, const RobotDimensionsBH& theRobotDimensionsBH, const JointCalibrationBH& theJointCalibrationBH)
  {
    const Vector3BH<> leftDir = left.rotation * Vector3BH<>(0, -1, 0),
                    rightDir = right.rotation * Vector3BH<>(0, 1, 0);

    //transform to "shoulder"-coordinate-system
    Vector3BH<> leftTarget = left.translation - Vector3BH<>(theRobotDimensionsBH.armOffset.x,
                           theRobotDimensionsBH.armOffset.y,
                           theRobotDimensionsBH.armOffset.z),
                           rightTarget = right.translation - Vector3BH<>(theRobotDimensionsBH.armOffset.x,
                                         -theRobotDimensionsBH.armOffset.y,
                                         theRobotDimensionsBH.armOffset.z);

    //avoid straigt arm
    static const float maxLength = (theRobotDimensionsBH.upperArmLength + theRobotDimensionsBH.lowerArmLength) * 0.9999f;
    if(leftTarget.squareAbs() >= sqrBH(maxLength))
      leftTarget.normalizeBH(maxLength);

    if(rightTarget.squareAbs() >= sqrBH(maxLength))
      rightTarget.normalizeBH(maxLength);

    bool res1, res2;
    res1 = calcArmJoints(leftTarget, leftDir, 1, targetJointData, theRobotDimensionsBH, theJointCalibrationBH);
    res2 = calcArmJoints(rightTarget, rightDir, -1, targetJointData, theRobotDimensionsBH, theJointCalibrationBH);

    return res1 && res2;
  }

  static bool calcArmJoints(Vector3BH<> target, Vector3BH<> targetDir, int side, JointDataBH& targetJointData, const RobotDimensionsBH& theRobotDimensionsBH, const JointCalibrationBH& theJointCalibrationBH)
  {
    //hacked mirror
    target.y *= (float)side;
    targetDir.y *= (float)side;

    const int offset = side == -1 ? JointDataBH::RShoulderPitch : JointDataBH::LShoulderPitch;

    Vector3BH<> elbow;
    if(!calcElbowPosition(target, targetDir, side, elbow, theRobotDimensionsBH, theJointCalibrationBH))
      return false;

    calcJointsForElbowPos(elbow, target, targetJointData, offset, theRobotDimensionsBH);

    return true;
  }

  static bool calcElbowPosition(Vector3BH<> &target, const Vector3BH<> &targetDir, int side, Vector3BH<> &elbow, const RobotDimensionsBH& theRobotDimensionsBH, const JointCalibrationBH& theJointCalibrationBH)
  {
    const Vector3BH<> M1(0, 0, 0); //shoulder
    const Vector3BH<> M2(target); //hand
    const float r1 = theRobotDimensionsBH.upperArmLength;
    const float r2 = theRobotDimensionsBH.lowerArmLength;
    const Vector3BH<> M12 = M2 - M1;

    Vector3BH<> n = target;
    n.normalizeBH();

    //center of intersection circle of spheres around shoulder and hand
    const Vector3BH<> M3 = M1 + M12 * ((sqrBH(r1) - sqrBH(r2)) / (2 * M12.squareAbs()) + 0.5f);

    //calculate radius of intersection circle
    const Vector3BH<> M23 = M3 - M2;
    float diff = sqrBH(r2) - M23.squareAbs();
    const float radius = std::sqrt(diff);

    //determine a point on the circle
    const bool specialCase = n.x == 1 && n.y == 0 && n.z == 0 ? true : false;
    const Vector3BH<> bla(specialCase ? 0.0f : 1.0f, specialCase ? 1.0f : 0.0f, 0.0f);
    const Vector3BH<> pointOnCircle = M3 + (n ^ bla).normalizeBH(radius);

    //find best point on circle
    float angleDiff = pi * 2.0f / 3.0f;
    Vector3BH<> bestMatch = pointOnCircle;
    float bestAngle = 0.0f;
    float newBestAngle = bestAngle;
    float bestQuality = -2.0f;

    Vector3BH<> tDir = targetDir;
    tDir.normalizeBH();

    const int offset = side == 1 ? 0 : 4;
    int iterationCounter = 0;
    const float maxAngleEpsilon = 1.0f * pi / 180.0f;
    while(2.0f * angleDiff > maxAngleEpsilon)
    {
      for(int i = -1; i <= 1; i++)
      {
        if(i == 0 && iterationCounter != 1)
          continue;

        iterationCounter++;

        const Pose3DBH elbowRotation(RotationMatrixBH(n, bestAngle + angleDiff * i));
        const Vector3BH<> possibleElbow = elbowRotation * pointOnCircle;
        const Vector3BH<> elbowDir = (M3 - possibleElbow).normalizeBH();
        float quality = elbowDir * tDir;
        if(quality > bestQuality)
        {
          bestQuality = quality;
          bestMatch = possibleElbow;
          newBestAngle = bestAngle + angleDiff * i;
        }
      }
      angleDiff /= 2.0f;
      bestAngle = newBestAngle;
    }
    //printf("iterations %d\n", iterationCounter);
    if(bestQuality == -2.0f)
      return false;

    //special case of target-out-of-joints-limit problem
    JointDataBH tAJR;
    calcJointsForElbowPos(bestMatch, target, tAJR, offset, theRobotDimensionsBH);

    const JointCalibrationBH::JointInfo ji = theJointCalibrationBH.joints[JointDataBH::LShoulderPitch + offset + 1];
    if(tAJR.angles[offset + 1] < ji.minAngle)
    {
      tAJR.angles[offset + 1] = ji.minAngle;
      Pose3DBH shoulder2Elbow;
      shoulder2Elbow.translate(0, -theRobotDimensionsBH.upperArmLength, 0);
      shoulder2Elbow.rotateX(-(tAJR.angles[offset + 1] - pi / 2.0f));
      shoulder2Elbow.rotateY(tAJR.angles[offset + 0] + pi / 2.0f);
      Vector3BH<> handInEllbow = shoulder2Elbow * target;

      handInEllbow.normalizeBH(theRobotDimensionsBH.lowerArmLength);
      target = shoulder2Elbow.invert() * handInEllbow;
      bestMatch = shoulder2Elbow.invert() * Vector3BH<>(0, 0, 0);
    }

    elbow = bestMatch;
    return true;
  }

  static void calcJointsForElbowPos(const Vector3BH<> &elbow, const Vector3BH<> &target, JointDataBH& targetJointData, int offset, const RobotDimensionsBH& theRobotDimensionsBH)
  {
    //set elbow position with the pitch/yaw unit in the shoulder
    targetJointData.angles[offset + 0] = std::atan2(elbow.z, elbow.x);
    targetJointData.angles[offset + 1] = std::atan2(elbow.y, std::sqrt(sqrBH(elbow.x) + sqrBH(elbow.z)));

    //calculate desired elbow "klapp"-angle
    const float c = target.abs(),
                a = theRobotDimensionsBH.upperArmLength,
                b = theRobotDimensionsBH.lowerArmLength;

    //cosine theorem
    float cosAngle = (-sqrBH(c) + sqrBH(b) + sqrBH(a)) / (2.0f * a * b);
    if(cosAngle < -1.0f)
    {
//      ASSERT(cosAngle > -1.1);
      cosAngle = -1.0f;
    }
    else if(cosAngle > 1.0f)
    {
      ASSERT(cosAngle < 1.1);
      cosAngle = 1.0f;
    }
    targetJointData.angles[offset + 3] = std::acos(cosAngle) - pi;

    //calculate hand in elbow coordinate system and calculate last angle
    Pose3DBH shoulder2Elbow;
    shoulder2Elbow.translate(0, -theRobotDimensionsBH.upperArmLength, 0);
    shoulder2Elbow.rotateX(-(targetJointData.angles[offset + 1] - pi / 2.0f));
    shoulder2Elbow.rotateY(targetJointData.angles[offset + 0] + pi / 2.0f);
    const Vector3BH<> handInEllbow = shoulder2Elbow * target;

    targetJointData.angles[offset + 2] = -(std::atan2(handInEllbow.z, handInEllbow.x) + pi / 2.f);
    while(targetJointData.angles[offset + 2] > pi)
      targetJointData.angles[offset + 2] -= 2 * pi;
    while(targetJointData.angles[offset + 2] < -pi)
      targetJointData.angles[offset + 2] += 2 * pi;
  }

  /**
  * Solves the inverse kinematics for the arms of the Nao with arbitrary elbow yaw.
  * @param position Position of the arm in cartesian space relative to the robot origin.
  * @param elbowYaw The fixed angle of the elbow yaw joint.
  * @param jointData The instance of JointDataBH where the resulting joints are written into.
  * @param left Determines whether the left or right arm is computed.
  * @param robotDimensions The robot dimensions needed for the calculation.
  */
  static void calcArmJoints(const Vector3BH<>& position, const float elbowYaw, JointDataBH& jointData, bool left, const RobotDimensionsBH& robotDimensions)
  {
    JointDataBH::Joint firstJoint(left ? JointDataBH::LShoulderPitch : JointDataBH::RShoulderPitch);
    const int sign(left ? -1 : 1);
    const Vector3BH<> pos(position - Vector3BH<>(robotDimensions.armOffset.x, robotDimensions.armOffset.y * -sign, robotDimensions.armOffset.z));
    float& joint0 = jointData.angles[firstJoint + 0];
    float& joint1 = jointData.angles[firstJoint + 1];
    const float& joint2 = jointData.angles[firstJoint + 2] = elbowYaw;
    float& joint3 = jointData.angles[firstJoint + 3];

    // distance of the end effector position to the origin
    const float positionAbs = pos.abs();

    // the upper and lower arm form a triangle with the air line to the end effector position being the third edge. Elbow angle can be computed using cosine theorem
    const float actualUpperArmLength = Vector2BH<>(robotDimensions.upperArmLength, robotDimensions.yElbowShoulder).abs();
    float cosElbow = (sqrBH(actualUpperArmLength) + sqrBH(robotDimensions.lowerArmLength) - sqrBH(positionAbs)) / (2.0f * robotDimensions.upperArmLength * robotDimensions.lowerArmLength);
    // clip for the case of unreachable position
    cosElbow = RangeBH<>(-1.0f, 1.0f).limit(cosElbow);
    // elbow is streched in zero-position, hence pi - innerAngle
    joint3 = -(pi - std::acos(cosElbow));
    // compute temporary end effector position from known third and fourth joint angle
    const Pose3DBH tempPose = Pose3DBH(robotDimensions.upperArmLength, robotDimensions.yElbowShoulder * -sign, 0).rotateX(joint2 * -sign).rotateZ(joint3 * -sign).translate(robotDimensions.lowerArmLength, 0, 0);

    /* offset caused by third and fourth joint angle */                /* angle needed to realise y-component of target position */
    joint1 = std::atan2(tempPose.translation.y * sign, tempPose.translation.x) + std::asin(pos.y / Vector2BH<>(tempPose.translation.x, tempPose.translation.y).abs()) * -sign;
    // refresh temporary endeffector position with known joint1
    const Pose3DBH tempPose2 = Pose3DBH().rotateZ(joint1 * -sign).conc(tempPose);

    /* first compensate offset from temporary position */       /* angle from target x- and z-component of target position */
    joint0 = -std::atan2(tempPose2.translation.z, tempPose2.translation.x) + std::atan2(pos.z, pos.x);
  }

  /**
   * Solves the inverse kinematics for the head of the Nao such that the camera looks at a certain point.
   * @param position Point the camera should look at in cartesian space relative to the robot origin.
   * @param imageTilt Tilt angle at which the point should appear in the image (pi/2: center of image, less than pi/2 => closer to the top of the image.)
   * @param panTilt Vector [pan, tilt] containing the resulting joint angles.
   * @param lowerCamera true if the joint angles are to be determined for the lower camera, false for the upper camera.
   * @param robotDimensions The robot dimensions needed for the calculation.
   * @param cameraCalibration The camera calibration
   */
  static void calcHeadJoints(const Vector3BH<>& position, const float imageTilt,
    const RobotDimensionsBH& robotDimensions, const bool lowerCamera, Vector2BH<>& panTilt, const CameraCalibrationBH& cameraCalibration)
  {
    Vector2BH<> headJoint2Target(std::sqrt(sqrBH(position.x) + sqrBH(position.y)), position.z - robotDimensions.zLegJoint1ToHeadPan);
    Vector2BH<> headJoint2Camera(robotDimensions.getXHeadTiltToCamera(lowerCamera),
                               robotDimensions.getZHeadTiltToCamera(lowerCamera));
    const float headJoint2CameraAngle = std::atan2(headJoint2Camera.x, headJoint2Camera.y);
    const float cameraAngle = pi3_2 - imageTilt - (pi_2 - headJoint2CameraAngle) - robotDimensions.getHeadTiltToCameraTilt(lowerCamera);
    const float targetAngle = std::asin(headJoint2Camera.abs() * std::sin(cameraAngle) / headJoint2Target.abs());
    const float headJointAngle = pi - targetAngle - cameraAngle;
    const float tilt = std::atan2(headJoint2Target.x, headJoint2Target.y) - headJointAngle - headJoint2CameraAngle;
    panTilt.y = -tilt;
    panTilt.x = std::atan2(position.y, position.x);
    if(lowerCamera)
    {
      panTilt.y += cameraCalibration.cameraTiltCorrection; // headTilt joint angle is flipped for some reason
      panTilt.x -= cameraCalibration.cameraPanCorrection;
    }
    else
    {
      panTilt.y += cameraCalibration.upperCameraTiltCorrection; // headTilt joint angle is flipped for some reason
      panTilt.x -= cameraCalibration.upperCameraPanCorrection;
    }
  }
};
