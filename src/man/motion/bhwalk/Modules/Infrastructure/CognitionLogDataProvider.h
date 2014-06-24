/**
* @file CognitionLogDataProvider.h
* This file declares a module that provides data replayed from a log file.
* @author <a href="mailto:Thomas.Roefer@dfki.de">Thomas Röfer</a>
*/

#pragma once
//TODO clean up includes
#include "Tools/ModuleBH/Module.h"
#include "Tools/MessageQueue/InMessage.h"
#include "Tools/Debugging/DebugImages.h"
#include "Tools/Debugging/DebugDrawings3D.h"
#include "LogDataProvider.h"
#include "Representations/Infrastructure/Image.h"
#include "Representations/Infrastructure/FrameInfo.h"
#include "Representations/Configuration/FieldDimensions.h"
#include "Representations/Perception/LinePercept.h"
#include "Representations/Perception/BallPercept.h"
#include "Representations/Perception/GoalPercept.h"
#include "Representations/Perception/CameraMatrix.h"
#include "Representations/Perception/ImageCoordinateSystem.h"
#include "Representations/Modeling/RobotPose.h"
#include "Representations/Modeling/BallModel.h"
#include "Representations/Modeling/RobotsModel.h"
#include "Representations/Perception/JPEGImage.h"
#include "Representations/Infrastructure/JointData.h"
#include "Representations/Infrastructure/RobotHealth.h"
#include "Representations/Infrastructure/Thumbnail.h"
#include "Representations/Modeling/SideConfidence.h"
#include "Representations/Modeling/ObstacleModel.h"
#include "Representations/BehaviorControl/BehaviorControlOutput.h"
#include "Representations/Infrastructure/SensorData.h"
#include "Representations/Modeling/CombinedWorldModel.h"
#include "Representations/MotionControl/MotionInfo.h"
#include "Representations/Perception/ColorReference.h"
#include "Representations/Perception/FieldBoundary.h"
#include "Representations/Infrastructure/Thumbnail.h"
#include "Representations/Perception/ObstacleSpots.h"
#include "Representations/Modeling/ObstacleWheel.h"
#include "Representations/Perception/BodyContour.h"

MODULE(CognitionLogDataProvider)
  PROVIDES_WITH_MODIFY_AND_OUTPUT(CameraInfoBH)
  USES(CameraInfoBH)
  PROVIDES_WITH_OUTPUT(ImageBH)
  REQUIRES(FieldDimensionsBH)
  PROVIDES_WITH_MODIFY_AND_OUTPUT(FrameInfoBH)
  USES(FrameInfoBH)
  REQUIRES(OwnTeamInfoBH)
  PROVIDES_WITH_MODIFY_AND_OUTPUT(LinePerceptBH)
  PROVIDES_WITH_MODIFY_AND_OUTPUT_AND_DRAW(BallPerceptBH)
  PROVIDES_WITH_MODIFY_AND_OUTPUT_AND_DRAW(GoalPerceptBH)
  PROVIDES_WITH_MODIFY_AND_OUTPUT_AND_DRAW(BallModelBH)
  PROVIDES_WITH_MODIFY_AND_OUTPUT(FilteredJointDataBH)
  PROVIDES_WITH_DRAW(RobotsModelBH)
  PROVIDES_WITH_MODIFY_AND_OUTPUT_AND_DRAW(GroundTruthRobotPoseBH)
  PROVIDES_WITH_MODIFY_AND_OUTPUT_AND_DRAW(GroundTruthBallModelBH)
  PROVIDES_WITH_DRAW(GroundTruthRobotsModelBH)
  PROVIDES_WITH_MODIFY_AND_OUTPUT_AND_DRAW(CameraMatrixBH)
  REQUIRES(ImageBH)
  PROVIDES_WITH_MODIFY_AND_OUTPUT_AND_DRAW(ImageCoordinateSystemBH)
  PROVIDES_WITH_MODIFY_AND_OUTPUT(RobotPoseBH)
  PROVIDES_WITH_MODIFY_AND_DRAW(SideConfidenceBH)
  PROVIDES_WITH_DRAW(ObstacleModelBH)
  PROVIDES(FilteredSensorDataBH)
  PROVIDES_WITH_MODIFY(BehaviorControlOutputBH)
  PROVIDES_WITH_MODIFY_AND_OUTPUT_AND_DRAW(MotionRequestBH)
  PROVIDES_WITH_MODIFY(HeadMotionRequestBH)
  PROVIDES_WITH_MODIFY(BehaviorLEDRequestBH)
  PROVIDES_WITH_MODIFY(ArmMotionRequestBH)
  PROVIDES_WITH_DRAW(CombinedWorldModelBH)
  PROVIDES_WITH_MODIFY(MotionInfoBH)
  PROVIDES_WITH_MODIFY_AND_DRAW(RobotHealthBH)
  PROVIDES_WITH_MODIFY_AND_DRAW(FieldBoundaryBH)
  PROVIDES_WITH_MODIFY_AND_OUTPUT(ActivationGraphBH)
  PROVIDES_WITH_DRAW(ObstacleWheelBH)
  PROVIDES(ColorReferenceBH)
  PROVIDES_WITH_DRAW(ObstacleSpotsBH)
  PROVIDES_WITH_OUTPUT_AND_DRAW(ThumbnailBH)
  PROVIDES_WITH_DRAW(BodyContourBH)
END_MODULE

class CognitionLogDataProvider : public CognitionLogDataProviderBase, public LogDataProvider
{
private:
  static PROCESS_WIDE_STORAGE(CognitionLogDataProvider) theInstance; /**< Points to the only instance of this class in this process or is 0 if there is none. */
  bool frameDataComplete; /**< Were all messages of the current frame received? */
  BehaviorControlOutputBH behaviorControlOutput;

