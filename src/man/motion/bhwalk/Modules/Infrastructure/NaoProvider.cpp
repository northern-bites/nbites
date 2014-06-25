/**
* @file Modules/Infrastructure/NaoProvider.cpp
* The file declares a module that provides information from the Nao via DCM.
* @author <a href="mailto:Thomas.Roefer@dfki.de">Thomas Röfer</a>
*/

//#define MEASURE_DELAY

#include <cstdio>
#include <cstring>

#include "NaoProvider.h"

#ifdef TARGET_ROBOT

#ifdef MEASURE_DELAY
#include "Tools/Streams/InStreams.h"
#endif
#include "Representations/Infrastructure/JointDataDeg.h"
#include "Tools/Debugging/DebugDrawings.h"
#include "Tools/Settings.h"

#include "libbhuman/bhuman.h"

PROCESS_WIDE_STORAGE(NaoProvider) NaoProvider::theInstance = 0;

NaoProvider::NaoProvider() : gameControlTimeStamp(0)
{
  NaoProvider::theInstance = this;

  OUTPUT(idText, text, "Hi, I am " << Global::getSettings().robot << ".");
  OUTPUT(idRobotname, bin, Global::getSettings().robot);

#ifndef RELEASE
  for(int i = 0; i < JointDataBH::numOfJoints; ++i)
    clippedLastFrame[i] = JointDataBH::off;
#endif
}

NaoProvider::~NaoProvider()
{
  NaoProvider::theInstance = 0;
}

bool NaoProvider::isFrameDataComplete()
{
  return true;
}

void NaoProvider::waitForFrameData()
{
  if(theInstance)
    theInstance->naoBody.wait();
}

void NaoProvider::send()
{
  DEBUG_RESPONSE("module:NaoProvider:lag100", SystemCall::sleep(100););
  DEBUG_RESPONSE("module:NaoProvider:lag200", SystemCall::sleep(200););
  DEBUG_RESPONSE("module:NaoProvider:lag300", SystemCall::sleep(300););
  DEBUG_RESPONSE("module:NaoProvider:lag1000", SystemCall::sleep(1000););
  DEBUG_RESPONSE("module:NaoProvider:lag3000", SystemCall::sleep(3000););
  DEBUG_RESPONSE("module:NaoProvider:lag6000", SystemCall::sleep(6000););
  DEBUG_RESPONSE("module:NaoProvider:segfault", *(volatile char*)0 = 0;);

  DEBUG_RESPONSE("module:NaoProvider:ClippingInfo",
  {
    for(int i = 0; i < JointDataBH::numOfJoints; ++i)
    {
      if(i == JointDataBH::RHipYawPitch) // missing on Nao
        ++i;

      if(theJointRequestBH.angles[i] != JointDataBH::off)
      {
        if(theJointRequestBH.angles[i] > theJointCalibrationBH.joints[i].maxAngle)
        {
          if(clippedLastFrame[i] != theJointCalibrationBH.joints[i].maxAngle)
          {
            char tmp[64];
            sprintf(tmp, "warning: clipped joint %s at %.03f, requested %.03f.", JointDataBH::getName((JointDataBH::Joint)i), toDegrees(theJointCalibrationBH.joints[i].maxAngle), toDegrees(theJointRequestBH.angles[i]));
            OUTPUT(idText, text, tmp);
            clippedLastFrame[i] = theJointCalibrationBH.joints[i].maxAngle;
          }
        }
        else if(theJointRequestBH.angles[i] < theJointCalibrationBH.joints[i].minAngle)
        {
          if(clippedLastFrame[i] != theJointCalibrationBH.joints[i].minAngle)
          {
            char tmp[64];
            sprintf(tmp, "warning: clipped joint %s at %.04f, requested %.03f.", JointDataBH::getName((JointDataBH::Joint)i), toDegrees(theJointCalibrationBH.joints[i].minAngle), toDegrees(theJointRequestBH.angles[i]));
            OUTPUT(idText, text, tmp);
            clippedLastFrame[i] = theJointCalibrationBH.joints[i].minAngle;
          }
        }
        else
          clippedLastFrame[i] = JointDataBH::off;
      }
    }
  });

#ifdef MEASURE_DELAY
  OutTextFile stream("delay.log", true);
  stream << "jointRequest";
  stream << theJointRequestBH.angles[JointDataBH::LHipPitch];
  stream << theJointRequestBH.angles[JointDataBH::LKneePitch];
  stream << theJointRequestBH.angles[JointDataBH::LAnklePitch];
  stream << endl;
#endif

  float* actuators;
  naoBody.openActuators(actuators);
  int j = 0;
  ASSERT(headYawPositionActuator == 0);
  ASSERT(int(JointDataBH::numOfJoints) + 1 == headYawHardnessActuator);

  for(int i = 0; i < JointDataBH::numOfJoints; ++i)
  {
    if(i == JointDataBH::RHipYawPitch) // missing on Nao
      ++i;

    if(theJointRequestBH.angles[i] == JointDataBH::off)
    {
      actuators[j] = 0.0f;
      actuators[j + headYawHardnessActuator] = 0.0f; // hardness
    }
    else
    {
      actuators[j] = (theJointRequestBH.angles[i] + theJointCalibrationBH.joints[i].offset) * float(theJointCalibrationBH.joints[i].sign);
      actuators[j + headYawHardnessActuator] = float(theJointRequestBH.jointHardness.hardness[i]) / 100.f;
    }
    ++j;
  }

  actuators[j] = 0.f;
  actuators[j + headYawHardnessActuator] = float(theJointRequestBH.jointHardness.hardness[0]) / 100.f;
  ++j;
  actuators[j] = 0.f;
  actuators[j + headYawHardnessActuator] = float(theJointRequestBH.jointHardness.hardness[0]) / 100.f;
  ++j;
  j += headYawHardnessActuator;
  ASSERT(j == faceLedRedLeft0DegActuator);

  const LEDRequestBH& ledRequest(theLEDRequestBH);
  bool on = (theJointDataBH.timeStamp / 50 & 8) != 0;
  bool fastOn = (theJointDataBH.timeStamp / 10 & 8) != 0;
  for(int i = 0; i < LEDRequestBH::numOfLEDs; ++i)
    actuators[j++] = (ledRequest.ledStates[i] == LEDRequestBH::on ||
                      (ledRequest.ledStates[i] == LEDRequestBH::blinking && on) ||
                      (ledRequest.ledStates[i] == LEDRequestBH::fastBlinking && fastOn))
                     ? 1.0f : (ledRequest.ledStates[i] == LEDRequestBH::half ? 0.5f : 0.0f);

  actuators[usActuator] = (float) theUSRequestBH.sendMode;

  naoBody.closeActuators();
  naoBody.setTeamInfo(Global::getSettings().teamNumber, Global::getSettings().teamColor, Global::getSettings().playerNumber);
}

