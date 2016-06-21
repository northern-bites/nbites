/**
 * @file bhuman.cpp
 * Implementation of a NaoQi module that provides basic ipc NaoQi DCM access via semaphore and shared memory.
 */

#include <sys/types.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <semaphore.h>
#include <csignal>
#include <sys/resource.h>
#include <ctime>
#include <cstring>

#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wconversion"
#endif
#include <alcore/altypes.h>
#include <alcore/alerror.h>
#include <alcommon/albroker.h>
#include <alcommon/alproxy.h>
#include <alproxies/dcmproxy.h>
#include <alproxies/almemoryproxy.h>
#ifdef __clang__
#pragma clang diagnostic pop
#endif

#include "bhuman.h"

static const char* sensorNames[] =
{
  "Device/SubDeviceList/HeadYaw/Position/Sensor/Value",
  "Device/SubDeviceList/HeadYaw/ElectricCurrent/Sensor/Value",
  "Device/SubDeviceList/HeadYaw/Temperature/Sensor/Value",
  "Device/SubDeviceList/HeadPitch/Position/Sensor/Value",
  "Device/SubDeviceList/HeadPitch/ElectricCurrent/Sensor/Value",
  "Device/SubDeviceList/HeadPitch/Temperature/Sensor/Value",
  "Device/SubDeviceList/LShoulderPitch/Position/Sensor/Value",
  "Device/SubDeviceList/LShoulderPitch/ElectricCurrent/Sensor/Value",
  "Device/SubDeviceList/LShoulderPitch/Temperature/Sensor/Value",
  "Device/SubDeviceList/LShoulderRoll/Position/Sensor/Value",
  "Device/SubDeviceList/LShoulderRoll/ElectricCurrent/Sensor/Value",
  "Device/SubDeviceList/LShoulderRoll/Temperature/Sensor/Value",
  "Device/SubDeviceList/LElbowYaw/Position/Sensor/Value",
  "Device/SubDeviceList/LElbowYaw/ElectricCurrent/Sensor/Value",
  "Device/SubDeviceList/LElbowYaw/Temperature/Sensor/Value",
  "Device/SubDeviceList/LElbowRoll/Position/Sensor/Value",
  "Device/SubDeviceList/LElbowRoll/ElectricCurrent/Sensor/Value",
  "Device/SubDeviceList/LElbowRoll/Temperature/Sensor/Value",
  "Device/SubDeviceList/RShoulderPitch/Position/Sensor/Value",
  "Device/SubDeviceList/RShoulderPitch/ElectricCurrent/Sensor/Value",
  "Device/SubDeviceList/RShoulderPitch/Temperature/Sensor/Value",
  "Device/SubDeviceList/RShoulderRoll/Position/Sensor/Value",
  "Device/SubDeviceList/RShoulderRoll/ElectricCurrent/Sensor/Value",
  "Device/SubDeviceList/RShoulderRoll/Temperature/Sensor/Value",
  "Device/SubDeviceList/RElbowYaw/Position/Sensor/Value",
  "Device/SubDeviceList/RElbowYaw/ElectricCurrent/Sensor/Value",
  "Device/SubDeviceList/RElbowYaw/Temperature/Sensor/Value",
  "Device/SubDeviceList/RElbowRoll/Position/Sensor/Value",
  "Device/SubDeviceList/RElbowRoll/ElectricCurrent/Sensor/Value",
  "Device/SubDeviceList/RElbowRoll/Temperature/Sensor/Value",
  "Device/SubDeviceList/LHipYawPitch/Position/Sensor/Value",
  "Device/SubDeviceList/LHipYawPitch/ElectricCurrent/Sensor/Value",
  "Device/SubDeviceList/LHipYawPitch/Temperature/Sensor/Value",
  "Device/SubDeviceList/LHipRoll/Position/Sensor/Value",
  "Device/SubDeviceList/LHipRoll/ElectricCurrent/Sensor/Value",
  "Device/SubDeviceList/LHipRoll/Temperature/Sensor/Value",
  "Device/SubDeviceList/LHipPitch/Position/Sensor/Value",
  "Device/SubDeviceList/LHipPitch/ElectricCurrent/Sensor/Value",
  "Device/SubDeviceList/LHipPitch/Temperature/Sensor/Value",
  "Device/SubDeviceList/LKneePitch/Position/Sensor/Value",
  "Device/SubDeviceList/LKneePitch/ElectricCurrent/Sensor/Value",
  "Device/SubDeviceList/LKneePitch/Temperature/Sensor/Value",
  "Device/SubDeviceList/LAnklePitch/Position/Sensor/Value",
  "Device/SubDeviceList/LAnklePitch/ElectricCurrent/Sensor/Value",
  "Device/SubDeviceList/LAnklePitch/Temperature/Sensor/Value",
  "Device/SubDeviceList/LAnkleRoll/Position/Sensor/Value",
  "Device/SubDeviceList/LAnkleRoll/ElectricCurrent/Sensor/Value",
  "Device/SubDeviceList/LAnkleRoll/Temperature/Sensor/Value",
  "Device/SubDeviceList/RHipRoll/Position/Sensor/Value",
  "Device/SubDeviceList/RHipRoll/ElectricCurrent/Sensor/Value",
  "Device/SubDeviceList/RHipRoll/Temperature/Sensor/Value",
  "Device/SubDeviceList/RHipPitch/Position/Sensor/Value",
  "Device/SubDeviceList/RHipPitch/ElectricCurrent/Sensor/Value",
  "Device/SubDeviceList/RHipPitch/Temperature/Sensor/Value",
  "Device/SubDeviceList/RKneePitch/Position/Sensor/Value",
  "Device/SubDeviceList/RKneePitch/ElectricCurrent/Sensor/Value",
  "Device/SubDeviceList/RKneePitch/Temperature/Sensor/Value",
  "Device/SubDeviceList/RAnklePitch/Position/Sensor/Value",
  "Device/SubDeviceList/RAnklePitch/ElectricCurrent/Sensor/Value",
  "Device/SubDeviceList/RAnklePitch/Temperature/Sensor/Value",
  "Device/SubDeviceList/RAnkleRoll/Position/Sensor/Value",
  "Device/SubDeviceList/RAnkleRoll/ElectricCurrent/Sensor/Value",
  "Device/SubDeviceList/RAnkleRoll/Temperature/Sensor/Value",

  "Device/SubDeviceList/InertialSensor/GyrX/Sensor/Value",
  "Device/SubDeviceList/InertialSensor/GyrY/Sensor/Value",
  "Device/SubDeviceList/InertialSensor/GyrRef/Sensor/Value",
  "Device/SubDeviceList/InertialSensor/AccelerometerX/Sensor/Value",
  "Device/SubDeviceList/InertialSensor/AccelerometerY/Sensor/Value",
  "Device/SubDeviceList/InertialSensor/AccelerometerZ/Sensor/Value",
  "Device/SubDeviceList/Battery/Charge/Sensor/Value",
  "Device/SubDeviceList/LFoot/FSR/FrontLeft/Sensor/Value",
  "Device/SubDeviceList/LFoot/FSR/FrontRight/Sensor/Value",
  "Device/SubDeviceList/LFoot/FSR/RearLeft/Sensor/Value",
  "Device/SubDeviceList/LFoot/FSR/RearRight/Sensor/Value",
  "Device/SubDeviceList/RFoot/FSR/FrontLeft/Sensor/Value",
  "Device/SubDeviceList/RFoot/FSR/FrontRight/Sensor/Value",
  "Device/SubDeviceList/RFoot/FSR/RearLeft/Sensor/Value",
  "Device/SubDeviceList/RFoot/FSR/RearRight/Sensor/Value",
  "Device/SubDeviceList/US/Left/Sensor/Value",
  "Device/SubDeviceList/US/Left/Sensor/Value1",
  "Device/SubDeviceList/US/Left/Sensor/Value2",
  "Device/SubDeviceList/US/Left/Sensor/Value3",
  "Device/SubDeviceList/US/Left/Sensor/Value4",
  "Device/SubDeviceList/US/Left/Sensor/Value5",
  "Device/SubDeviceList/US/Left/Sensor/Value6",
  "Device/SubDeviceList/US/Left/Sensor/Value7",
  "Device/SubDeviceList/US/Left/Sensor/Value8",
  "Device/SubDeviceList/US/Left/Sensor/Value9",
  "Device/SubDeviceList/US/Right/Sensor/Value",
  "Device/SubDeviceList/US/Right/Sensor/Value1",
  "Device/SubDeviceList/US/Right/Sensor/Value2",
  "Device/SubDeviceList/US/Right/Sensor/Value3",
  "Device/SubDeviceList/US/Right/Sensor/Value4",
  "Device/SubDeviceList/US/Right/Sensor/Value5",
  "Device/SubDeviceList/US/Right/Sensor/Value6",
  "Device/SubDeviceList/US/Right/Sensor/Value7",
  "Device/SubDeviceList/US/Right/Sensor/Value8",
  "Device/SubDeviceList/US/Right/Sensor/Value9",
  "Device/SubDeviceList/InertialSensor/AngleX/Sensor/Value",
  "Device/SubDeviceList/InertialSensor/AngleY/Sensor/Value",
  "Device/SubDeviceList/RFoot/Bumper/Right/Sensor/Value",
  "Device/SubDeviceList/RFoot/Bumper/Left/Sensor/Value",
  "Device/SubDeviceList/LFoot/Bumper/Right/Sensor/Value",
  "Device/SubDeviceList/LFoot/Bumper/Left/Sensor/Value",
  "Device/SubDeviceList/ChestBoard/Button/Sensor/Value",
};

