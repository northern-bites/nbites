/*
* @file KickEngineData.cpp
* This file implements a module that creates motions.
* @author <A href="mailto:judy@tzi.de">Judith Müller</A>
*/

#include <cstring>

#include "KickEngineData.h"
#include "Tools/Debugging/Modify.h"
#include "Tools/InverseKinematic.h"
#include "Tools/Debugging/DebugDrawings.h"

using namespace std;

bool KickEngineData::getMotionIDByName(const MotionRequestBH& motionRequest, const std::vector<KickEngineParameters>& params)
{
  motionID = -1;

  for(unsigned int i = 0; i < params.size(); ++i)
    if(motionRequest.kickRequest.getKickMotionFromName(&params[i].name[0]) == motionRequest.kickRequest.kickMotionType)
    {
      motionID = i;
      return true;
    }

  return false;
}

void KickEngineData::calculateOrigins(const RobotDimensionsBH& theRobotDimensions, const FilteredJointDataBH& jd)
{
  if(!wasActive)
  {
    origins[Phase::leftFootTra] = robotModel.limbs[MassCalibrationBH::footLeft].translate(0.f, 0.f, -theRobotDimensions.heightLeg5Joint).translation;
    origins[Phase::rightFootTra] = robotModel.limbs[MassCalibrationBH::footRight].translate(0.f, 0.f, -theRobotDimensions.heightLeg5Joint).translation;
    origins[Phase::leftArmTra] = robotModel.limbs[MassCalibrationBH::foreArmLeft].translation;
    origins[Phase::rightArmTra] = robotModel.limbs[MassCalibrationBH::foreArmRight].translation;

    origins[Phase::leftFootRot] =  robotModel.limbs[MassCalibrationBH::footLeft].rotation.getAngleAxis();
    origins[Phase::rightFootRot] =  robotModel.limbs[MassCalibrationBH::footRight].rotation.getAngleAxis();
    origins[Phase::leftHandRot] = robotModel.limbs[MassCalibrationBH::foreArmLeft].rotation.getAngleAxis();
    origins[Phase::rightHandRot] = robotModel.limbs[MassCalibrationBH::foreArmRight].rotation.getAngleAxis();
  }
  else
  {
    for(int i = 0; i < Phase::numOfLimbs; ++i)
    {
      origins[i] = currentParameters.phaseParameters[currentParameters.numberOfPhases - 1].controlPoints[i][2];
    }
  }
}

bool KickEngineData::checkPhaseTime(const FrameInfoBH& frame, const RobotDimensionsBH& rd, const FilteredJointDataBH& jd, const TorsoMatrixBH& torsoMatrix)
{
  timeSinceTimeStamp = frame.getTimeSince(timeStamp);

  if(motionID > -1)
  {
    if(phaseNumber < currentParameters.numberOfPhases)
    {
      if((unsigned int) timeSinceTimeStamp > currentParameters.phaseParameters[phaseNumber].duration)
      {
        phaseNumber++;
        timeStamp = frame.time;
        timeSinceTimeStamp = frame.getTimeSince(timeStamp);
        if(currentKickRequest.dynamical && !currentKickRequest.dynPoints.empty())
          for(unsigned int i = 0; i < currentKickRequest.dynPoints.size(); i++)
            if(currentKickRequest.dynPoints[i].phaseNumber == phaseNumber)
              addDynPoint(currentKickRequest.dynPoints[i], rd, torsoMatrix);
      }
    }
    else if(currentParameters.loop && phaseNumber == currentParameters.numberOfPhases)
    {
      phaseNumber = 0;
      calculateOrigins(rd, jd);
      currentParameters.initFirstPhaseLoop(origins, currentParameters.phaseParameters[currentParameters.numberOfPhases - 1].comTra[2], Vector2BH<>(jd.angles[JointDataBH::HeadPitch], jd.angles[JointDataBH::HeadYaw]));
      if(currentKickRequest.dynamical && !currentKickRequest.dynPoints.empty())
        for(unsigned int i = 0; i < currentKickRequest.dynPoints.size(); i++)
          if(currentKickRequest.dynPoints[i].phaseNumber == phaseNumber)
            addDynPoint(currentKickRequest.dynPoints[i], rd, torsoMatrix);
    }

    return (phaseNumber < currentParameters.numberOfPhases);
  }
  else
    return false;
}

bool KickEngineData::calcJoints(JointRequestBH& jointRequest, const RobotDimensionsBH& rd, const HeadJointRequestBH& hr)
{
  //Calculate Legs
  if(motionID > -1)
  {
    if(!currentParameters.ignoreHead)
    {
      jointRequest.angles[JointDataBH::HeadPitch] = head.x;
      jointRequest.angles[JointDataBH::HeadYaw] = head.y;
    }
    else
    {
     jointRequest.angles[JointDataBH::HeadYaw] = hr.pan == JointDataBH::off ? 0 : (!currentKickRequest.mirror ? hr.pan : -hr.pan);
     jointRequest.angles[JointDataBH::HeadPitch] = hr.tilt == JointDataBH::off ? 0 : hr.tilt;
    }

    calcLegJoints(JointDataBH::LHipYawPitch, jointRequest, rd);
    calcLegJoints(JointDataBH::RHipYawPitch, jointRequest, rd);

    jointRequest.angles[JointDataBH::LHipRoll] -= comOffset.x;
    jointRequest.angles[JointDataBH::RHipRoll] += comOffset.x;

    jointRequest.angles[JointDataBH::RHipPitch] += comOffset.y;
    jointRequest.angles[JointDataBH::LHipPitch] += comOffset.y;

    simpleCalcArmJoints(JointDataBH::LShoulderPitch, jointRequest, rd, positions[Phase::leftArmTra], positions[Phase::leftHandRot]);
    simpleCalcArmJoints(JointDataBH::RShoulderPitch, jointRequest, rd, positions[Phase::rightArmTra], positions[Phase::rightHandRot]);
    return true;
  }
  else //just set the angles from init
  {
    for(int i = JointDataBH::LShoulderPitch; i < JointDataBH::numOfJoints; ++i)
      jointRequest.angles[i] = lastBalancedJointRequest.angles[i]; 

    return false;
  }
}

