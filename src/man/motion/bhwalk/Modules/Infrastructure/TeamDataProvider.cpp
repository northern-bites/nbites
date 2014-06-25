/**
 * @file TeamDataProvider.cpp
 * This file implements a module that provides the data received by team communication.
 * @author <a href="mailto:Thomas.Roefer@dfki.de">Thomas Röfer</a>
 */

#include "TeamDataProvider.h"
#include "Tools/Settings.h"
#include "Tools/Team.h"

/**
 * This macro unpacks compressed representations. It reads
 * representationCompressed from the MessageQueue and unpacks it into
 * teamMateData.array[robotNumber].
 */
#define UNPACK(representation, array) \
  representation##Compressed the##representation##Compressed; \
  message.bin >> the##representation##Compressed; \
  theTeamMateDataBH.array[robotNumber] = the##representation##Compressed;

/**
 * This macro converts a timeStamp into local time via ntp.
 */
#define REMOTE_TO_LOCAL_TIME(timeStamp) \
  if(timeStamp) { timeStamp = ntp.getRemoteTimeInLocalTime(timeStamp); }

PROCESS_WIDE_STORAGE(TeamDataProvider) TeamDataProvider::theInstance = 0;

TeamDataProvider::TeamDataProvider() :
  timeStamp(0), robotNumber(-1), lastSentTimeStamp(0)
{
  theInstance = this;
}

TeamDataProvider::~TeamDataProvider()
{
  theInstance = 0;
}

void TeamDataProvider::update(TeamMateDataBH& teamMateData)
{
  DECLARE_PLOT("module:TeamDataProvider:ntpOffset1"); // 1-5: players
  DECLARE_PLOT("module:TeamDataProvider:ntpOffset2");
  DECLARE_PLOT("module:TeamDataProvider:ntpOffset3");
  DECLARE_PLOT("module:TeamDataProvider:ntpOffset4");
  DECLARE_PLOT("module:TeamDataProvider:ntpOffset5");

  PLOT("module:TeamDataProvider:ntpOffset1", ntp.timeSyncBuffers[1].bestOffset);
  PLOT("module:TeamDataProvider:ntpOffset2", ntp.timeSyncBuffers[2].bestOffset);
  PLOT("module:TeamDataProvider:ntpOffset3", ntp.timeSyncBuffers[3].bestOffset);
  PLOT("module:TeamDataProvider:ntpOffset4", ntp.timeSyncBuffers[4].bestOffset);
  PLOT("module:TeamDataProvider:ntpOffset5", ntp.timeSyncBuffers[5].bestOffset);

  teamMateData = theTeamMateDataBH;
  teamMateData.currentTimestamp = theFrameInfoBH.time;
  teamMateData.numOfConnectedTeamMates = 0;
  teamMateData.firstTeamMate = TeamMateDataBH::numOfPlayers;
  for(int i = TeamMateDataBH::firstPlayer; i < TeamMateDataBH::numOfPlayers; ++i)
  {
    teamMateData.isActive[i] = false;
    teamMateData.isFullyActive[i] = false;
    // Check, if network connection is working (-> connected):
    if(teamMateData.timeStamps[i] && theFrameInfoBH.getTimeSince(teamMateData.timeStamps[i]) < static_cast<int>(teamMateData.networkTimeout))
    {
      teamMateData.numOfConnectedTeamMates++;
      // Check, if team mate is not penalized (-> active):
      if(!teamMateData.isPenalized[i] && theOwnTeamInfoBH.players[i - 1].penalty == PENALTY_NONE)
      {
        teamMateData.numOfActiveTeamMates++;
        teamMateData.isActive[i] = true;
        if(teamMateData.numOfActiveTeamMates == 1)
          teamMateData.firstTeamMate = i;
        // Check, if team mate has not been fallen down or lost ground contact (-> fully active):
        if(teamMateData.hasGroundContact[i] && teamMateData.isUpright[i])
        {
          teamMateData.numOfFullyActiveTeamMates++;
          teamMateData.isFullyActive[i] = true;
        }
      }
    }
  }
  if(teamMateData.numOfConnectedTeamMates)
    teamMateData.wasConnected = theTeamMateDataBH.wasConnected = true;
  teamMateData.sendThisFrame = (ntp.doSynchronization(theFrameInfoBH.time, Global::getTeamOut()) ||
                                theFrameInfoBH.getTimeSince(lastSentTimeStamp) >= 200) // TODO config file?
#ifdef TARGET_ROBOT
                               && !(theMotionRequestBH.motion == MotionRequestBH::specialAction && theMotionRequestBH.specialActionRequest.specialAction == SpecialActionRequest::playDead)
                               && !(theMotionInfoBH.motion == MotionRequestBH::specialAction && theMotionInfoBH.specialActionRequest.specialAction == SpecialActionRequest::playDead)
#endif
                               ;
  if(teamMateData.sendThisFrame)
    lastSentTimeStamp = theFrameInfoBH.time;
}