static const char* actuatorNames[] =
{
  "HeadYaw/Position/Actuator/Value",
  "HeadPitch/Position/Actuator/Value",
  "LShoulderPitch/Position/Actuator/Value",
  "LShoulderRoll/Position/Actuator/Value",
  "LElbowYaw/Position/Actuator/Value",
  "LElbowRoll/Position/Actuator/Value",
  "RShoulderPitch/Position/Actuator/Value",
  "RShoulderRoll/Position/Actuator/Value",
  "RElbowYaw/Position/Actuator/Value",
  "RElbowRoll/Position/Actuator/Value",
  "LHipYawPitch/Position/Actuator/Value",
  "LHipRoll/Position/Actuator/Value",
  "LHipPitch/Position/Actuator/Value",
  "LKneePitch/Position/Actuator/Value",
  "LAnklePitch/Position/Actuator/Value",
  "LAnkleRoll/Position/Actuator/Value",
  "RHipRoll/Position/Actuator/Value",
  "RHipPitch/Position/Actuator/Value",
  "RKneePitch/Position/Actuator/Value",
  "RAnklePitch/Position/Actuator/Value",
  "RAnkleRoll/Position/Actuator/Value",
  "LHand/Position/Actuator/Value",
  "RHand/Position/Actuator/Value",

  "HeadYaw/Hardness/Actuator/Value",
  "HeadPitch/Hardness/Actuator/Value",
  "LShoulderPitch/Hardness/Actuator/Value",
  "LShoulderRoll/Hardness/Actuator/Value",
  "LElbowYaw/Hardness/Actuator/Value",
  "LElbowRoll/Hardness/Actuator/Value",
  "RShoulderPitch/Hardness/Actuator/Value",
  "RShoulderRoll/Hardness/Actuator/Value",
  "RElbowYaw/Hardness/Actuator/Value",
  "RElbowRoll/Hardness/Actuator/Value",
  "LHipYawPitch/Hardness/Actuator/Value",
  "LHipRoll/Hardness/Actuator/Value",
  "LHipPitch/Hardness/Actuator/Value",
  "LKneePitch/Hardness/Actuator/Value",
  "LAnklePitch/Hardness/Actuator/Value",
  "LAnkleRoll/Hardness/Actuator/Value",
  "RHipRoll/Hardness/Actuator/Value",
  "RHipPitch/Hardness/Actuator/Value",
  "RKneePitch/Hardness/Actuator/Value",
  "RAnklePitch/Hardness/Actuator/Value",
  "RAnkleRoll/Hardness/Actuator/Value",
  "LHand/Hardness/Actuator/Value",
  "RHand/Hardness/Actuator/Value",

  "Face/Led/Red/Left/0Deg/Actuator/Value",
  "Face/Led/Red/Left/45Deg/Actuator/Value",
  "Face/Led/Red/Left/90Deg/Actuator/Value",
  "Face/Led/Red/Left/135Deg/Actuator/Value",
  "Face/Led/Red/Left/180Deg/Actuator/Value",
  "Face/Led/Red/Left/225Deg/Actuator/Value",
  "Face/Led/Red/Left/270Deg/Actuator/Value",
  "Face/Led/Red/Left/315Deg/Actuator/Value",
  "Face/Led/Green/Left/0Deg/Actuator/Value",
  "Face/Led/Green/Left/45Deg/Actuator/Value",
  "Face/Led/Green/Left/90Deg/Actuator/Value",
  "Face/Led/Green/Left/135Deg/Actuator/Value",
  "Face/Led/Green/Left/180Deg/Actuator/Value",
  "Face/Led/Green/Left/225Deg/Actuator/Value",
  "Face/Led/Green/Left/270Deg/Actuator/Value",
  "Face/Led/Green/Left/315Deg/Actuator/Value",
  "Face/Led/Blue/Left/0Deg/Actuator/Value",
  "Face/Led/Blue/Left/45Deg/Actuator/Value",
  "Face/Led/Blue/Left/90Deg/Actuator/Value",
  "Face/Led/Blue/Left/135Deg/Actuator/Value",
  "Face/Led/Blue/Left/180Deg/Actuator/Value",
  "Face/Led/Blue/Left/225Deg/Actuator/Value",
  "Face/Led/Blue/Left/270Deg/Actuator/Value",
  "Face/Led/Blue/Left/315Deg/Actuator/Value",
  "Face/Led/Red/Right/0Deg/Actuator/Value",
  "Face/Led/Red/Right/45Deg/Actuator/Value",
  "Face/Led/Red/Right/90Deg/Actuator/Value",
  "Face/Led/Red/Right/135Deg/Actuator/Value",
  "Face/Led/Red/Right/180Deg/Actuator/Value",
  "Face/Led/Red/Right/225Deg/Actuator/Value",
  "Face/Led/Red/Right/270Deg/Actuator/Value",
  "Face/Led/Red/Right/315Deg/Actuator/Value",
  "Face/Led/Green/Right/0Deg/Actuator/Value",
  "Face/Led/Green/Right/45Deg/Actuator/Value",
  "Face/Led/Green/Right/90Deg/Actuator/Value",
  "Face/Led/Green/Right/135Deg/Actuator/Value",
  "Face/Led/Green/Right/180Deg/Actuator/Value",
  "Face/Led/Green/Right/225Deg/Actuator/Value",
  "Face/Led/Green/Right/270Deg/Actuator/Value",
  "Face/Led/Green/Right/315Deg/Actuator/Value",
  "Face/Led/Blue/Right/0Deg/Actuator/Value",
  "Face/Led/Blue/Right/45Deg/Actuator/Value",
  "Face/Led/Blue/Right/90Deg/Actuator/Value",
  "Face/Led/Blue/Right/135Deg/Actuator/Value",
  "Face/Led/Blue/Right/180Deg/Actuator/Value",
  "Face/Led/Blue/Right/225Deg/Actuator/Value",
  "Face/Led/Blue/Right/270Deg/Actuator/Value",
  "Face/Led/Blue/Right/315Deg/Actuator/Value",
  "Ears/Led/Left/36Deg/Actuator/Value",
  "Ears/Led/Left/72Deg/Actuator/Value",
  "Ears/Led/Left/108Deg/Actuator/Value",
  "Ears/Led/Left/144Deg/Actuator/Value",
  "Ears/Led/Left/180Deg/Actuator/Value",
  "Ears/Led/Left/216Deg/Actuator/Value",
  "Ears/Led/Left/252Deg/Actuator/Value",
  "Ears/Led/Left/288Deg/Actuator/Value",
  "Ears/Led/Left/324Deg/Actuator/Value",
  "Ears/Led/Left/0Deg/Actuator/Value",
  "Ears/Led/Right/0Deg/Actuator/Value",
  "Ears/Led/Right/36Deg/Actuator/Value",
  "Ears/Led/Right/72Deg/Actuator/Value",
  "Ears/Led/Right/108Deg/Actuator/Value",
  "Ears/Led/Right/144Deg/Actuator/Value",
  "Ears/Led/Right/180Deg/Actuator/Value",
  "Ears/Led/Right/216Deg/Actuator/Value",
  "Ears/Led/Right/252Deg/Actuator/Value",
  "Ears/Led/Right/288Deg/Actuator/Value",
  "Ears/Led/Right/324Deg/Actuator/Value",
  "ChestBoard/Led/Red/Actuator/Value",
  "ChestBoard/Led/Green/Actuator/Value",
  "ChestBoard/Led/Blue/Actuator/Value",
  "LFoot/Led/Red/Actuator/Value",
  "LFoot/Led/Green/Actuator/Value",
  "LFoot/Led/Blue/Actuator/Value",
  "RFoot/Led/Red/Actuator/Value",
  "RFoot/Led/Green/Actuator/Value",
  "RFoot/Led/Blue/Actuator/Value",

  "US/Actuator/Value"
};