void KickEngineData::calcLegJoints(const JointDataBH::Joint& joint, JointRequestBH& jointRequest, const RobotDimensionsBH& theRobotDimensions)
{
  float sign = joint == JointDataBH::LHipYawPitch ? 1.f : -1.f;
  float leg0, leg1, leg2, leg3, leg4, leg5;

  const Vector3BH<>& footPos = (sign > 0) ? positions[Phase::leftFootTra] : positions[Phase::rightFootTra];
  const Vector3BH<>& footRotAng = (sign > 0) ? positions[Phase::leftFootRot] : positions[Phase::rightFootRot];

  RotationMatrixBH rotateBodyTilt = RotationMatrixBH().rotateX(comOffset.x);
  Vector3BH<> anklePos = rotateBodyTilt * Vector3BH<>(footPos.x, footPos.y, footPos.z);
  //we need just the leg length x and y have to stay the same
  anklePos.y = footPos.y;
  anklePos.x = footPos.x;
  // for the translation of the footpos we only need to translate the anklepoint, which is the intersection of the axis leg4 and leg5
  // the rotation of the foot will be made later by rotating the footpos around the anklepoint
  anklePos -= Vector3BH<>(0.f, sign * (theRobotDimensions.lengthBetweenLegs / 2), -theRobotDimensions.heightLeg5Joint);

  RotationMatrixBH rotateBecauseOfHip = RotationMatrixBH().rotateZ(footRotAng.z).rotateX(-sign * pi_4);

  anklePos = rotateBecauseOfHip * anklePos;

  float diagonal = anklePos.abs();

  // upperLegLength, lowerLegLength, and diagonal form a triangle, use cosine theorem
  float a1 = (theRobotDimensions.upperLegLength * theRobotDimensions.upperLegLength -
              theRobotDimensions.lowerLegLength * theRobotDimensions.lowerLegLength + diagonal * diagonal) /
             (2 * theRobotDimensions.upperLegLength * diagonal);
  //if(abs(a1) > 1.f) OUTPUT(idText, text, "clipped a1");
  a1 = abs(a1) > 1.f ? 0.f : acos(a1);

  float a2 = (theRobotDimensions.upperLegLength * theRobotDimensions.upperLegLength +
              theRobotDimensions.lowerLegLength * theRobotDimensions.lowerLegLength - diagonal * diagonal) /
             (2 * theRobotDimensions.upperLegLength * theRobotDimensions.lowerLegLength);
  //if(abs(a2) > 1.f) OUTPUT(idText, text, "clipped a2");
  a2 = abs(a2) > 1.f ? pi : acos(a2);

  leg0 = footRotAng.z * sign;
  leg2 = -a1 - atan2(anklePos.x, Vector2BH<>(anklePos.y, anklePos.z).abs() * -sgnBH(anklePos.z));

  leg1 = anklePos.z == 0.0f ? 0.0f : atan(anklePos.y / -anklePos.z) * -sign;
  leg3 = pi - a2;

  RotationMatrixBH footRot = RotationMatrixBH().rotateX(leg1 * -sign).rotateY(leg2 + leg3);
  footRot = footRot.invert() * rotateBecauseOfHip;

  leg5 = asin(-footRot[2].y) * sign * -1;
  leg4 = -atan2(footRot[2].x, footRot[2].z) * -1;

  leg4 += footRotAng.y;
  leg5 += footRotAng.x;

  jointRequest.angles[joint] = leg0;
  jointRequest.angles[joint + 1] = -pi_4 + leg1;
  jointRequest.angles[joint + 2] = leg2;
  jointRequest.angles[joint + 3] = leg3;
  jointRequest.angles[joint + 4] = leg4;
  jointRequest.angles[joint + 5] = leg5;
}