  DECLARE_DEBUG_IMAGE(corrected);

#define DISTANCE 300

  UPDATE2(ImageBH,
  {
    DECLARE_DEBUG_DRAWING3D("representation:ImageBH", "camera");
    IMAGE3D("representation:ImageBH", DISTANCE, 0, 0, 0, 0, 0,
            DISTANCE * theCameraInfoBH.width / theCameraInfoBH.focalLength,
            DISTANCE * theCameraInfo.height / theCameraInfoBH.focalLength,
            _Image);
    DEBUG_RESPONSE("representation:JPEGImage", OUTPUT(idJPEGImage, bin, JPEGImage(_Image)););
  })
  UPDATE(CameraInfoBH)
  UPDATE(FrameInfoBH)
  UPDATE2(FieldBoundaryBH, _FieldBoundary.width = theImageBH.width;);
  UPDATE(ThumbnailBH)
  UPDATE(ActivationGraphBH)
  UPDATE(ObstacleWheelBH)
  UPDATE(BodyContourBH);
  UPDATE(ObstacleSpotsBH)
  UPDATE(RobotHealthBH);
  UPDATE(ColorReferenceBH)

  UPDATE(BehaviorControlOutputBH);
  void update(MotionRequestBH& motionRequest) {motionRequest = behaviorControlOutput.motionRequest;}
  void update(HeadMotionRequestBH& headMotionRequest) {headMotionRequest = behaviorControlOutput.headMotionRequest;}
  void update(BehaviorLEDRequestBH& behaviorLEDRequest) {behaviorLEDRequest = behaviorControlOutput.behaviorLEDRequest;}
  void update(ArmMotionRequestBH& armMotionRequest) {armMotionRequest = behaviorControlOutput.armMotionRequest;}

  UPDATE(CombinedWorldModelBH)
  UPDATE(ObstacleModelBH)
  UPDATE(FilteredSensorDataBH)
  UPDATE(LinePerceptBH)
  UPDATE(BallPerceptBH)
  UPDATE(GoalPerceptBH)
  UPDATE(BallModelBH)
  UPDATE(FilteredJointDataBH)
  UPDATE(RobotsModelBH)
  UPDATE2(GroundTruthRobotPoseBH, _GroundTruthRobotPose.timestamp = theFrameInfoBH.time;)
  UPDATE(GroundTruthBallModelBH)
  UPDATE(GroundTruthRobotsModelBH)
  UPDATE(CameraMatrixBH)
  UPDATE2(ImageCoordinateSystemBH,
  {
    _ImageCoordinateSystem.setCameraInfo(theCameraInfoBH);
    DECLARE_DEBUG_DRAWING("loggedHorizon", "drawingOnImage"); // displays the horizon
    ARROW("loggedHorizon",
          _ImageCoordinateSystem.origin.x,
          _ImageCoordinateSystem.origin.y,
          _ImageCoordinateSystem.origin.x + _ImageCoordinateSystem.rotation.c[0].x * 50,
          _ImageCoordinateSystem.origin.y + _ImageCoordinateSystem.rotation.c[0].y * 50,
          0, Drawings::ps_solid, ColorRGBA(255, 0, 0));
    ARROW("loggedHorizon",
          _ImageCoordinateSystem.origin.x,
          _ImageCoordinateSystem.origin.y,
          _ImageCoordinateSystem.origin.x + _ImageCoordinateSystem.rotation.c[1].x * 50,
          _ImageCoordinateSystem.origin.y + _ImageCoordinateSystem.rotation.c[1].y * 50,
          0, Drawings::ps_solid, ColorRGBA(255, 0, 0));
    COMPLEX_DEBUG_IMAGE(corrected,
    {
      ImageBH* i = (ImageBH*) representationBuffer[idImage];
      if(i)
      {
        INIT_DEBUG_IMAGE_BLACK(corrected, theCameraInfoBH.width, theCameraInfo.height);
        int yDest = -_ImageCoordinateSystem.toCorrectedCenteredNeg(0, 0).y;
        for(int ySrc = 0; ySrc < theCameraInfo.height; ++ySrc)
          for(int yDest2 = -_ImageCoordinateSystem.toCorrectedCenteredNeg(0, ySrc).y; yDest <= yDest2; ++yDest)
          {
            int xDest = -_ImageCoordinateSystem.toCorrectedCenteredNeg(0, ySrc).x;
            for(int xSrc = 0; xSrc < theCameraInfoBH.width; ++xSrc)
            {
              for(int xDest2 = -_ImageCoordinateSystem.toCorrectedCenteredNeg(xSrc, ySrc).x; xDest <= xDest2; ++xDest)
              {
                DEBUG_IMAGE_SET_PIXEL_YUV(corrected, xDest + int(theCameraInfoBH.opticalCenter.x + 0.5f),
                yDest + int(theCameraInfoBH.opticalCenter.y + 0.5f),
                (*i)[ySrc][xSrc].y,
                (*i)[ySrc][xSrc].cb,
                (*i)[ySrc][xSrc].cr);
              }
            }
          }
        SEND_DEBUG_IMAGE(corrected);
      }
    });
  })
  UPDATE2(RobotPoseBH,{_RobotPose.draw(theOwnTeamInfoBH.teamColor != TEAM_BLUE);})
  UPDATE(SideConfidenceBH)
  UPDATE(MotionInfoBH)


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
  CognitionLogDataProvider();

  /**
  * Destructor.
  */
  ~CognitionLogDataProvider();

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
};