static const char* teamInfoNames[] =
{
  "GameCtrl/teamNumber",
  "GameCtrl/teamColour",
  "GameCtrl/playerNumber"
};

static const float sitDownAngles[21] =
{
  0.f,
  0.f,

  0.89f,
  0.06f,
  0.26f,
  -0.62f,

  0.89f,
  -0.06f,
  -0.26f,
  0.62f,

  0.f,
  0.f,
  -0.87f,
  2.16f,
  -1.18f,
  0.f,

  0.f,
  -0.87f,
  2.16f,
  -1.18f,
  0.f
};

class BHuman : public AL::ALModule
{
private:
  static BHuman* theInstance; /**< The only instance of this class. */

  static const int allowedFrameDrops = 3; /**< Maximum number of frame drops allowed before Nao sits down. */

  int memoryHandle; /**< The file handle of the shared memory. */
  LBHData* data; /**< The shared memory. */
  sem_t* sem; /**< The semaphore used to notify bhuman about new data. */
  AL::DCMProxy* proxy;
  AL::ALMemoryProxy* memory;
  AL::ALValue positionRequest;
  AL::ALValue hardnessRequest;
  AL::ALValue usRequest;
  AL::ALValue ledRequest;
  float* sensorPtrs[lbhNumOfSensorIds]; /** Pointers to where NaoQi stores the current sensor values. */

