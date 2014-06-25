/**
* @file Modules/Infrastructure/RobotHealthProvider.h
* This file implements a module that provides information about the robot's health.
* @author <a href="mailto:timlaue@informatik.uni-bremen.de">Tim Laue</a>
*/

#include <algorithm>
#include <numeric>

#include "RobotHealthProvider.h"
#include "Tools/Settings.h"
#include "Tools/Streams/InStreams.h"

RobotHealthProvider::RobotHealthProvider() :
  lastExecutionTime(0),
  lastRelaxedHealthComputation(0),
  startBatteryLow(0),
  lastBatteryLevel(1),
  batteryVoltageFalling(false),
  highTemperatureSince(0)
#ifdef TARGET_ROBOT
  , lastBodyTemperatureReadTime(0),
  lastWlanCheckedTime(0)
#endif
{
}

void RobotHealthProvider::update(RobotHealthBH& robotHealth)
{
  // count percepts
  if(theBallPerceptBH.ballWasSeen)
    ++robotHealth.ballPercepts;
  robotHealth.linePercepts += theLinePerceptBH.lines.size();
  robotHealth.goalPercepts += theGoalPerceptBH.goalPosts.size();

  // Transfer information from other process:
  robotHealth = theMotionRobotHealthBH;
  // Compute frame rate of cognition process:
  unsigned now = SystemCall::getCurrentSystemTime();
  if(lastExecutionTime != 0)
    timeBuffer.add(now - lastExecutionTime);
  robotHealth.cognitionFrameRate = timeBuffer.getSum() ? 1000.0f / (static_cast<float>(timeBuffer.getSum()) / timeBuffer.getNumberOfEntries()) : 0.0f;
  lastExecutionTime = now;

  // read cpu and mainboard temperature
#ifdef TARGET_ROBOT
  if(theFrameInfoBH.getTimeSince(lastBodyTemperatureReadTime) > 10 * 1000)
  {
    lastBodyTemperatureReadTime = theFrameInfoBH.time;
    float cpuTemperature, mbTemperature;
    naoBody.getTemperature(cpuTemperature, mbTemperature);
    robotHealth.cpuTemperature = (unsigned char)cpuTemperature;
    robotHealth.boardTemperature = (unsigned char)mbTemperature;
  }
  if(theFrameInfoBH.getTimeSince(lastWlanCheckedTime) > 10 * 1000)
  {
    lastWlanCheckedTime = theFrameInfoBH.time;
    robotHealth.wlan = naoBody.getWlanStatus();
  }
#endif

  if(theFrameInfoBH.getTimeSince(lastRelaxedHealthComputation) > 5000)
  {
    lastRelaxedHealthComputation = theFrameInfoBH.time;

    // transfer maximal temperature, battery level and total current from SensorDataBH:
    robotHealth.batteryLevel = (unsigned char)((theFilteredSensorDataBH.data[SensorDataBH::batteryLevel] == SensorDataBH::off ? 1.f : theFilteredSensorDataBH.data[SensorDataBH::batteryLevel]) * 100.f);
	robotHealth.maxJointTemperature = *std::max_element(theFilteredSensorDataBH.temperatures,theFilteredSensorDataBH.temperatures+JointDataBH::numOfJoints);	
	robotHealth.totalCurrent=std::accumulate(theFilteredSensorDataBH.currents,theFilteredSensorDataBH.currents+JointDataBH::numOfJoints,0.0f);
	
    // Add cpu load, memory load and robot name:
    float memoryUsage, load[3];
    SystemCall::getLoad(memoryUsage, load);
    robotHealth.load[0] = (unsigned char)(load[0] * 10.f);
    robotHealth.load[1] = (unsigned char)(load[1] * 10.f);
    robotHealth.load[2] = (unsigned char)(load[2] * 10.f);
    robotHealth.memoryUsage = (unsigned char)(memoryUsage * 100.f);
    robotHealth.robotName = Global::getSettings().robot;

    std::string wavName = Global::getSettings().robot.c_str();
    wavName.append(".wav");

    //battery warning
    if(lastBatteryLevel < robotHealth.batteryLevel)
      batteryVoltageFalling = false;
    else if(lastBatteryLevel > robotHealth.batteryLevel)
      batteryVoltageFalling = true;
    if(robotHealth.batteryLevel < batteryLow)
    {
      if(batteryVoltageFalling && theFrameInfoBH.getTimeSince(startBatteryLow) > 1000)
      {
        if(enableName)
          SystemCall::playSound(wavName.c_str());
        SystemCall::playSound("lowBattery.wav");
        //next warning in 90 seconds
        startBatteryLow = theFrameInfoBH.time + 30000;
        batteryVoltageFalling = false;
      }
    }
    else if(startBatteryLow < theFrameInfoBH.time)
      startBatteryLow = theFrameInfoBH.time;
    lastBatteryLevel = robotHealth.batteryLevel;

    //temperature warning
    if(robotHealth.maxJointTemperature > temperatureHigh)
    {
      if(theFrameInfoBH.getTimeSince(highTemperatureSince) > 1000)
      {
        if(enableName)
          SystemCall::playSound(wavName.c_str());
        SystemCall::playSound("heat.wav");
        highTemperatureSince = theFrameInfoBH.time + 20000;
      }
    }
    else if(highTemperatureSince < theFrameInfoBH.time)
      highTemperatureSince = theFrameInfoBH.time;
  }
}

MAKE_MODULE(RobotHealthProvider, Cognition Infrastructure)
