/*
* @file KickEngine.cpp
* This file implements a module that creates motions.
* @author <A href="mailto:judy@tzi.de">Judith Müller</A>
*/

#include "KickEngine.h"

#include "Tools/Debugging/DebugDrawings.h"
#include "Tools/Debugging/DebugDrawings3D.h"
#include "Representations/MotionControl/KickRequest.h"
#include "Tools/Debugging/Modify.h"
#include "KickEngineParameters.h"
#include "Tools/InverseKinematic.h"
#include "Tools/Math/Pose3D.h"
#include <cerrno>

KickEngine::KickEngine() :
compensate(false),
compensated(false),
timeSinceLastPhase(0)
{
  params.reserve(10);

  // Below is used for reading in multiple kicks. As of now we only use one kick, so hardcode!
  // char dirname[260];

  // sprintf(dirname, "%s/Config/KickEngine/", File::getBHDir());
  // DIR* dir = opendir(dirname);
  // ASSERT(dir);
  // struct dirent* file = readdir(dir);

  // while(file != NULL)
  // {
  //   char name[260];
  //   sprintf(name, "KickEngine/%s", file->d_name);

  //   if(strstr(name, ".kmc"))
  //   {
  //     InMapFile stream(name);
  //     ASSERT(stream.exists());

  //     KickEngineParameters parameters;
  //     stream >> parameters;

  //     sprintf(name, "%s", file->d_name);
  //     for(int i = 0; i < 260; i++)
  //     {
  //       if(name[i] == '.') name[i] = 0;
  //     }
  //     strcpy(parameters.name, name);

  //     if(KickRequest::getKickMotionFromName(parameters.name) < KickRequest::none)
  //     {
  //       params.push_back(parameters);
  //     }
  //     else
  //     {
  //       OUTPUT(idText, text, "Warning: KickRequest is missing the id for " << parameters.name);
  //       fprintf(stderr, "Warning: KickRequest is missing the id for %s \n", parameters.name);
  //     }
  //   }
  //   file = readdir(dir);
  // }
  // closedir(dir);

  InMapFile stream(ModuleBase::config_path + "kickForward.kmc");
  ASSERT(stream.exists());

  KickEngineParameters parameters;
  stream >> parameters;

  // sprintf(name, "%s", file->d_name);
  // for(int i = 0; i < 260; i++)
  // {
  //     if(name[i] == '.') name[i] = 0;
  // }
  strcpy(parameters.name, "kickForward.kmc");

  if(KickRequest::getKickMotionFromName(parameters.name) < KickRequest::none)
  {
      params.push_back(parameters);
  }
  else
  {
      std::cout << "COULD NOT PARSE ANY KICKS FROM 'kickForward.kmc'!" << std::endl;
      OUTPUT(idText, text, "Warning: KickRequest is missing the id for " << parameters.name);
      fprintf(stderr, "Warning: KickRequest is missing the id for %s \n", parameters.name);
  }

  for(int i = 0; i < KickRequest::numOfKickMotionIDs - 2; ++i)
  {
    int id = -1;
    for(unsigned int p = 0; p < params.size(); ++p)
    {
      if(KickRequest::getKickMotionFromName(&params[p].name[0]) == i)
      {
        id = i;
        break;
      }
    }
    if(id == -1)
    {
      OUTPUT(idText, text, "Warning: The kick motion file for id " << KickRequest::getName((KickRequest::KickMotionID) i) << " is missing.");
      fprintf(stderr, "Warning: The kick motion file for id %s is missing. \n", KickRequest::getName((KickRequest::KickMotionID) i));
    }
  }

  //This is needed for adding new kicks
#ifndef RELEASE
  KickEngineParameters newKickMotion;
  strcpy(newKickMotion.name, "newKick");
  params.push_back(newKickMotion);
#endif
};

void KickEngine::update(KickEngineOutput& kickEngineOutput)
{
  if(theMotionSelectionBH.ratios[MotionRequestBH::kick] > 0.f)
  {
    data.setCycleTime(theFrameInfoBH.cycleTime);

    if(theMotionSelectionBH.ratios[MotionRequestBH::kick] < 1.f && !compensated) compensate = true;

    data.setRobotModel(theRobotModelBH);

    if(data.sitOutTransitionDisturbance(compensate, compensated, theFilteredSensorDataBH, kickEngineOutput, theWalkingEngineStandOutputBH, theFrameInfoBH))
    {
      if(data.activateNewMotion(theMotionRequestBH.kickRequest, kickEngineOutput.isLeavingPossible) && theMotionRequestBH.motion == MotionRequestBH::kick)
      {
        data.initData(compensated, theFrameInfoBH, theMotionRequestBH, theRobotDimensionsBH, params, theFilteredJointDataBH, theTorsoMatrixBH);
        data.setCurrentKickRequest(theMotionRequestBH);
        data.setExecutedKickRequest(kickEngineOutput.executedKickRequest);

        data.internalIsLeavingPossible = false;
        kickEngineOutput.isLeavingPossible = false;

        kickEngineOutput.odometryOffset = Pose2DBH();

        for(int i = JointDataBH::LShoulderPitch; i < JointDataBH::numOfJoints; ++i)
          kickEngineOutput.jointHardness.hardness[i] = 100;

        kickEngineOutput.isStable = true;
      }//this gotta go to config file and be more common

      if(data.checkPhaseTime(theFrameInfoBH, theRobotDimensionsBH, theFilteredJointDataBH, theTorsoMatrixBH))
      {
        data.calcPhaseState();
        data.calcPositions(kickEngineOutput, theFilteredJointDataBH);
        data.setStaticReference();
        timeSinceLastPhase = theFrameInfoBH.time;
      }
      else
      {
        kickEngineOutput.isLeavingPossible = true;
        data.internalIsLeavingPossible = true;
      }

      //  if(data.isMotionAlmostOver()) //last three phases are unstable
      //    kickEngineOutput.isStable = false;

      if(data.calcJoints(kickEngineOutput, theRobotDimensionsBH, theHeadJointRequestBH))
      {
#ifndef RELEASE
        data.debugFormMode(params);
#endif
        data.balanceCOM(kickEngineOutput, theRobotDimensionsBH, theMassCalibrationBH, theFilteredJointDataBH);
        data.calcJoints(kickEngineOutput, theRobotDimensionsBH, theHeadJointRequestBH);
        data.mirrorIfNecessary(kickEngineOutput);
      }
      data.addGyroBalance(kickEngineOutput, theJointCalibrationBH, theFilteredSensorDataBH, theMotionSelectionBH.ratios[MotionRequestBH::kick]);
    }
  }
  else
  {
    compensated = false;
  }

  data.setEngineActivation(theMotionSelectionBH.ratios[MotionRequestBH::kick]);
  data.ModifyData(theMotionRequestBH.kickRequest, kickEngineOutput, params);
}

MAKE_MODULE(KickEngine, Motion Control)