  int dcmTime; /**< Current dcm time, updated at each onPreProcess call. */

  float requestedActuators[lbhNumOfActuatorIds]; /**< The previous actuator values requested. */

  int lastReadingActuators; /**< The previous actuators read. For detecting frames without seemingly new data from bhuman. */
  int actuatorDrops; /**< The number of frames without seemingly new data from bhuman. */
  int frameDrops; /**< The number frames without a reaction from bhuman. */

  enum State {sitting, standingUp, standing, sittingDown, preShuttingDown, shuttingDown} state;
  float phase; /**< How far is the Nao in its current standing up or sitting down motion [0 ... 1]? */
  int ledIndex; /**< The index of the last LED set. */

  int rightEarLEDsChangedTime; // Last time when the right ear LEDs were changed by the B-Human code

  float startAngles[lbhNumOfPositionActuatorIds]; /**< Start angles for standing up or sitting down. */
  float startHardness[lbhNumOfPositionActuatorIds]; /**< Start hardness for sitting down. */

  int startPressedTime; /**< The last time the chest button was not pressed. */
  unsigned lastBHumanStartTime; /**< The last time bhuman was started. */

  /** Close all resources acquired. Called when initialization failed or during destruction. */
  void close()
  {
    fprintf(stderr, "libbhuman: Stopping.\n");

    if(proxy)
    {
      proxy->getGenericProxy()->getModule()->removeAllPreProcess();
      proxy->getGenericProxy()->getModule()->removeAllPostProcess();
      delete proxy;
    }
    if(memory)
      delete memory;
    if(sem != SEM_FAILED)
      sem_close(sem);
    if(data != MAP_FAILED)
      munmap(data, sizeof(LBHData));

    fprintf(stderr, "libbhuman: Stopped.\n");
  }

