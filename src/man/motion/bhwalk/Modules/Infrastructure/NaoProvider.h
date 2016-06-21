/**
* @file Modules/Infrastructure/NaoProvider.h
* The file declares a module that provides information from the Nao via DCM.
* @author <a href="mailto:Thomas.Roefer@dfki.de">Thomas Röfer</a>
*/

#pragma once

#include "Tools/Module/Module.h"
#include "Representations/Configuration/JointCalibration.h"
#include "Representations/Infrastructure/JointData.h"
#include "Representations/Infrastructure/SensorData.h"
#include "Representations/Configuration/SensorCalibration.h"
#include "Representations/Infrastructure/KeyStates.h"
#include "Representations/Infrastructure/FrameInfo.h"
#include "Representations/Infrastructure/RobotInfo.h"
#include "Representations/Infrastructure/TeamInfo.h"
#include "Representations/Infrastructure/GameInfo.h"
#include "Representations/Infrastructure/LEDRequest.h"
#include "Representations/Infrastructure/USRequest.h"
#include "Platform/Linux/NaoBody.h"

MODULE(NaoProvider)
  REQUIRES(JointCalibrationBH)
  REQUIRES(JointDataBH)
  REQUIRES(LEDRequestBH)
  REQUIRES(SensorCalibrationBH)
  REQUIRES(USRequestBH)
  PROVIDES_WITH_MODIFY_AND_OUTPUT(KeyStatesBH)
  PROVIDES_WITH_MODIFY(FrameInfoBH)
  PROVIDES_WITH_MODIFY_AND_OUTPUT(RobotInfoBH)
  PROVIDES_WITH_MODIFY_AND_OUTPUT_AND_DRAW(OwnTeamInfoBH)
  PROVIDES_WITH_MODIFY_AND_OUTPUT_AND_DRAW(OpponentTeamInfoBH)
  PROVIDES_WITH_MODIFY_AND_OUTPUT_AND_DRAW(GameInfoBH)
  USES(JointRequestBH) // Will be accessed in send()
END_MODULE


#ifdef TARGET_ROBOT

/**
* @class NaoProvider
* A module that provides information from the Nao.
*/
class NaoProvider : public NaoProviderBase
{
private:
  NaoBody naoBody;
  KeyStatesBH keyStates; /**< The last key states received. */
  RoboCupGameControlData gameControlData; /**< The last game control data received. */
  unsigned gameControlTimeStamp; /**< The time when the last gameControlData was received (kind of). */

#ifndef RELEASE
  float clippedLastFrame[JointDataBH::numOfJoints]; /**< Array that indicates whether a certain joint value was clipped in the last frame (and what was the value)*/
#endif

  /**
  * The function sends a command to the Nao.
  */
  void send();

public:
  static PROCESS_WIDE_STORAGE(NaoProvider) theInstance; /**< The only instance of this module. */

  void update(JointDataBH& jointData, SensorDataBH& sensorData);
  void update(KeyStatesBH& keyStates) {keyStates = this->keyStates;}
  void update(FrameInfoBH& frameInfo) {frameInfo.time = theJointDataBH.timeStamp; frameInfo.cycleTime = 0.01f;}
  void update(RobotInfoBH& robotInfo);
  void update(OwnTeamInfoBH& ownTeamInfo);
  void update(OpponentTeamInfoBH& opponentTeamInfo);
  void update(GameInfoBH& gameInfo);

  /**
  * Constructor.
  */
  NaoProvider();

  /**
  * Destructor.
  */
  ~NaoProvider();

  /**
  * The method is called by process Motion to send the requests to the Nao.
  */
  static void finishFrame();

  static bool isFrameDataComplete();

  static void waitForFrameData();
};

#else
//TARGET_ROBOT not defined here (Simulator).

class NaoProvider : public NaoProviderBase
{
private:
  void update(JointDataBH& jointData) {}
  void update(SensorDataBH& sensorData) {}
  void update(KeyStatesBH& keyStates) {}
  void update(FrameInfoBH& frameInfo) {}
  void update(RobotInfoBH& robotInfo) {}
  void update(OwnTeamInfoBH& ownTeamInfo) {}
  void update(OpponentTeamInfoBH& opponentTeamInfo) {}
  void update(GameInfoBH& gameInfo) {}
  void send();

public:
  NaoProvider() {}
  ~NaoProvider() {}
  static void finishFrame() {}
  static bool isFrameDataComplete() {return true;}
  static void waitForFrameData() {}
};

#endif