void KickEngineData::simpleCalcArmJoints(const JointDataBH::Joint& joint, JointRequestBH& jointRequest, const RobotDimensionsBH& theRobotDimensions, const Vector3BH<>& armPos, const Vector3BH<>& handRotAng)
{
  float sign = joint == JointDataBH::LShoulderPitch ? 1.f : -1.f;

  Vector3BH<> target =  armPos -  Vector3BH<>(theRobotDimensions.armOffset.x,
                                          theRobotDimensions.armOffset.y * sign,
                                          theRobotDimensions.armOffset.z);

  jointRequest.angles[joint + 0] = atan2(target.z, target.x);
  jointRequest.angles[joint + 1] = atan2(target.y * sign, sqrt(sqrBH(target.x) + sqrBH(target.z)));

  float length2ElbowJoint = Vector3BH<>(theRobotDimensions.upperArmLength, theRobotDimensions.yElbowShoulder, 0.f).abs();
  float angle = asin(theRobotDimensions.upperArmLength / length2ElbowJoint);

  Pose3DBH elbow;
  elbow.rotateY(-jointRequest.angles[joint + 0])
  .rotateZ(jointRequest.angles[joint + 1])
  .translate(length2ElbowJoint, 0.f , 0.f)
  .rotateZ(-angle)
  .translate(theRobotDimensions.yElbowShoulder, 0.f, 0.f);

  jointRequest.angles[joint + 0] = atan2(elbow.translation.z, elbow.translation.x);
  jointRequest.angles[joint + 1] = atan2(elbow.translation.y, sqrt(sqrBH(elbow.translation.x) + sqrBH(elbow.translation.z)));
  jointRequest.angles[joint + 0] = (jointRequest.angles[joint + 0] < pi) ? jointRequest.angles[joint + 0] : 0;  //clip special case

  Pose3DBH hand(elbow.translation);

  hand.rotateZ(handRotAng.z * sign)
  .rotateY(handRotAng.y)
  .rotateX(handRotAng.x * sign)
  .translate(theRobotDimensions.lowerArmLength, 0.f, 0.f);

  InverseKinematic::calcJointsForElbowPos(elbow.translation, hand.translation, jointRequest, joint, theRobotDimensions);
}

void KickEngineData::balanceCOM(JointRequestBH& joints, const RobotDimensionsBH& rd, const MassCalibrationBH& mc, const FilteredJointDataBH& theFilteredJointData)
{
  if(currentParameters.autoComTra)
  {
    setStandLeg(origin.y);
    calculatePreviewCom(ref, origin);
  }
  comRobotModel.setJointData(joints, rd, mc);
  Vector3BH<> com(comRobotModel.centerOfMass);

  Pose3DBH torso = (toLeftSupport) ? comRobotModel.limbs[MassCalibrationBH::footLeft] : comRobotModel.limbs[MassCalibrationBH::footRight];

  com = torso.rotation.invert() * com;

  actualDiff = com - ref;
  // if(!toLeftSupport)
  //    actualDiff.y += 15;

  balanceSum += Vector2BH<> (actualDiff.x, actualDiff.y);

  float height = comRobotModel.centerOfMass.z - ref.z;

  Vector2BH<> balance;
  balance.y = -currentParameters.kpx * (actualDiff.y) + -currentParameters.kix * balanceSum.y + -currentParameters.kdx * ((actualDiff.y - lastCom.y) / cycletime);
  balance.x = currentParameters.kpy * (actualDiff.x)  + currentParameters.kiy * balanceSum.x + currentParameters.kdy * ((actualDiff.x - lastCom.x) / cycletime);

  if(height != 0.f)
  {
    comOffset.x = (balance.y != 0) ? atan2((balance.y), height) : 0;
    comOffset.y = (balance.x != 0) ? atan2((balance.x), height) : 0;
  }

  // Northern bites debugging, for measuring stability
  // printf("%d %f %f\n", phaseNumber, actualDiff.y, comOffset.x);

  lastCom = actualDiff;
}

void KickEngineData::calculatePreviewCom(Vector3BH<>& ref, Vector2BH<>& origin)
{
  ref = Vector3BH<>(0.f, 0.f, 0.f);
  float lastPhaseTime = 0.f;
  float future = 0.f;
  int futPhaNum = phaseNumber;
  float currPhaseTime = (float)timeSinceTimeStamp;

  for(int j = 1; j < currentParameters.preview + 1; j++)
  {
    if(futPhaNum < currentParameters.numberOfPhases)
    {
      future = (float)(currPhaseTime + j * cycletime * 1000 - lastPhaseTime) / (float) currentParameters.phaseParameters[futPhaNum].duration;

      if(future > 1.f &&  futPhaNum < currentParameters.numberOfPhases - 1)
      {
        future -= 1.f;
        lastPhaseTime = j * cycletime * 1000;
        currPhaseTime = 0.f;
        futPhaNum++;
      }

      if(future > 1.f && futPhaNum + 1 > currentParameters.numberOfPhases - 1)

      {
        if(currentParameters.loop)
        {
          future = 0.f;
          lastPhaseTime = j * cycletime * 1000;
          currPhaseTime = 0.f;
          futPhaNum = 0;
        }
        else
        {
          future = 1.f;
          futPhaNum = currentParameters.numberOfPhases - 1;
        }
      }
    }
    else
    {
      future = 1.f;
      futPhaNum = currentParameters.numberOfPhases - 1;
    }

    Vector3BH<> futPos[2];
    futPos[0] = currentParameters.getPosition(future, futPhaNum, Phase::leftFootTra);
    futPos[1] = currentParameters.getPosition(future, futPhaNum, Phase::rightFootTra);

    Vector3BH<> comRef(0, 0, 0);
    Vector2BH<> fOrigin(0, 0);

    getCOMReference(futPos[0],  futPos[1], comRef, fOrigin);

    ref += comRef;
    origin += fOrigin;
  }
  ref /= (float)currentParameters.preview;
  origin /= (float)currentParameters.preview;
}

void KickEngineData::setStandLeg(const float& originY)
{
  //If the average standLegPos Reference has reached the current standleg position, lock that
  if(abs(positions[Phase::leftFootTra].y - (float)standLegPos.getAverage().y) < 1.f)
  {
    lSupp = true;
    rSupp = false;
  }
  if(abs(positions[Phase::rightFootTra].y - (float)standLegPos.getAverage().y) < 1.f)
  {
    rSupp = true;
    lSupp = false;
  }
  //originY goes back to zero when the feet reaches a stable position (example: The distances of the feet to the middle are equal)
  if(abs(originY) < 1.f)
  {
    rSupp = false;
    lSupp = false;
  }
}