  /**
   * Set the eye LEDs based on the current state.
   * Shutting down -> Lower segments are red.
   * bhuman crashed -> Whole eyes alternately flash red.
   * bhuman not running -> Lower segments flash blue.
   * @param actuators The actuator values a part of which will be set by this method.
   */
  void setEyeLeds(float* actuators)
  {
    for(int i = faceLedRedLeft0DegActuator; i <= faceLedBlueRight315DegActuator; ++i)
      actuators[i] = 0.f;

    if(state == shuttingDown)
    {
      actuators[faceLedRedLeft180DegActuator] = 1.f;
      actuators[faceLedRedRight180DegActuator] = 1.f;
    }
    else if(data->state != okState)
    {
      // set the "libbhuman is active and bhuman crashed" leds
      float blink = float(dcmTime / 500 & 1);
      for(int i = faceLedRedLeft0DegActuator; i <= faceLedRedLeft315DegActuator; ++i)
        actuators[i] = blink;
      for(int i = faceLedRedRight0DegActuator; i <= faceLedRedRight315DegActuator; ++i)
        actuators[i] = 1.f - blink;
    }
    else
    {
      // set the "libbhuman is active and bhuman is not running" LEDs
      float blink = float(dcmTime / 500 & 1);
      actuators[faceLedBlueLeft180DegActuator] = blink;
      actuators[faceLedBlueRight180DegActuator] = blink;
    }
  }

  /**
   * Shows the battery state in the right ear if the robot is in the standing state
   * and bhuman has not used one of these LEDs in the past 5 seconds.
   * @param actuators The actuator values a part of which will be set by this method.
   */
  void setBatteryLeds(float* actuators)
  {
    for(int i = earsLedRight0DegActuator; i <= earsLedRight324DegActuator; ++i)
      if(actuators[i] != requestedActuators[i])
      {
        rightEarLEDsChangedTime = dcmTime;
        requestedActuators[i] = actuators[i];
      }

    if(state != standing || dcmTime - rightEarLEDsChangedTime > 5000)
      for(int i = 0; i < int(*sensorPtrs[batteryChargeSensor] * 10.f) && i < 10; ++i)
        actuators[earsLedRight0DegActuator + i] = 1.f;
  }

  /**
   * Copies everything that's not for servos from one set of actuator values to another.
   * @param srcActuators The actuator values from which is copied.
   * @param destActuators The actuator values to which is copied.
   */
  void copyNonServos(const float* srcActuators, float* destActuators)
  {
    for(int i = faceLedRedLeft0DegActuator; i < lbhNumOfActuatorIds; ++i)
      destActuators[i] = srcActuators[i];
  }

  /** Resets ultrasound measurements so new ones can be detected. */
  void resetUsMeasurements()
  {
    for(int i = lUsSensor; i <= rUs9Sensor; ++i)
      *sensorPtrs[i] = 0.f;
  }