void NaoProvider::update(JointDataBH& jointData, SensorDataBH& sensorData)
{
  jointData.timeStamp = sensorData.timeStamp = std::max(jointData.timeStamp + 1, SystemCall::getCurrentSystemTime());

  // Northern Bites pass sensors in via bhwalkprovider instead
  //float* sensors = naoBody.getSensors();

  for(int i = 0; i < JointDataBH::numOfJoints; ++i)
  {
    if(i == JointDataBH::RHipYawPitch)
    {
      jointData.angles[i] = jointData.angles[JointDataBH::LHipYawPitch];
    }
    else
    {
      // Northern Bites undo this transformation after bhwalk executes
      jointData.angles[i] = jointData.angles[i] * theJointCalibrationBH.joints[i].sign - theJointCalibrationBH.joints[i].offset;
    }
  }

#ifdef MEASURE_DELAY
  OutTextFile stream("delay.log", true);
  stream << "timestamp" << SystemCall::getCurrentSystemTime();
  stream << "jointData";
  stream << jointData.angles[JointDataBH::LHipPitch];
  stream << jointData.angles[JointDataBH::LKneePitch];
  stream << jointData.angles[JointDataBH::LAnklePitch];
#endif

  sensorData.data[SensorDataBH::gyroX] *= 12.f / 1600.f;
  sensorData.data[SensorDataBH::gyroY] *= 12.f / 1600.f;

  sensorData.data[SensorDataBH::fsrLFL] = (sensorData.data[SensorDataBH::fsrLFL] + theSensorCalibrationBH.fsrLFLOffset) * theSensorCalibrationBH.fsrLFLGain;
  sensorData.data[SensorDataBH::fsrLFR] = (sensorData.data[SensorDataBH::fsrLFR] + theSensorCalibrationBH.fsrLFROffset) * theSensorCalibrationBH.fsrLFRGain;
  sensorData.data[SensorDataBH::fsrLBL] = (sensorData.data[SensorDataBH::fsrLBL] + theSensorCalibrationBH.fsrLBLOffset) * theSensorCalibrationBH.fsrLBLGain;
  sensorData.data[SensorDataBH::fsrLBR] = (sensorData.data[SensorDataBH::fsrLBR] + theSensorCalibrationBH.fsrLBROffset) * theSensorCalibrationBH.fsrLBRGain;
  sensorData.data[SensorDataBH::fsrRFL] = (sensorData.data[SensorDataBH::fsrRFL] + theSensorCalibrationBH.fsrRFLOffset) * theSensorCalibrationBH.fsrRFLGain;
  sensorData.data[SensorDataBH::fsrRFR] = (sensorData.data[SensorDataBH::fsrRFR] + theSensorCalibrationBH.fsrRFROffset) * theSensorCalibrationBH.fsrRFRGain;
  sensorData.data[SensorDataBH::fsrRBL] = (sensorData.data[SensorDataBH::fsrRBL] + theSensorCalibrationBH.fsrRBLOffset) * theSensorCalibrationBH.fsrRBLGain;
  sensorData.data[SensorDataBH::fsrRBR] = (sensorData.data[SensorDataBH::fsrRBR] + theSensorCalibrationBH.fsrRBROffset) * theSensorCalibrationBH.fsrRBRGain;

#ifdef MEASURE_DELAY
  stream << "sensorData";
  stream << sensorData.data[SensorDataBH::gyroX] << sensorData.data[SensorDataBH::gyroY] << sensorData.data[SensorDataBH::accX] << sensorData.data[SensorDataBH::accY] << sensorData.data[SensorDataBH::accZ];
#endif

  //for(int i = 0; i < KeyStatesBH::numOfKeys; ++i)
    //keyStates.pressed[i] = sensors[j++] != 0;

  // ultasound

  //if(theUSRequestBH.receiveMode != -1)
  //{
    //for(int i = SensorDataBH::usL; i < SensorDataBH::usREnd; ++i)
    //{
      //float data = sensors[i - SensorDataBH::usL + lUsSensor];
      //sensorData.data[i] = data != 0.f ? data * 1000.f : 2550.f;
    //}
    //sensorData.usTimeStamp = theJointDataBH.timeStamp;
    //sensorData.usActuatorMode = (SensorDataBH::UsActuatorMode) theUSRequestBH.receiveMode;
  //}

  PLOT("module:NaoProvider:usLeft", sensorData.data[SensorDataBH::usL]);
  PLOT("module:NaoProvider:usRight", sensorData.data[SensorDataBH::usR]);

  //if(memcmp(&gameControlData, &naoBody.getGameControlData(), sizeof(RoboCupGameControlData)))
  //{
    //gameControlData = naoBody.getGameControlData();
    //gameControlTimeStamp = theJointDataBH.timeStamp;
  //}

#ifndef RELEASE
  JointDataDeg jointDataDeg(jointData);
#endif
  MODIFY("representation:JointDataDeg", jointDataDeg);
}