void KickEngineData::getCOMReference(const Vector3BH<>& lFootPos, const Vector3BH<>& rFootPos, Vector3BH<>& comRef, Vector2BH<>& origin)
{
  origin = Vector2BH<>(rFootPos.x + 30.f, 0.f);
  comRef = Vector3BH<>(origin.x, 0.f, rFootPos.z); //double support

  if(abs(lFootPos.y + rFootPos.y) > 2.f)  //equal distance to the middle
  {
    if((lFootPos.y + rFootPos.y) < -2.f  && !lSupp && !rSupp) //calc reference
    {
      origin.y = origins[Phase::leftFootTra].y;
      comRef = lFootPos - Vector3BH<>(-30.f, 0.f, 0.f); //L-Support
      if(comRef.y > 0.f) comRef.y = 0.f;
      standLegPos.add(Vector2BH<int>((int)comRef.x, (int)comRef.y));
    }

    if((lFootPos.y + rFootPos.y) > 2.f  && !lSupp && !rSupp)
    {
      origin.y = origins[Phase::rightFootTra].y;
      comRef = rFootPos - Vector3BH<>(-30.f, 0.f, 0.f); //R-Support
      if(comRef.y < 0.f) comRef.y = 0.f;
      standLegPos.add(Vector2BH<int>((int)comRef.x, (int)comRef.y));
    }

    if(lSupp)
    {
      origin.y = origins[Phase::leftFootTra].y;
      comRef = Vector3BH<>((float)standLegPos.getAverage().x, (float)standLegPos.getAverage().y + 10, lFootPos.z);
    }
    if(rSupp)
    {
      origin.y = origins[Phase::rightFootTra].y;
      comRef = Vector3BH<>((float)standLegPos.getAverage().x, (float)standLegPos.getAverage().y - 10, rFootPos.z);
    }
  }
}

void KickEngineData::setStaticReference()
{
  if(!currentParameters.autoComTra)
  {
    Vector2BH<> staticref =  currentParameters.getComRefPosition(phase, phaseNumber);
    ref = Vector3BH<>(staticref.x, staticref.y, 0.f);
    ref.z = (positions[Phase::leftFootTra].z > positions[Phase::rightFootTra].z) ? positions[Phase::leftFootTra].z : positions[Phase::rightFootTra].z;
  }
}

void KickEngineData::mirrorIfNecessary(JointRequestBH& joints)
{
  if((positions[Phase::leftFootTra].z - positions[Phase::rightFootTra].z) > 5)
  {
    toLeftSupport = false;
  }
  else if((positions[Phase::leftFootTra].z - positions[Phase::rightFootTra].z) < -5)
  {
    toLeftSupport = true;
  }
  else
  {
    if(ref.y > 0)
    {
      toLeftSupport = true;
    }
    else
    {
      toLeftSupport = false;
    }
  }

  if(currentKickRequest.mirror)
  {
    JointRequestBH old = joints;
    for(int i = 0; i < JointDataBH::numOfJoints; ++i)
    {
      if(i == JointDataBH::HeadPitch)
        continue;

      joints.angles[i] = old.mirror(JointDataBH::Joint(i));
    }
  }
}