  /**
   * Handles the different states libbhuman can be in.
   * @param actuators The actuator values requested. They will not be changed, but might
   *                  be used as result of this method.
   * @return The actuator values that should be set. In the standing state, they are
   *         identical to the actuators passed to this method. In all other states,
   *         they are different.
   */
  float* handleState(float* actuators)
  {
    static float controlledActuators[lbhNumOfActuatorIds];

    switch(state)
    {
    sitting:
      state = sitting;

    case sitting:
      memset(controlledActuators, 0, sizeof(controlledActuators));
      if(frameDrops > allowedFrameDrops ||
         (actuators[lHipPitchHardnessActuator] == 0.f && actuators[rHipPitchHardnessActuator] == 0.f))
        return controlledActuators;

      for(int i = 0; i < lbhNumOfPositionActuatorIds - 2; ++i)
        startAngles[i] = *sensorPtrs[i * 3];

	  startAngles[21] = 0;
	  startAngles[22] = 0;

    standingUp:
      state = standingUp;
      phase = 0.f;

    case standingUp:
      if(phase < 1.f && frameDrops <= allowedFrameDrops)
      {
        memset(controlledActuators, 0, sizeof(controlledActuators));
        phase = std::min(phase + 0.005f, 1.f);
        for(int i = 0; i < lbhNumOfPositionActuatorIds; ++i)
        {
          controlledActuators[i] = actuators[i] * phase + startAngles[i] * (1.f - phase);
          float h = std::min(actuators[i + headYawHardnessActuator], 0.5f);
          controlledActuators[i + headYawHardnessActuator] = actuators[i + headYawHardnessActuator] * phase + h * (1.f - phase);
        }
        return controlledActuators;
      }
      state = standing;

    case standing:
      if(frameDrops <= allowedFrameDrops)
        return actuators; // use original actuators

    case preShuttingDown:
      for(int i = 0; i < lbhNumOfPositionActuatorIds; ++i)
      {
        startAngles[i] = positionRequest[5][i][0];
        startHardness[i] = std::min<float>(hardnessRequest[5][i][0], 0.3f);
      }
      state = state == preShuttingDown ? shuttingDown : sittingDown;
      phase = 0.f;

    case sittingDown:
    case shuttingDown:
      if((phase < 1.f && frameDrops > allowedFrameDrops) || state == shuttingDown)
      {
        memset(controlledActuators, 0, sizeof(controlledActuators));
        phase = std::min(phase + 0.005f, 1.f);
        for(int i = 0; i < lbhNumOfPositionActuatorIds - 2; ++i)
        {
          controlledActuators[i] = sitDownAngles[i] * phase + startAngles[i] * (1.f - phase);
          controlledActuators[i + headYawHardnessActuator] = startHardness[i];
        }
        return controlledActuators;
      }
      else if(frameDrops <= allowedFrameDrops)
      {
        for(int i = 0; i < lbhNumOfPositionActuatorIds; ++i)
          startAngles[i] = positionRequest[5][i][0];
        goto standingUp;
      }
      else
        goto sitting;
    }
  }

  /** The method sets all actuators. */
  void setActuators()
  {
    // set all actuator values according to the values in the shared memory block
    try
    {
      dcmTime = proxy->getTime(0);

      data->readingActuators = data->newestActuators;
      if(data->readingActuators == lastReadingActuators)
      {
        if(actuatorDrops == 0)
          fprintf(stderr, "libbhuman: missed actuator request.\n");
        ++actuatorDrops;
      }
      else
        actuatorDrops = 0;
      lastReadingActuators = data->readingActuators;
      float* readingActuators = data->actuators[data->readingActuators];
      float* actuators = handleState(readingActuators);

      if(state != standing)
      {
        if(frameDrops > 0 || state == shuttingDown)
          setEyeLeds(actuators);
        else
          copyNonServos(readingActuators, actuators);
      }
      setBatteryLeds(actuators);

      // set position actuators
      positionRequest[4][0] = dcmTime; // 0 delay!
      for(int i = 0; i < lbhNumOfPositionActuatorIds; ++i)
        positionRequest[5][i][0] = actuators[i];
      proxy->setAlias(positionRequest);

      // set hardness actuators
      bool requestedHardness = false;
      for(int i = headYawHardnessActuator; i < headYawHardnessActuator + lbhNumOfHardnessActuatorIds; ++i)
        if(actuators[i] != requestedActuators[i])
        {
          hardnessRequest[4][0] = dcmTime; // 0 delay!
          for(int j = 0; j < lbhNumOfHardnessActuatorIds; ++j)
            hardnessRequest[5][j][0] = requestedActuators[headYawHardnessActuator + j] = actuators[headYawHardnessActuator + j];
          proxy->setAlias(hardnessRequest);
          requestedHardness = true;
          break;
        }

      // set us actuator
      bool requestedUs = false;
      if(requestedActuators[usActuator] != actuators[usActuator])
      {
        requestedActuators[usActuator] = actuators[usActuator];
        if(actuators[usActuator] >= 0.f)
        {
          resetUsMeasurements();
          usRequest[4][0] = dcmTime;
          usRequest[5][0][0] = actuators[usActuator];
          proxy->setAlias(usRequest);
          requestedUs = true;
        }
      }

      // set led
      if(!requestedHardness && !requestedUs)
        for(int i = 0; i < lbhNumOfLedActuatorIds; ++i)
        {
          int index = faceLedRedLeft0DegActuator + ledIndex;
          if(++ledIndex == lbhNumOfLedActuatorIds)
            ledIndex = 0;
          if(actuators[index] != requestedActuators[index])
          {
            ledRequest[0] = std::string(actuatorNames[index]);
            ledRequest[2][0][0] = requestedActuators[index] = actuators[index];
            ledRequest[2][0][1] = dcmTime;
            proxy->set(ledRequest);
            break;
          }
        }

      // set team info
      // since this should very rarely, we don't use a proxy here
      if(data->bhumanStartTime != lastBHumanStartTime)
      {
        for(int i = 0; i < lbhNumOfTeamInfoIds; ++i)
          memory->insertData(teamInfoNames[i], data->teamInfo[i]);
        lastBHumanStartTime = data->bhumanStartTime;
      }
    }
    catch(AL::ALError& e)
    {
      fprintf(stderr, "libbhuman: %s\n", e.toString().c_str());
    }
  }