void NaoProvider::finishFrame()
{
  if(theInstance)
    theInstance->send();
}

void NaoProvider::update(RobotInfoBH& robotInfo)
{
  TeamInfo& team = gameControlData.teams[gameControlData.teams[0].teamNumber == Global::getSettings().teamNumber ? 0 : 1];
  (RobotInfo&) robotInfo = team.players[Global::getSettings().playerNumber - 1];
  robotInfo.number = Global::getSettings().playerNumber;
}

void NaoProvider::update(OwnTeamInfoBH& ownTeamInfo)
{
  (TeamInfo&) ownTeamInfo = gameControlData.teams[gameControlData.teams[0].teamNumber == Global::getSettings().teamNumber ? 0 : 1];
}

void NaoProvider::update(OpponentTeamInfoBH& opponentTeamInfo)
{
  (TeamInfo&) opponentTeamInfo = gameControlData.teams[gameControlData.teams[0].teamNumber == Global::getSettings().teamNumber ? 1 : 0];
}

void NaoProvider::update(GameInfoBH& gameInfo)
{
  memcpy(&(RoboCupGameControlData&) gameInfo, &gameControlData, (char*) gameControlData.teams - (char*) &gameControlData);
  gameInfo.timeLastPackageReceived = gameControlTimeStamp;
}

#endif // TARGET_ROBOT

MAKE_MODULE(NaoProvider, Motion Infrastructure)