void KickEngineData::addGyroBalance(JointRequestBH& jointRequest, const JointCalibrationBH& jc, const FilteredSensorDataBH& sd, const float& ratio)
{
  if(sd.data[SensorDataBH::gyroY] != 0 &&  sd.data[SensorDataBH::gyroX] != 0 && !willBeLeft)
  {
    //Predict next gyrodata
    gyro.y = sd.data[SensorDataBH::gyroY] * 0.3f + 0.7f * gyro.y;
    gyro.x = sd.data[SensorDataBH::gyroX] * 0.3f + 0.7f * gyro.x;

    //some clipping
    for(int i = JointDataBH::LHipYawPitch; i < JointDataBH::numOfJoints; i++)
    {
      if(jointRequest.angles[i] > jc.joints[i].maxAngle) jointRequest.angles[i] = jc.joints[i].maxAngle;
      if(jointRequest.angles[i] < jc.joints[i].minAngle) jointRequest.angles[i] = jc.joints[i].minAngle;
    }

    JointRequestBH balancedJointRequest = jointRequest;

    //calculate the commandedVelocity
    float commandedVelocity[4];
    //y-velocity if left leg is support
    commandedVelocity[0] = (balancedJointRequest.angles[JointDataBH::LHipPitch] - lastBalancedJointRequest.angles[JointDataBH::LHipPitch]) / cycletime;
    //y-velocity if right leg is support
    commandedVelocity[1] = (balancedJointRequest.angles[JointDataBH::RHipPitch] - lastBalancedJointRequest.angles[JointDataBH::RHipPitch]) / cycletime;
    //x-velcocity if left leg is support
    commandedVelocity[2] = (balancedJointRequest.angles[JointDataBH::LHipRoll] - lastBalancedJointRequest.angles[JointDataBH::LHipRoll]) / cycletime;
    //x-velocity if right leg is support
    commandedVelocity[3] = (balancedJointRequest.angles[JointDataBH::RHipRoll] - lastBalancedJointRequest.angles[JointDataBH::RHipRoll]) / cycletime;

    //calculate disturbance from meseaured velocity and commanded velocity
    // y-velocity if left leg is support
    float gyroVelyLeft = (gyro.y + commandedVelocity[0] - lastGyroLeft.y) / cycletime;
    lastGyroLeft.y = gyro.y + commandedVelocity[0];
    //y-velocity if right leg is support
    float gyroVelyRight = (gyro.y + commandedVelocity[1] - lastGyroRight.y) / cycletime;
    lastGyroRight.y = gyro.y + commandedVelocity[1];
    //x-velocity if left leg is support
    float gyroVelxLeft = (gyro.x + commandedVelocity[2] - lastGyroLeft.x) / cycletime;
    lastGyroLeft.x = gyro.x + commandedVelocity[2];
    //x-velocity if right leg is support
    float gyroVelxRight = (gyro.x - commandedVelocity[3] - lastGyroRight.x) / cycletime;
    lastGyroRight.x = gyro.x - commandedVelocity[3];

    //calculate control variable with PID-Control
    float calcVelocity[4];
    //y if left supprt
    calcVelocity[0] = -gyroP.y * (gyro.y + commandedVelocity[0]) - gyroD.y * gyroVelyLeft - gyroI.y * (gyroErrorLeft.y);
    //y if right support
    calcVelocity[1] = -gyroP.y * (gyro.y + commandedVelocity[1]) - gyroD.y * gyroVelyRight - gyroI.y * (gyroErrorRight.y);
    //x if left support
    calcVelocity[2] = gyroP.x * (gyro.x + commandedVelocity[2]) - gyroD.x * gyroVelxLeft - gyroI.x * gyroErrorLeft.x;
    //x if right support
    calcVelocity[3] = -gyroP.x * (gyro.x - commandedVelocity[3]) + gyroD.x * gyroVelxRight + gyroI.x * gyroErrorRight.x;

    bool supp = (currentKickRequest.mirror) ? !toLeftSupport : toLeftSupport;

    if(supp)  //last support Leg was left
    {
      //y
      jointRequest.angles[JointDataBH::RHipPitch] += calcVelocity[0] * cycletime * ratio;
      jointRequest.angles[JointDataBH::LHipPitch] += calcVelocity[0] * cycletime * ratio;
      jointRequest.angles[JointDataBH::LAnklePitch] +=  calcVelocity[0] * cycletime * ratio;
      jointRequest.angles[JointDataBH::RAnklePitch] += calcVelocity[0] * cycletime * ratio;
      //x
      jointRequest.angles[JointDataBH::LHipRoll] += -calcVelocity[2] * cycletime * ratio;
      jointRequest.angles[JointDataBH::RHipRoll] += calcVelocity[2] * cycletime * ratio;
      jointRequest.angles[JointDataBH::LAnkleRoll] -= calcVelocity[2] * cycletime * ratio;
      jointRequest.angles[JointDataBH::RAnkleRoll] -= -calcVelocity[2] * cycletime * ratio;
    }
    else //if(toRightSupport)
    {
      //y
      jointRequest.angles[JointDataBH::RHipPitch] += calcVelocity[1] * cycletime * ratio;
      jointRequest.angles[JointDataBH::LHipPitch] += calcVelocity[1] * cycletime * ratio;
      jointRequest.angles[JointDataBH::LAnklePitch] +=  calcVelocity[1] * cycletime * ratio;

      jointRequest.angles[JointDataBH::RAnklePitch] += calcVelocity[1] * cycletime * ratio;

      //x
      jointRequest.angles[JointDataBH::LHipRoll] += calcVelocity[3] * cycletime * ratio;
      jointRequest.angles[JointDataBH::RHipRoll] += -calcVelocity[3] * cycletime * ratio;

      jointRequest.angles[JointDataBH::LAnkleRoll] -= -calcVelocity[3] * cycletime * ratio;
      jointRequest.angles[JointDataBH::RAnkleRoll] -= calcVelocity[3] * cycletime * ratio;
    }
    gyroErrorLeft += lastGyroLeft;
    gyroErrorRight += lastGyroRight;
    lastBalancedJointRequest = balancedJointRequest;
  }
}

void KickEngineData::addDynPoint(const DynPoint& dynPoint, const RobotDimensionsBH& rd, const TorsoMatrixBH& torsoMatrix)
{
  Vector3BH <> cubePoint, diff, d(dynPoint.translation);

  transferDynPoint(d, rd, torsoMatrix);

  int phaseNumber = dynPoint.phaseNumber;
  int limb =  dynPoint.limb;

  if(dynPoint.duration > 0) currentParameters.phaseParameters[phaseNumber].duration = dynPoint.duration;

  currentParameters.phaseParameters[phaseNumber].odometryOffset = dynPoint.odometryOffset;

  cubePoint = currentParameters.phaseParameters[phaseNumber].controlPoints[limb][2];

  diff = cubePoint - d;

  currentParameters.phaseParameters[phaseNumber].controlPoints[limb][2] -= diff;

  currentParameters.phaseParameters[phaseNumber].controlPoints[limb][1] -= diff;

  Vector3BH<> point1 = currentParameters.phaseParameters[phaseNumber].controlPoints[limb][2] -
                     currentParameters.phaseParameters[phaseNumber].controlPoints[limb][1];

  Vector3BH<> absAngle;

  absAngle.x = atan2(point1.y, point1.z);
  absAngle.y = atan2(point1.x, point1.z);
  absAngle.z = atan2(point1.x, point1.y);

  RotationMatrixBH rot = RotationMatrixBH().rotateX(dynPoint.angle.x - absAngle.x).
                       rotateY(dynPoint.angle.y - absAngle.y).
                       rotateZ(dynPoint.angle.z - absAngle.z);

  currentParameters.phaseParameters[phaseNumber].controlPoints[limb][1] =
    (rot * point1) + currentParameters.phaseParameters[phaseNumber].controlPoints[limb][1];

  if(phaseNumber < currentParameters.numberOfPhases - 1)
  {
    currentParameters.phaseParameters[phaseNumber + 1].controlPoints[limb][0] =
      currentParameters.phaseParameters[phaseNumber].controlPoints[limb][2] -
      currentParameters.phaseParameters[phaseNumber].controlPoints[limb][1];

    float factor = (float)currentParameters.phaseParameters[phaseNumber + 1].duration /
                   (float)currentParameters.phaseParameters[phaseNumber].duration;
    currentParameters.phaseParameters[phaseNumber + 1].controlPoints[limb][0] *= factor;

    currentParameters.phaseParameters[phaseNumber + 1].controlPoints[limb][0] +=
      currentParameters.phaseParameters[phaseNumber].controlPoints[limb][2];
  }
}

