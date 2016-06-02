/**
 * @file FallDownStateDetector.cpp
 *
 * This file implements a module that provides information about the current state of the robot's body.
 *
 * @author <a href="mailto:maring@informatik.uni-bremen.de">Martin Ring</a>
 */

#include "FallDownStateDetector.h"
#include "Representations/Infrastructure/JointData.h"
#include "Tools/Debugging/DebugDrawings.h"

using namespace std;

//#define BHDEBUG

PROCESS_WIDE_STORAGE(FallDownStateDetector) FallDownStateDetector::theInstance = 0;

FallDownStateDetector::FallDownStateDetector() : lastFallDetected(0)
{
  fallDownAngleX *= pi_180;
  fallDownAngleY *= pi_180;
  onGroundAngle  *= pi_180;
  staggeringAngleX *= pi_180;
  staggeringAngleY *= pi_180;
}

void FallDownStateDetector::update(FallDownStateBH& fallDownState)
{
  DECLARE_PLOT("module:FallDownStateDetector:accelerationAngleXZ");
  DECLARE_PLOT("module:FallDownStateDetector:accelerationAngleYZ");

  // Buffer data:
  buffers[accX].add(theFilteredSensorDataBH.data[SensorDataBH::accX]);
  buffers[accY].add(theFilteredSensorDataBH.data[SensorDataBH::accY]);
  buffers[accZ].add(theFilteredSensorDataBH.data[SensorDataBH::accZ]);

  // Compute average acceleration values and angles:
  float accXaverage(buffers[accX].getAverage());
  float accYaverage(buffers[accY].getAverage());
  float accZaverage(buffers[accZ].getAverage());
  float accelerationAngleXZ(atan2(accZaverage, accXaverage));
  float accelerationAngleYZ(atan2(accZaverage, accYaverage));
  MODIFY("module:FallDownStateDetector:accX",  accXaverage);
  MODIFY("module:FallDownStateDetector:accY",  accYaverage);
  MODIFY("module:FallDownStateDetector:accZ",  accZaverage);
  MODIFY("module:FallDownStateDetector:accAngleXZ", accelerationAngleXZ);
  MODIFY("module:FallDownStateDetector:accAngleYZ", accelerationAngleYZ);
  PLOT("module:FallDownStateDetector:accelerationAngleXZ", accelerationAngleXZ);
  PLOT("module:FallDownStateDetector:accelerationAngleYZ", accelerationAngleYZ);

  fallDownState.odometryRotationOffset = 0;

  if(isCalibrated() && !specialSpecialAction())
  {
    if(theFrameInfoBH.getTimeSince(lastFallDetected) <= fallTime)
    {
#ifdef BHDEBUG
      std::cout << "[BH DEBUG] This is changing to falling NOW" << std::endl;
#endif
      fallDownState.state = FallDownStateBH::falling;
    }
    else if((abs(theFilteredSensorDataBH.data[SensorDataBH::angleX]) <= staggeringAngleX - pi_180
             && abs(theFilteredSensorDataBH.data[SensorDataBH::angleY]) <= staggeringAngleY - pi_180)
            || (fallDownState.state == FallDownStateBH::upright && !isStaggering()))
    {
      fallDownState.state = FallDownStateBH::upright;
      fallDownState.direction = FallDownStateBH::none;
      fallDownState.sidewards = FallDownStateBH::noot;
    }
    else if(fallDownState.state == FallDownStateBH::staggering && isFalling())
    {
#ifdef BHDEBUG
        std::cout << "[BH DEBUG] This is changing to falling NOW 2" << std::endl;
#endif
      lastFallDetected = theFrameInfoBH.time;
      fallDownState.state = FallDownStateBH::falling;
      fallDownState.direction = directionOf(theFilteredSensorDataBH.data[SensorDataBH::angleX], theFilteredSensorDataBH.data[SensorDataBH::angleY]);
      if(fallDownState.sidewards != FallDownStateBH::fallen)
      {
        fallDownState.sidewards = sidewardsOf(fallDownState.direction);
      }
    }
    else if((isUprightOrStaggering(fallDownState)
             && isStaggering())
            || (fallDownState.state == FallDownStateBH::staggering
                && abs(theFilteredSensorDataBH.data[SensorDataBH::angleX]) <= staggeringAngleX - pi_180
                && abs(theFilteredSensorDataBH.data[SensorDataBH::angleY]) <= staggeringAngleY - pi_180))
    {
#ifdef BHDEBUG
      std::cout << "[BH DEBUG] This is changing to falling NOW 3" << std::endl;
      std::cout << "[BH DEBUG] isStaggering() " << isStaggering() << std::endl;
      std::cout << "[BH DEBUG] fallDownState.state " << fallDownState.state  << std::endl;
      std::cout << "[BH DEBUG] abs(theFilteredSensorDataBH.data[SensorDataBH::angleX])" << abs(theFilteredSensorDataBH.data[SensorDataBH::angleX]) << isStaggering() << std::endl;
      std::cout << "[BH DEBUG] isUprightOrStaggering(fallDownState):" << isUprightOrStaggering(fallDownState) << std::endl;
#endif
      fallDownState.state = FallDownStateBH::staggering;
      fallDownState.direction = directionOf(theFilteredSensorDataBH.data[SensorDataBH::angleX], theFilteredSensorDataBH.data[SensorDataBH::angleY]);
      if(fallDownState.sidewards != FallDownStateBH::fallen)
      {
        fallDownState.sidewards = sidewardsOf(fallDownState.direction);
      }
    }
    else
    {
#ifdef BHDEBUG
      std::cout << "[BH DEBUG] This is changing to falling NOW 4 UNDEFINED" << std::endl;
#endif
      fallDownState.state = FallDownStateBH::undefined;

      if(abs(accelerationAngleXZ) < 0.5f)
      {
        fallDownState.direction = FallDownStateBH::front;
        if(theMotionInfoBH.motion != MotionRequestBH::getUp)
        {
#ifdef BHDEBUG
          std::cout << "[BH DEBUG] This is changing to falling NOW 5 GROUND" << std::endl;
          std::cout << "[BH DEBUG] Acc angle" << accelerationAngleXZ << std::endl;
#endif
          fallDownState.state = FallDownStateBH::onGround;
          if(fallDownState.sidewards == FallDownStateBH::leftwards)
          {
            fallDownState.odometryRotationOffset = pi_2;
            fallDownState.sidewards = FallDownStateBH::fallen;
          }
          else if(fallDownState.sidewards == FallDownStateBH::rightwards)
          {
            fallDownState.odometryRotationOffset = -pi_2;
            fallDownState.sidewards = FallDownStateBH::fallen;
          }
        }
      }
      else if(abs(accelerationAngleXZ) > 2.5f)
      {

        fallDownState.direction = FallDownStateBH::back;
        if(theMotionInfoBH.motion != MotionRequestBH::getUp)
        {
#ifdef BHDEBUG
          std::cout << "[BH DEBUG] This is changing to falling NOW 6 onground" << std::endl;
#endif
          fallDownState.state = FallDownStateBH::onGround;
          if(fallDownState.sidewards == FallDownStateBH::leftwards)
          {
            fallDownState.odometryRotationOffset = -pi_2;
            fallDownState.sidewards = FallDownStateBH::fallen;
          }
          else if(fallDownState.sidewards == FallDownStateBH::rightwards)
          {
            fallDownState.odometryRotationOffset = pi_2;
            fallDownState.sidewards = FallDownStateBH::fallen;
          }
        }
      }
      else if(abs(accelerationAngleYZ) < 0.5f)
      {
        fallDownState.direction = FallDownStateBH::left;
        if(theMotionInfoBH.motion != MotionRequestBH::getUp)
        {
#ifdef BHDEBUG
          std::cout << "[BH DEBUG] This is changing to falling NOW 7 grrruond" << std::endl;
#endif
          fallDownState.state = FallDownStateBH::onGround;

          if(fallDownState.sidewards != FallDownStateBH::fallen)
          {
            fallDownState.sidewards = FallDownStateBH::leftwards;
          }
        }
      }
      else if(abs(accelerationAngleYZ) > 2.5f)
      {
        fallDownState.direction = FallDownStateBH::right;
        if(theMotionInfoBH.motion != MotionRequestBH::getUp)
        {
#ifdef BHDEBUG
          std::cout << "[BH DEBUG] This is changing to falling NOW 8 ground" << std::endl;
#endif
          fallDownState.state = FallDownStateBH::onGround;

          if(fallDownState.sidewards != FallDownStateBH::fallen)
          {
            fallDownState.sidewards = FallDownStateBH::rightwards;
          }
        }
      }

    }
  }
  else
  {
    // std::cout << "[BH DEBUG] This is changing to falling NOW 9 UNDEF" << std::endl;
    // std::cout << "[BH DEBUG] iscalibrated:" << isCalibrated() << std::endl;
    // std::cout << "[BH DEBUG] specialSpecialAction:" << specialSpecialAction() << std::endl;
    fallDownState.state = FallDownStateBH::undefined;
  }
}


