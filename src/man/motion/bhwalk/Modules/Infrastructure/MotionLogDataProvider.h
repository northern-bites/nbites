/**
* @file MotionLogDataProvider.h
* This file declares a module that provides data replayed from a log file.
* @author <a href="mailto:Thomas.Roefer@dfki.de">Thomas Röfer</a>
*/

#pragma once

#include "Tools/ModuleBH/Module.h"
#include "Representations/Infrastructure/JointData.h"
#include "Representations/Infrastructure/JointDataDeg.h"
#include "Representations/Infrastructure/SensorData.h"
#include "Representations/Infrastructure/KeyStates.h"
#include "Representations/Infrastructure/FrameInfo.h"
#include "Representations/Infrastructure/GameInfo.h"
#include "Representations/Infrastructure/RobotInfo.h"
#include "Representations/Infrastructure/TeamInfo.h"
#include "Representations/MotionControl/OdometryData.h"
#include "Representations/Sensing/OrientationData.h"
#include "Tools/MessageQueue/InMessage.h"
#include "LogDataProvider.h"

MODULE(MotionLogDataProvider)
  PROVIDES_WITH_MODIFY_AND_OUTPUT(JointDataBH)
  PROVIDES_WITH_MODIFY_AND_OUTPUT(SensorDataBH)
  PROVIDES_WITH_MODIFY_AND_OUTPUT(KeyStatesBH)
  PROVIDES_WITH_MODIFY(FrameInfoBH)
  PROVIDES_WITH_MODIFY_AND_OUTPUT(OdometryDataBH)
  PROVIDES_WITH_MODIFY_AND_OUTPUT(GroundTruthOdometryDataBH)
  PROVIDES_WITH_MODIFY_AND_OUTPUT(OrientationDataBH)
  PROVIDES_WITH_MODIFY_AND_OUTPUT(GroundTruthOrientationDataBH)
  PROVIDES_WITH_MODIFY_AND_OUTPUT_AND_DRAW(GameInfoBH)
  PROVIDES_WITH_MODIFY_AND_OUTPUT_AND_DRAW(OwnTeamInfoBH)
  PROVIDES_WITH_MODIFY_AND_OUTPUT_AND_DRAW(OpponentTeamInfoBH)
  PROVIDES_WITH_MODIFY(RobotInfoBH)
  PROVIDES(FilteredSensorDataBH)
  PROVIDES(FilteredJointDataBH)
END_MODULE

class MotionLogDataProvider : public MotionLogDataProviderBase, public LogDataProvider
{
private:
  static PROCESS_WIDE_STORAGE(MotionLogDataProvider) theInstance; /**< Points to the only instance of this class in this process or is 0 if there is none. */
  bool frameDataComplete; /**< Were all messages of the current frame received? */

#ifndef RELEASE
  OdometryDataBH lastOdometryData;
#endif

  /**
  * The method is called for every incoming debug message by handleMessage.
  * @param message An interface to read the message from the queue.
  * @return Was the message handled?
  */
  bool handleMessage2(InMessage& message);

public:
  /**
  * Default constructor.
  */
  MotionLogDataProvider();

  /**
  * Destructor.
  */
  ~MotionLogDataProvider();

  /**
  * The method is called for every incoming debug message.
  * @param message An interface to read the message from the queue.
  * @return Was the message handled?
  */
  static bool handleMessage(InMessage& message);

  /**
  * The method returns whether idProcessFinished was received.
  * @return Were all messages of the current frame received?
  */
  static bool isFrameDataComplete();

  UPDATE(FrameInfoBH)
  UPDATE2(JointDataBH,
  {
    JointDataDeg jointDataDeg(_JointData);
    MODIFY("representation:JointDataDeg", jointDataDeg);
  })
  UPDATE(KeyStatesBH)
  UPDATE(OdometryDataBH)
  UPDATE(SensorDataBH)
  UPDATE(FilteredSensorDataBH)
  UPDATE(FilteredJointDataBH)
  UPDATE(OrientationDataBH)
  UPDATE(GameInfoBH)
  UPDATE(OwnTeamInfoBH)
  UPDATE(OpponentTeamInfoBH)
  UPDATE(RobotInfoBH)

  void update(GroundTruthOdometryDataBH&);
  void update(GroundTruthOrientationDataBH&);
};