void KickEngineData::transferDynPoint(Vector3BH<>& d, const RobotDimensionsBH& rd, const TorsoMatrixBH& torsoMatrix)
{
  Pose3DBH left(positions[Phase::leftFootTra]);
  Pose3DBH right(positions[Phase::rightFootTra]);

  const bool useLeft = left.translation.z < right.translation.z;
  const Vector3BH<> foot(useLeft ? left.translation : right.translation);

  Pose3DBH left2(torsoMatrix.rotation);
  left2.conc(robotModel.limbs[MassCalibrationBH::footLeft])
  .translate(0.f, 0.f, -rd.heightLeg5Joint);
  Pose3DBH right2(torsoMatrix.rotation);
  right2.conc(robotModel.limbs[MassCalibrationBH::footRight])
  .translate(0.f, 0.f, -rd.heightLeg5Joint);

  if(currentKickRequest.mirror)
  {
    Pose3DBH temp = left2;
    left2 = right2;
    right2 = temp;
    left2.translation.y *= -1;
    right2.translation.y *= -1;
  }

  const Vector3BH<> foot2(useLeft ? left2.translation : right2.translation);

  Vector3BH<> offset = foot - foot2;

  d += offset;
}

void KickEngineData::ModifyData(const KickRequest& br, JointRequestBH& kickEngineOutput, std::vector<KickEngineParameters>& params)
{
#ifndef RELEASE
  size_t p = params.size() - 1;
  MODIFY("module:KickEngine:newKickMotion", params[p]);
  strcpy(params[p].name, "newKick");
#endif //RELEASE

  MODIFY("module:KickEngine:px", gyroP.x);
  MODIFY("module:KickEngine:dx", gyroD.x);
  MODIFY("module:KickEngine:ix", gyroI.x);
  MODIFY("module:KickEngine:py", gyroP.y);
  MODIFY("module:KickEngine:dy", gyroD.y);
  MODIFY("module:KickEngine:iy", gyroI.y);

  MODIFY("module:KickEngine:formMode", formMode);
  MODIFY("module:KickEngine:lFootTraOff", limbOff[Phase::leftFootTra]);
  MODIFY("module:KickEngine:rFootTraOff", limbOff[Phase::rightFootTra]);
  MODIFY("module:KickEngine:lFootRotOff", limbOff[Phase::leftFootRot]);
  MODIFY("module:KickEngine:rFootRotOff", limbOff[Phase::rightFootRot]);
  MODIFY("module:KickEngine:lHandTraOff", limbOff[Phase::leftArmTra]);
  MODIFY("module:KickEngine:rHandTraOff", limbOff[Phase::rightArmTra]);
  MODIFY("module:KickEngine:lHandRotOff", limbOff[Phase::leftHandRot]);
  MODIFY("module:KickEngine:rHandRotOff", limbOff[Phase::rightHandRot]);

  //Plot com stabilizing
  PLOT("module:KickEngine:comy", robotModel.centerOfMass.y);
  PLOT("module:KickEngine:diffy", actualDiff.y);
  PLOT("module:KickEngine:refy", ref.y);

  PLOT("module:KickEngine:comx", robotModel.centerOfMass.x);
  PLOT("module:KickEngine:diffx", actualDiff.x);
  PLOT("module:KickEngine:refx", ref.x);

  PLOT("module:KickEngine:lastdiffy", toDegrees(lastBody.y));
  PLOT("module:KickEngine:bodyErrory", toDegrees(bodyError.y));

#ifndef RELEASE
  for(int i = 0; i < Phase::numOfLimbs; i++)
  {
    int hardness = 100;

    if(limbOff[i])
    {
      hardness = 0;
    }
    switch((Phase::Limb)i)
    {
    case Phase::leftFootTra:
      kickEngineOutput.jointHardness.hardness[JointDataBH::LHipRoll] = hardness;
      kickEngineOutput.jointHardness.hardness[JointDataBH::LHipPitch] = hardness;
      kickEngineOutput.jointHardness.hardness[JointDataBH::LKneePitch] = hardness;
      kickEngineOutput.jointHardness.hardness[JointDataBH::LAnklePitch] = hardness;
      kickEngineOutput.jointHardness.hardness[JointDataBH::LAnkleRoll] = hardness;
      break;
    case Phase::rightFootTra:
      kickEngineOutput.jointHardness.hardness[JointDataBH::RHipRoll] = hardness;
      kickEngineOutput.jointHardness.hardness[JointDataBH::RHipPitch] = hardness;
      kickEngineOutput.jointHardness.hardness[JointDataBH::RKneePitch] = hardness;
      kickEngineOutput.jointHardness.hardness[JointDataBH::RAnklePitch] = hardness;
      kickEngineOutput.jointHardness.hardness[JointDataBH::RAnkleRoll] = hardness;
      break;
    case Phase::leftFootRot:
      kickEngineOutput.jointHardness.hardness[JointDataBH::LAnklePitch] = hardness;
      kickEngineOutput.jointHardness.hardness[JointDataBH::LAnkleRoll] = hardness;
      kickEngineOutput.jointHardness.hardness[JointDataBH::RHipYawPitch] = hardness;
      kickEngineOutput.jointHardness.hardness[JointDataBH::LHipYawPitch] = hardness;
      break;
    case Phase::rightFootRot:
      kickEngineOutput.jointHardness.hardness[JointDataBH::RAnklePitch] = hardness;
      kickEngineOutput.jointHardness.hardness[JointDataBH::RAnkleRoll] = hardness;
      kickEngineOutput.jointHardness.hardness[JointDataBH::RHipYawPitch] = hardness;
      kickEngineOutput.jointHardness.hardness[JointDataBH::LHipYawPitch] = hardness;
      break;
    case Phase::leftArmTra:
      kickEngineOutput.jointHardness.hardness[JointDataBH::LShoulderPitch] = hardness;
      kickEngineOutput.jointHardness.hardness[JointDataBH::LShoulderRoll] = hardness;
      break;
    case Phase::rightArmTra:
      kickEngineOutput.jointHardness.hardness[JointDataBH::RShoulderPitch] = hardness;
      kickEngineOutput.jointHardness.hardness[JointDataBH::RShoulderRoll] = hardness;
      break;
    case Phase::leftHandRot:
      kickEngineOutput.jointHardness.hardness[JointDataBH::LElbowRoll] = hardness;
      kickEngineOutput.jointHardness.hardness[JointDataBH::LElbowYaw] = hardness;
      break;
    case Phase::rightHandRot:
      kickEngineOutput.jointHardness.hardness[JointDataBH::RElbowRoll] = hardness;
      kickEngineOutput.jointHardness.hardness[JointDataBH::RElbowYaw] = hardness;
      break;
    }
  }
#endif
}