void TeamDataProvider::handleMessages(MessageQueue& teamReceiver)
{
  if(theInstance)
  {
    teamReceiver.handleAllMessages(*theInstance);
    TEAM_OUTPUT(idRobot, bin, theInstance->theRobotInfoBH.number);
  }

  teamReceiver.clear();
}

bool TeamDataProvider::handleMessage(InMessage& message)
{
  /*
  The robotNumber and the three flags hasGroundContact, isUpright and isPenalized should always be updated.
   */
  switch(message.getMessageID())
  {
    case idNTPHeader:
      VERIFY(ntp.handleMessage(message));
      timeStamp = ntp.receiveTimeStamp;
      return false;
    case idNTPIdentifier:
    case idNTPRequest:
    case idNTPResponse:
      return ntp.handleMessage(message);

    case idRobot:
      message.bin >> robotNumber;
      if(robotNumber != theRobotInfoBH.number)
        if(robotNumber >= TeamMateDataBH::firstPlayer && robotNumber < TeamMateDataBH::numOfPlayers)
          theTeamMateDataBH.timeStamps[robotNumber] = timeStamp;
      return true;

    case idGroundTruthBallModel:
    {
      Vector2BH<> position;
      message.bin >> theGroundTruthBallModelBH.timeWhenLastSeen >> position;
      REMOTE_TO_LOCAL_TIME(theGroundTruthBallModelBH.timeWhenLastSeen);
      if(theOwnTeamInfoBH.teamColor == TEAM_BLUE)
        position *= -1;
      theGroundTruthBallModelBH.lastPerception = theGroundTruthRobotPoseBH.invert() * position;
      theGroundTruthBallModelBH.estimate.position = theGroundTruthBallModelBH.lastPerception;
    }
      return true;

    case idGroundTruthRobotPose:
    {
      char teamColor,
          id;
      unsigned timeStamp;
      Pose2DBH robotPose;
      message.bin >> teamColor >> id >> timeStamp >> robotPose;
      if(teamColor == (int) theOwnTeamInfoBH.teamColor && id == theRobotInfoBH.number)
      {
        if(theOwnTeamInfoBH.teamColor == TEAM_BLUE)
          robotPose = Pose2DBH(pi) + robotPose;
        (Pose2DBH&) theGroundTruthRobotPoseBH = robotPose;
      }
    }
      return true;

    case idTeamMateIsPenalized:
      if(robotNumber != theRobotInfoBH.number)
        if(robotNumber >= TeamMateDataBH::firstPlayer && robotNumber < TeamMateDataBH::numOfPlayers)
          message.bin >> theTeamMateDataBH.isPenalized[robotNumber];
      return true;

    case idTeamMateHasGroundContact:
      if(robotNumber != theRobotInfoBH.number)
        if(robotNumber >= TeamMateDataBH::firstPlayer && robotNumber < TeamMateDataBH::numOfPlayers)
        {
          message.bin >> theTeamMateData.hasGroundContact[robotNumber];
          // This is a potentially evil quick workaround that should be replaced by a better handling of ground contacts of team mates
          // at many different places in our code! For a detailed problem description, ask Tim.
          if(!theTeamMateData.hasGroundContact[robotNumber])
            theTeamMateData.hasGroundContact[robotNumber] = theFrameInfoBH.getTimeSince(theTeamMateDataBH.timeLastGroundContact[robotNumber]) < 2000;
        }
      return true;

    case idTeamMateIsUpright:
      if(robotNumber != theRobotInfoBH.number)
        if(robotNumber >= TeamMateDataBH::firstPlayer && robotNumber < TeamMateDataBH::numOfPlayers)
          message.bin >> theTeamMateDataBH.isUpright[robotNumber];
      return true;

    case idTeamMateTimeSinceLastGroundContact:
      if(robotNumber != theRobotInfoBH.number)
        if(robotNumber >= TeamMateDataBH::firstPlayer && robotNumber < TeamMateDataBH::numOfPlayers)
        {
          message.bin >> theTeamMateDataBH.timeLastGroundContact[robotNumber];
          REMOTE_TO_LOCAL_TIME(theTeamMateDataBH.timeLastGroundContact[robotNumber]);
        }
      return true;
  }

  /*
  The messages in the following switch block should only be updated
  if hasGroundContact == true and isPenalized == false, because the information of this message
  can only be reliable if the robot is actively playing.
   */
  if(!theTeamMateDataBH.isPenalized[robotNumber] && theTeamMateData.hasGroundContact[robotNumber])
  {
    switch(message.getMessageID())
    {
      case idTeamMateBallModel:
        if(robotNumber != theRobotInfoBH.number)
          if(robotNumber >= TeamMateDataBH::firstPlayer && robotNumber < TeamMateDataBH::numOfPlayers)
          {
            UNPACK(BallModelBH, ballModels);
            BallModelBH& ballModel = theTeamMateDataBH.ballModels[robotNumber];
            REMOTE_TO_LOCAL_TIME(ballModel.timeWhenLastSeen);
            REMOTE_TO_LOCAL_TIME(ballModel.timeWhenDisappeared);
          }
        return true;

      case idTeamMateObstacleModel:
        if(robotNumber != theRobotInfoBH.number)
          if(robotNumber >= TeamMateDataBH::firstPlayer && robotNumber < TeamMateDataBH::numOfPlayers)
          {
            UNPACK(ObstacleModelBH, obstacleModels);
          }
        return true;

      case idTeamMateRobotsModel:
        if(robotNumber != theRobotInfoBH.number)
          if(robotNumber >= TeamMateDataBH::firstPlayer && robotNumber < TeamMateDataBH::numOfPlayers)
          {
            UNPACK(RobotsModelBH, robotsModels);
            for(size_t i = 0; i < theTeamMateDataBH.robotsModels[robotNumber].robots.size(); i++)
            {
              REMOTE_TO_LOCAL_TIME(theTeamMateDataBH.robotsModels[robotNumber].robots[i].timeStamp);
            }
          }
        return true;

      case idObstacleClusters:
        if(robotNumber != theRobotInfoBH.number)
        {
          if(robotNumber >= TeamMateDataBH::firstPlayer && robotNumber < TeamMateDataBH::numOfPlayers)
          {
            UNPACK(ObstacleClustersBH, obstacleClusters);
          }
        }
        return true;

      case idTeamMateRobotPose:
        if(robotNumber != theRobotInfoBH.number)
          if(robotNumber >= TeamMateDataBH::firstPlayer && robotNumber < TeamMateDataBH::numOfPlayers)
          {
            UNPACK(RobotPoseBH, robotPoses);
          }
        return true;

      case idTeamMateSideConfidence:
        if(robotNumber != theRobotInfoBH.number)
          if(robotNumber >= TeamMateDataBH::firstPlayer && robotNumber < TeamMateDataBH::numOfPlayers)
            message.bin >> theTeamMateDataBH.robotsSideConfidence[robotNumber];
        return true;

      case idTeamMateBehaviorStatus:
        if(robotNumber != theRobotInfoBH.number)
          if(robotNumber >= TeamMateDataBH::firstPlayer && robotNumber < TeamMateDataBH::numOfPlayers)
            message.bin >> theTeamMateDataBH.behaviorStatus[robotNumber];
        return true;

      case idTeamHeadControl:
        if(robotNumber != theRobotInfoBH.number)
          if(robotNumber >= TeamMateDataBH::firstPlayer && robotNumber < TeamMateDataBH::numOfPlayers)
            message.bin >> theTeamMateDataBH.teamHeadControlStates[robotNumber];
        return true;

      case idTeamCameraHeight:
        if(robotNumber != theRobotInfoBH.number)
          if(robotNumber >= TeamMateDataBH::firstPlayer && robotNumber < TeamMateDataBH::numOfPlayers)
            message.bin >> theTeamMateDataBH.cameraHeights[robotNumber];
        return true;

      case idTeamMateFieldCoverage:
        if(robotNumber != theRobotInfoBH.number)
          if(robotNumber >= TeamMateDataBH::firstPlayer && robotNumber < TeamMateDataBH::numOfPlayers)
          {
            FieldCoverageBH::GridInterval& gridInterval = theTeamMateDataBH.fieldCoverages[robotNumber];
            message.bin >> gridInterval;
            REMOTE_TO_LOCAL_TIME(gridInterval.timestamp);
          }
        return true;
    }
  }

  return true;
}

MAKE_MODULE(TeamDataProvider, Cognition Infrastructure)