bool FallDownStateDetector::isUprightOrStaggering(FallDownStateBH& fallDownState)
{
  return fallDownState.state == FallDownStateBH::upright
         || fallDownState.state == FallDownStateBH::staggering;
}

bool FallDownStateDetector::specialSpecialAction()
{
  return (theMotionInfoBH.motion == MotionRequestBH::specialAction
          && (theMotionInfoBH.specialActionRequest.specialAction == SpecialActionRequest::playDead));
}

bool FallDownStateDetector::isStaggering()
{
  return abs(theFilteredSensorDataBH.data[SensorDataBH::angleX]) >= staggeringAngleX + pi_180
         || abs(theFilteredSensorDataBH.data[SensorDataBH::angleY]) >= staggeringAngleY + pi_180;
}

bool FallDownStateDetector::isFalling()
{
  return abs(theFilteredSensorDataBH.data[SensorDataBH::angleX]) >= fallDownAngleX
         || abs(theFilteredSensorDataBH.data[SensorDataBH::angleY]) >= fallDownAngleY;
}

bool FallDownStateDetector::isCalibrated()
{
  return theInertiaSensorDataBH.calibrated;
}

FallDownStateBH::Direction FallDownStateDetector::directionOf(float angleX, float angleY)
{
  if(abs(angleX) > abs(angleY) + 0.2f)
  {
    if(angleX < 0.f) return FallDownStateBH::left;
    else return FallDownStateBH::right;
  }
  else
  {
    if(angleY > 0.f) return FallDownStateBH::front;
    else return FallDownStateBH::back;
  }
}

FallDownStateBH::Sidestate FallDownStateDetector::sidewardsOf(FallDownStateBH::Direction dir)
{
  switch(dir)
  {
  case FallDownStateBH::left:
    return FallDownStateBH::leftwards;
  case FallDownStateBH::right:
    return FallDownStateBH::rightwards;
  default:
    return FallDownStateBH::noot;
  }
}

MAKE_MODULE(FallDownStateDetector, Sensing)