void KickEngineData::debugFormMode(std::vector<KickEngineParameters>& params)
{
#ifndef RELEASE
  //this is for making motions with the tool
  /*  if(motionID != -1 && formMode)
    {
      currentParameters = params[motionID];
      currentParameters.initFirstPhase(origins);

      if(phaseNumber >= currentParameters.numberOfPhases)
      {
        for(int i = 0; i < Phase::numOfLimbs; ++i)
        {
          positions[i] = currentParameters.getPosition(1.f, currentParameters.numberOfPhases - 1, i);
        }
      }
    }*/
#endif
}

void KickEngineData::setCycleTime(float time)
{
  cycletime = time;
}

void KickEngineData::calcPhaseState()
{
  phase = (float) timeSinceTimeStamp / (float) currentParameters.phaseParameters[phaseNumber].duration;
}

void KickEngineData::calcPositions(JointRequestBH& joints, const FilteredJointDataBH& theFilteredJointData)
{
  for(int i = 0; i < Phase::numOfLimbs; ++i)
    positions[i] = currentParameters.getPosition(phase, phaseNumber, i);

  if(!currentParameters.ignoreHead)
    head = currentParameters.getHeadRefPosition(phase, phaseNumber);
  
  //these need to be similar
  if(positions[Phase::leftFootRot].z != -positions[Phase::rightFootRot].z)
  {
    //just take the nearer postion to 0
    if(abs(positions[Phase::leftFootRot].z) < abs(positions[Phase::rightFootRot].z))
    {
      positions[Phase::rightFootRot] = -positions[Phase::leftFootRot];
    }
    else
    {
      positions[Phase::leftFootRot] = -positions[Phase::rightFootRot];
    }
  }
}

void KickEngineData::setRobotModel(RobotModelBH rm)
{
  robotModel = rm;
}

void KickEngineData::setCurrentKickRequest(const MotionRequestBH& mr)
{
  currentKickRequest = mr.kickRequest;
}

bool KickEngineData::isMotionAlmostOver()
{
  return false;
}

void KickEngineData::setExecutedKickRequest(KickRequest& br)
{
  br.mirror = currentKickRequest.mirror;
  br.dynamical = currentKickRequest.dynamical;
  br.kickMotionType = currentKickRequest.kickMotionType;
}

