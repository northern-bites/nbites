/**
* @file TeamDataSender.cpp
* Implementation of module TeamDataSender
* @author Colin Graf
*/

#include "TeamDataSender.h"
#include "Tools/Team.h"
#include "Representations/Modeling/ObstacleClusters.h"

MAKE_MODULE(TeamDataSender, Cognition Infrastructure)

void TeamDataSender::update(TeamDataSenderOutputBH& teamDataSenderOutput)
{
  if(theTeamMateDataBH.sendThisFrame)
  {
    ++sendFrames;

    // Own pose information and ball observation:
    TEAM_OUTPUT(idTeamMateRobotPose, bin, RobotPoseCompressed(theRobotPoseBH));
    TEAM_OUTPUT(idTeamMateSideConfidence, bin, theSideConfidenceBH);
    TEAM_OUTPUT(idTeamMateBallModel, bin, BallModelCompressed(theBallModelBH));

    // Obstacle stuff
    TEAM_OUTPUT(idObstacleClusters, bin, ObstacleClustersCompressed(theObstacleClustersBH, maxNumberOfObstacleClustersToSend));
    TEAM_OUTPUT(idTeamMateRobotsModel, bin, RobotsModelCompressed(theRobotsModelBH, maxNumberOfRobotsToSend));
    TEAM_OUTPUT(idTeamMateObstacleModel, bin,ObstacleModelCompressed(theObstacleModelBH, maxNumberOfObstaclesToSend));

    // Robot status
    TEAM_OUTPUT(idTeamMateIsPenalized, bin, (theRobotInfoBH.penalty != PENALTY_NONE));
    TEAM_OUTPUT(idTeamMateHasGroundContact, bin, theGroundContactStateBH.contact);
    TEAM_OUTPUT(idTeamMateIsUpright, bin, (theFallDownStateBH.state == theFallDownStateBH.upright));
    if(theGroundContactStateBH.contact)
      TEAM_OUTPUT(idTeamMateTimeSinceLastGroundContact, bin, theFrameInfoBH.time);
    TEAM_OUTPUT(idTeamCameraHeight, bin, theCameraMatrixBH.translation.z);

    if(sendFrames % 20 == 0)
      TEAM_OUTPUT(idRobotHealth, bin, theRobotHealthBH);
  }
}
