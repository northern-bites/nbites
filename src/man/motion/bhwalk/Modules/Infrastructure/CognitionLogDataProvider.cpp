/**
* @file CognitionLogDataProvider.cpp
* This file implements a module that provides data replayed from a log file.
* @author <a href="mailto:Thomas.Roefer@dfki.de">Thomas Röfer</a>
*/

#include "CognitionLogDataProvider.h"
#include "Tools/Settings.h"
#include <vector>

PROCESS_WIDE_STORAGE(CognitionLogDataProvider) CognitionLogDataProvider::theInstance = 0;

#define ASSIGN(target, source) \
  ALLOC(target) \
  (target&) *representationBuffer[id##target] = (target&) *representationBuffer[id##source];

CognitionLogDataProvider::CognitionLogDataProvider() :
  LogDataProvider(),
  frameDataComplete(false)
{
  theInstance = this;
}

CognitionLogDataProvider::~CognitionLogDataProvider()
{
  theInstance = 0;
}

bool CognitionLogDataProvider::handleMessage(InMessage& message)
{
  return theInstance && theInstance->handleMessage2(message);
}

bool CognitionLogDataProvider::isFrameDataComplete()
{
  if(!theInstance)
    return true;
  else if(theInstance->frameDataComplete)
  {
    OUTPUT(idLogResponse, bin, '\0');
    theInstance->frameDataComplete = false;
    return true;
  }
  else
    return false;
}

bool CognitionLogDataProvider::handleMessage2(InMessage& message)
{
  switch(message.getMessageID())
  {
    HANDLE2(ImageBH,
    {
      ALLOC(FrameInfoBH)
      FrameInfoBH& frameInfo = (FrameInfoBH&) *representationBuffer[idFrameInfo];
      const ImageBH& image = (const ImageBH&) *representationBuffer[idImage];
      frameInfo.cycleTime = (float) (image.timeStamp - frameInfo.time) * 0.001f;
      frameInfo.time = image.timeStamp;
    })
    HANDLE(CameraInfoBH)
    HANDLE(FrameInfoBH)
    HANDLE(LinePerceptBH)
    HANDLE(ActivationGraphBH)
    HANDLE(BallPerceptBH)
    HANDLE(GoalPerceptBH)
    HANDLE(FieldBoundaryBH)
    HANDLE(ObstacleSpotsBH)
    HANDLE(BallModelBH)
    HANDLE(ObstacleWheelBH)
    HANDLE(BodyContourBH)
    HANDLE(ThumbnailBH)
	HANDLE(RobotHealthBH)
    HANDLE2(FilteredSensorDataBH,
    {
      ALLOC(FrameInfoBH)
      FrameInfoBH& frameInfo = (FrameInfoBH&) *representationBuffer[idFrameInfo];
      const FilteredSensorDataBH& filteredSensorData = (const FilteredSensorDataBH&) *representationBuffer[idFilteredSensorData];
      frameInfo.cycleTime = (float) (filteredSensorData.timeStamp - frameInfo.time) * 0.001f;
      frameInfo.time = filteredSensorData.timeStamp;
    })
    HANDLE2(BehaviorControlOutputBH,
    {
      behaviorControlOutput = (const BehaviorControlOutputBH&) *representationBuffer[idBehaviorControlOutput];
    })
    HANDLE(ObstacleModelBH)
    HANDLE(FilteredJointDataBH)
    HANDLE(RobotsModelBH)
    HANDLE(CombinedWorldModelBH)
    HANDLE2(GroundTruthRobotPoseBH, ASSIGN(RobotPoseBH, GroundTruthRobotPoseBH))
    HANDLE2(GroundTruthBallModelBH, ASSIGN(BallModelBH, GroundTruthBallModelBH))
    HANDLE2(GroundTruthRobotsModelBH, ASSIGN(RobotsModelBH, GroundTruthRobotsModelBH))
    HANDLE(CameraMatrixBH)

    HANDLE(ImageCoordinateSystemBH)
    HANDLE(RobotPoseBH)
    HANDLE(SideConfidenceBH)
    HANDLE(MotionInfoBH)
    HANDLE(ColorReferenceBH)

  case idProcessFinished:
    frameDataComplete = true;
    return true;

  case idStopwatch:
  {
    const int size = message.getMessageSize();
    std::vector<unsigned char> data;
    data.resize(size);
    message.bin.read(&data[0], size);
    Global::getDebugOut().bin.write(&data[0], size);
    Global::getDebugOut().finishMessage(idStopwatch);
    return true;
  }
  case idJPEGImage:
    ALLOC(ImageBH)
    {
      JPEGImage jpegImage;
      message.bin >> jpegImage;
      jpegImage.toImage((ImageBH&) *representationBuffer[idImage]);
    }
    ALLOC(FrameInfoBH)
    ((FrameInfoBH&) *representationBuffer[idFrameInfo]).time = ((ImageBH&) *representationBuffer[idImage]).timeStamp;
    return true;

  default:
    return false;
  }
}

MAKE_MODULE(CognitionLogDataProvider, Cognition Infrastructure)