void KickEngineData::initData(const bool& compensated, const FrameInfoBH& frame, const MotionRequestBH& mr, const RobotDimensionsBH& theRobotDimensions, std::vector<KickEngineParameters>& params, const FilteredJointDataBH& jd, const TorsoMatrixBH& torsoMatrix)
{
  if(getMotionIDByName(mr, params))
  {
    phase = 0.f;
    phaseNumber = 0;
    timeStamp = frame.time;

    ref = Vector3BH<>(0.f, 0.f, 0.f);
    actualDiff = ref;
    currentParameters = params[motionID];
    calculateOrigins(theRobotDimensions, jd);
    currentParameters.initFirstPhase(origins, Vector2BH<>(jd.angles[JointDataBH::HeadPitch], (mr.kickRequest.mirror) ? -jd.angles[JointDataBH::HeadYaw] : jd.angles[JointDataBH::HeadYaw]));

    if(!wasActive)
    {
      comRobotModel = robotModel;
      lSupp = false;
      rSupp = false;
      toLeftSupport = false;
      comOffset = Vector2BH<>(0.f, 0.f);
      origin = Vector2BH<>(0.f, 0.f);
      balanceSum = Vector2BH<>(0.f, 0.f);
      gyro = Vector2BH<>(0.f, 0.f);
      lastGyroLeft = Vector2BH<>(0.f, 0.f);
      lastGyroRight = Vector2BH<>(0.f, 0.f);
      gyroErrorLeft = Vector2BH<>(0.f, 0.f);
      gyroErrorRight = Vector2BH<>(0.f, 0.f);
      bodyError = Vector2BH<>(0.f, 0.f);
      lastBody = Vector2BH<>(0.f, 0.f);
      lastCom = Vector3BH<>(0.f, 0.f, 0.f);

      for(int i = 0; i < JointDataBH::numOfJoints; i++)
      {
        lastBalancedJointRequest.angles[i] = jd.angles[i];
      }
    }
    if(currentKickRequest.dynamical && !currentKickRequest.dynPoints.empty())
      for(unsigned int i = 0; i < currentKickRequest.dynPoints.size(); i++)
        if(currentKickRequest.dynPoints[i].phaseNumber == phaseNumber)
          addDynPoint(currentKickRequest.dynPoints[i], theRobotDimensions, torsoMatrix);
  }
}

void KickEngineData::setEngineActivation(const float& ratio)
{
  willBeLeft = (ratio < 1.f && lastRatio > ratio);
  wasActive = (ratio != 0.f && motionID > -1);
  startComp = (ratio != 0.f && lastRatio <= ratio);
  lastRatio = ratio;
}

bool KickEngineData::activateNewMotion(const KickRequest& br, const bool& isLeavingPossible)
{
    if(!wasActive || (br.kickMotionType != currentKickRequest.kickMotionType && isLeavingPossible)) {
        return true;
    }
  else if(br.kickMotionType == currentKickRequest.kickMotionType && br.mirror == currentKickRequest.mirror)
    currentKickRequest = br; // update KickRequest when it is compatible to the current motion

  return false;
}

Pose3DBH KickEngineData::calcDesBodyAngle(JointRequestBH& jointData, const RobotDimensionsBH& robotDimensions, JointDataBH::Joint joint)
{
  Pose3DBH footPos;
  float sign = joint == JointDataBH::LHipYawPitch ? -1.f : 1.f;

  footPos.translate(0, 0, robotDimensions.heightLeg5Joint)
  .rotateX(-jointData.angles[joint + 5]*sign)
  .rotateY(-jointData.angles[joint + 4])
  .translate(0, 0, robotDimensions.lowerLegLength)
  .rotateY(-jointData.angles[joint + 3])
  .translate(0, 0, robotDimensions.upperLegLength)
  .rotateY(-jointData.angles[joint + 2])
  .rotateX(((-jointData.angles[joint + 1] - pi_4)*sign))
  .rotateZ(-jointData.angles[joint]*sign)
  .rotateX(-pi_4 * sign);

  return footPos;
}

bool KickEngineData::sitOutTransitionDisturbance(bool& compensate, bool& compensated, const FilteredSensorDataBH& sd, KickEngineOutput& kickEngineOutput, const JointDataBH& jd, const FrameInfoBH& frame)
{
  if(compensate)
  {
    if(!startComp)
    {
      timeStamp = frame.time;
      lSupp = false;
      rSupp = false;
      toLeftSupport = false;
      comOffset = Vector2BH<>(0.f, 0.f);
      origin = Vector2BH<>(0.f, 0.f);
      balanceSum = Vector2BH<>(0.f, 0.f);
      gyro = Vector2BH<>(0.f, 0.f);
      lastGyroLeft = Vector2BH<>(0.f, 0.f);
      lastGyroRight = Vector2BH<>(0.f, 0.f);
      gyroErrorLeft = Vector2BH<>(0.f, 0.f);
      gyroErrorRight = Vector2BH<>(0.f, 0.f);
      bodyError = Vector2BH<>(0.f, 0.f);
      lastBody = Vector2BH<>(0.f, 0.f);
      lastCom = Vector3BH<>(0.f, 0.f, 0.f);
      motionID = -1;

      kickEngineOutput.isLeavingPossible = false;

      for(int i = 0; i < JointDataBH::numOfJoints; i++)
      {
        lastBalancedJointRequest.angles[i] = jd.angles[i];
        compenJoints.angles[i] = jd.angles[i];
      }
    }

    for(int i = 0; i < JointDataBH::numOfJoints; i++)
    {
      kickEngineOutput.angles[i] = compenJoints.angles[i];
      kickEngineOutput.jointHardness.hardness[i] = 100;
    }

    int time = frame.getTimeSince(timeStamp);
    if((abs(sd.data[SensorDataBH::gyroX]) < 0.1f && abs(sd.data[SensorDataBH::gyroY]) < 0.1f && time > 200) || time > 1000)
    {
      compensate = false;
      compensated = true;
      return true;
    }
    else
    {
      return false;
    }
  }

  return true;
}