  /**
   * The method reads all sensors. It also detects if the chest button was pressed
   * for at least three seconds. In that case, it shuts down the robot.
   */
  void readSensors()
  {
    // get new sensor values and copy them to the shared memory block
    try
    {
      // copy sensor values into the shared memory block
      int writingSensors = 0;
      if(writingSensors == data->newestSensors)
        ++writingSensors;
      if(writingSensors == data->readingSensors)
        if(++writingSensors == data->newestSensors)
          ++writingSensors;
      assert(writingSensors != data->newestSensors);
      assert(writingSensors != data->readingSensors);

      float* sensors = data->sensors[writingSensors];
      for(int i = 0; i < lbhNumOfSensorIds; ++i)
        sensors[i] = *sensorPtrs[i];

      AL::ALValue value = memory->getData("GameCtrl/RoboCupGameControlData");
      if(value.isBinary() && value.getSize() == sizeof(RoboCup::RoboCupGameControlData))
        memcpy(&data->gameControlData[writingSensors], value, sizeof(RoboCup::RoboCupGameControlData));

      data->newestSensors = writingSensors;

      // detect shutdown request via chest-button
      if(*sensorPtrs[chestButtonSensor] == 0.f)
        startPressedTime = dcmTime;
      else if(state != shuttingDown && startPressedTime && dcmTime - startPressedTime > 3000)
      {
        if(*sensorPtrs[rBumperRightSensor] != 0.f || *sensorPtrs[rBumperLeftSensor] != 0.f ||
           *sensorPtrs[lBumperRightSensor] != 0.f || *sensorPtrs[lBumperLeftSensor] != 0.f)
          (void) !system("( /home/nao/bin/bhumand stop && sudo shutdown -r now ) &");
        else
          (void) !system("( /home/nao/bin/bhumand stop && sudo shutdown -h now ) &");
        state = preShuttingDown;
      }
    }
    catch(AL::ALError& e)
    {
      fprintf(stderr, "libbhuman: %s\n", e.toString().c_str());
    }

    // raise the semaphore
    if(sem != SEM_FAILED)
    {
      int sval;
      if(sem_getvalue(sem, &sval) == 0)
      {
        if(sval < 1)
        {
          sem_post(sem);
          frameDrops = 0;
        }
        else
        {
          if(frameDrops == 0)
            fprintf(stderr, "libbhuman: dropped sensor data.\n");
          ++frameDrops;
        }
      }
    }
  }

  /**
   * The method is called by NaoQi immediately before it communicates with the chest board.
   * It sets all the actuators.
   */
  static void onPreProcess()
  {
    theInstance->setActuators();
  }

