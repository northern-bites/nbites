/**
 * Walk commands take parameters in the following units:
 *   - Centimeter for Distance, Degrees for rotation.
 * Walk commands are sent to ALMotionProxy in Meters and Radians.
 * Walk commands calculate odometry updates in Centimeters and Degrees.
 */
#ifndef _WalkCommand_h_DEFINED
#define _WalkCommand_h_DEFINED

#include <cmath>

// Aldebaran
#include "almotionproxy.h"   // For the motion proxy
using namespace AL;

// Ours
#include "WalkParameters.h"
#include "Kinematics.h"
using namespace Kinematics;

class WalkCommand
{
 public: // Constants
  static const int DEFAULT_SAMPLES_PER_STEP = 200;
 public:
  WalkCommand(const int _numSamples,
	      WalkParameters _params = WalkParameters::DEFAULT_PARAMS)
    : numSamplesPerStep(_numSamples), params(_params) { }
  WalkCommand(const WalkCommand &other)
    : numSamplesPerStep(other.numSamplesPerStep), params(other.params) { }
  virtual ~WalkCommand() {}

  virtual const float execute(ALMotionProxy *proxy) const = 0;

  const vector<float> odometry() const {
    const float temp[3] = {lastX,lastY,lastH};
    return vector<float>(temp,&temp[3]);
  };

 protected:
  const int numSamplesPerStep;
  const WalkParameters params;
  mutable float lastX, lastY, lastH;
};

class WalkStraight : public WalkCommand {
 public:
  WalkStraight(const float dist,
	       const int _numSamplesPerStep = DEFAULT_SAMPLES_PER_STEP,
	       const WalkParameters _params = WalkParameters::DEFAULT_PARAMS)
    : WalkCommand(_numSamplesPerStep, _params), distance(dist) { }
  WalkStraight(const WalkStraight &other)
    : WalkCommand(other), distance(other.distance) { }
  virtual ~WalkStraight() { }

  virtual const float execute(ALMotionProxy *proxy) const {
    proxy->walkStraight(distance*CM_TO_M, numSamplesPerStep);
    lastX = params.getMaxStepLength()*M_TO_CM / numSamplesPerStep;
    lastY = 0.0f;
    lastH = 0.0f;
  }

 private:
  const float distance;
};

class WalkSideways : public WalkCommand {
 public:
  WalkSideways(const float dist,
	       const int _numSamplesPerStep = DEFAULT_SAMPLES_PER_STEP,
	       const WalkParameters _params = WalkParameters::DEFAULT_PARAMS)
    : WalkCommand(_numSamplesPerStep, _params), distance(dist) { }
  WalkSideways(const WalkSideways &other)
    : WalkCommand(other), distance(other.distance) { }
  virtual ~WalkSideways() { }

  virtual const float execute(ALMotionProxy *proxy) const {
    proxy->walkSideways(distance*CM_TO_M, numSamplesPerStep);
    lastX = 0.0f;
    // It takes us two steps to complete a full step of size MaxStepSize
    lastY = params.getMaxStepSide()*M_TO_CM / (2.0f * numSamplesPerStep);
    lastH = 0.0f;
  }

 private:
  const float distance;
};

class WalkTurn : public WalkCommand {
 public:
  WalkTurn(const float _angle,
	   const int _numSamplesPerStep = DEFAULT_SAMPLES_PER_STEP,
	   const WalkParameters _params = WalkParameters::DEFAULT_PARAMS)
    : WalkCommand(_numSamplesPerStep, _params), angle(_angle) { }
  WalkTurn(const WalkTurn &other)
    : WalkCommand(other), angle(other.angle) { }
  virtual ~WalkTurn() { }

  virtual const float execute(ALMotionProxy *proxy) const {
    proxy->turn(angle, numSamplesPerStep);
    lastX = 0.0f;
    lastY = 0.0f;
    // It takes us two steps to complete a full step of size MaxStepSize
    lastH = (params.getMaxStepTurn()*TO_DEG) / (2.0f * numSamplesPerStep);
  }

 private:
  const float angle;
};

class WalkArc : public WalkCommand {
 public:
  WalkArc(const float _angle, const float _radius,
	  const int _numSamplesPerStep = DEFAULT_SAMPLES_PER_STEP,
	  const WalkParameters _params = WalkParameters::DEFAULT_PARAMS)
    : WalkCommand(_numSamplesPerStep, _params), angle(_angle), radius(_radius) { }
  WalkArc(const WalkArc &other)
    : WalkCommand(other), angle(other.angle), radius(other.radius) { }
  virtual ~WalkArc() { }

  virtual const float execute(ALMotionProxy *proxy) const {
    proxy->walkArc(angle, radius*CM_TO_M, numSamplesPerStep);
    const float arcLength = radius * angle;
    const int numSteps = ceil(arcLength / (params.getMaxStepLength()*M_TO_CM));
    const int numTotalSamples = numSamplesPerStep * numSteps;
    lastX = (radius - cos(angle) * radius) / (numTotalSamples);
    lastY = (sin(angle) * radius) / (numTotalSamples);
    // It takes us two steps to complete a full step of size MaxStepSize
    lastH = (angle / numTotalSamples)*TO_DEG;
  }

 private:
  const float angle;
  const float radius;
};

#endif