  /**
   * The method is called by NaoQi immediately after it communicated with the chest board.
   * It reads all sensors.
   */
  static void onPostProcess()
  {
    theInstance->readSensors();
  }

public:
  /**
   * The constructor initializes the shared memory for communicating with bhuman.
   * It also establishes a communication with NaoQi and prepares all data structures
   * required for this communication.
   * @param pBroker A NaoQi broker that allows accessing other NaoQi modules.
   */
  BHuman(boost::shared_ptr<AL::ALBroker> pBroker) :
    ALModule(pBroker, "BHuman"),
    data((LBHData*) MAP_FAILED),
    sem(SEM_FAILED),
    proxy(0),
    memory(0),
    dcmTime(0),
    lastReadingActuators(-1),
    actuatorDrops(0),
    frameDrops(allowedFrameDrops + 1),
    state(sitting),
    phase(0.f),
    ledIndex(0),
    rightEarLEDsChangedTime(0),
    startPressedTime(0),
    lastBHumanStartTime(0)
  {
    setModuleDescription("A module that provides basic ipc NaoQi DCM access using shared memory.");
    fprintf(stderr, "libbhuman: Starting.\n");

    assert(lbhNumOfSensorIds == sizeof(sensorNames) / sizeof(*sensorNames));
    assert(lbhNumOfActuatorIds == sizeof(actuatorNames) / sizeof(*actuatorNames));
    assert(lbhNumOfTeamInfoIds == sizeof(teamInfoNames) / sizeof(*teamInfoNames));

    // create shared memory
    memoryHandle = shm_open(LBH_MEM_NAME, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
    if(memoryHandle == -1)
      perror("libbhuman: shm_open");
    else if(ftruncate(memoryHandle, sizeof(LBHData)) == -1)
      perror("libbhuman: ftruncate");
    else
    {
      // map the shared memory
      data = (LBHData*) mmap(NULL, sizeof(LBHData), PROT_READ | PROT_WRITE, MAP_SHARED, memoryHandle, 0);
      if(data == MAP_FAILED)
        perror("libbhuman: mmap");
      else
      {
        memset(data, 0, sizeof(LBHData));

        // open semaphore
        sem = sem_open(LBH_SEM_NAME, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR, 0);
        if(sem == SEM_FAILED)
          perror("libbhuman: sem_open");
        else
          try
          {
            // get the robot name
            memory = new AL::ALMemoryProxy(pBroker);

            std::string robotName = (std::string) memory->getData("Device/DeviceList/ChestBoard/BodyNickName", 0);
            strncpy(data->robotName, robotName.c_str(), sizeof(data->robotName));

            // create "positionRequest" and "hardnessRequest" alias
            proxy = new AL::DCMProxy(pBroker);

            AL::ALValue params;
            AL::ALValue result;
            params.arraySetSize(1);
            params.arraySetSize(2);

            params[0] = std::string("positionActuators");
            params[1].arraySetSize(lbhNumOfPositionActuatorIds);
            for(int i = 0; i < lbhNumOfPositionActuatorIds; ++i)
              params[1][i] = std::string(actuatorNames[i]);
            result = proxy->createAlias(params);

            params[0] = std::string("hardnessActuators");
            params[1].arraySetSize(lbhNumOfHardnessActuatorIds);
            for(int i = 0; i < lbhNumOfHardnessActuatorIds; ++i)
              params[1][i] = std::string(actuatorNames[headYawHardnessActuator + i]);
            result = proxy->createAlias(params);

            params[0] = std::string("usRequest");
            params[1].arraySetSize(1);
            params[1][0] = std::string(actuatorNames[usActuator]);
            result = proxy->createAlias(params);

            // prepare positionRequest
            positionRequest.arraySetSize(6);
            positionRequest[0] = std::string("positionActuators");
            positionRequest[1] = std::string("ClearAll");
            positionRequest[2] = std::string("time-separate");
            positionRequest[3] = 0;
            positionRequest[4].arraySetSize(1);
            positionRequest[5].arraySetSize(lbhNumOfPositionActuatorIds);
            for(int i = 0; i < lbhNumOfPositionActuatorIds; ++i)
              positionRequest[5][i].arraySetSize(1);

            // prepare hardnessRequest
            hardnessRequest.arraySetSize(6);
            hardnessRequest[0] = std::string("hardnessActuators");
            hardnessRequest[1] = std::string("ClearAll");
            hardnessRequest[2] = std::string("time-separate");
            hardnessRequest[3] = 0;
            hardnessRequest[4].arraySetSize(1);
            hardnessRequest[5].arraySetSize(lbhNumOfHardnessActuatorIds);
            for(int i = 0; i < lbhNumOfHardnessActuatorIds; ++i)
              hardnessRequest[5][i].arraySetSize(1);

            // prepare usRequest
            usRequest.arraySetSize(6);
            usRequest[0] = std::string("usRequest");
            usRequest[1] = std::string("Merge"); // doesn't work with "ClearAll"
            usRequest[2] = std::string("time-separate");
            usRequest[3] = 0;
            usRequest[4].arraySetSize(1);
            usRequest[5].arraySetSize(1);
            usRequest[5][0].arraySetSize(1);

            // prepare ledRequest
            ledRequest.arraySetSize(3);
            ledRequest[1] = std::string("ClearAll");
            ledRequest[2].arraySetSize(1);
            ledRequest[2][0].arraySetSize(2);
            ledRequest[2][0][1] = 0;

            // prepare sensor pointers
            for(int i = 0; i < lbhNumOfSensorIds; ++i)
              sensorPtrs[i] = (float*) memory->getDataPtr(sensorNames[i]);
            resetUsMeasurements();

            // initialize requested actuators
            memset(requestedActuators, 0, sizeof(requestedActuators));
            for(int i = faceLedRedLeft0DegActuator; i < chestBoardLedRedActuator; ++i)
              requestedActuators[i] = -1.f;

            // register "onPreProcess" and "onPostProcess" callbacks
            theInstance = this;
            proxy->getGenericProxy()->getModule()->atPreProcess(&onPreProcess);
            proxy->getGenericProxy()->getModule()->atPostProcess(&onPostProcess);

            fprintf(stderr, "libbhuman: Started!\n");
            return; // success
          }
          catch(AL::ALError& e)
          {
            fprintf(stderr, "libbhuman: %s\n", e.toString().c_str());
          }
      }
    }
    close(); // error
  }

  /** Close all resources acquired. */
  ~BHuman()
  {
    close();
  }
};

BHuman* BHuman::theInstance = 0;

/**
 * This method is called by NaoQi when loading this library.
 * Creates an instance of class BHuman.
 * @param pBroker A NaoQi broker that allows accessing other NaoQi modules.
 */
extern "C" int _createModule(boost::shared_ptr<AL::ALBroker> pBroker)
{
  AL::ALModule::createModule<BHuman>(pBroker);
  return 0;
}
