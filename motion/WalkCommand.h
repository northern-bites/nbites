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
#include "MotionConstants.h"
#include "MotionCommand.h"
using namespace Kinematics;
using namespace MotionConstants;

class WalkCommand : public MotionCommand
{
 public: // Constants
  static const int DEFAULT_SAMPLES_PER_STEP = 200;
 public:
  WalkCommand(const int _numSamples,
	      WalkParameters _params = WalkParameters::DEFAULT_PARAMS)
	  : MotionCommand(MotionConstants::WALK),
		numSamplesPerStep(_numSamples),
		params(_params) { setChainList(); }
  WalkCommand(const WalkCommand &other)
	  : MotionCommand(MotionConstants::WALK),
		numSamplesPerStep(other.numSamplesPerStep),
		params(other.params) { setChainList(); }
  virtual ~WalkCommand() {}
#ifdef NAOQI1
    virtual const float execute(ALPtr<ALMotionProxy> proxy) const {}
#else
    virtual const float execute(ALMotionProxy *proxy) const {}
#endif

  const vector<float> odometry() const {
    const float temp[3] = {lastX,lastY,lastH};
    return vector<float>(temp,&temp[3]);
  };


 protected:
  const int numSamplesPerStep;
  const WalkParameters params;
  mutable float lastX, lastY, lastH;

private:
	virtual void setChainList() { chainList.assign(WALK_CHAINS,
												   WALK_CHAINS +
												   WALK_NUM_CHAINS); }

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
#ifdef NAOQI1
  virtual const float execute(ALPtr<ALMotionProxy> proxy) const {
#else
  virtual const float execute(ALMotionProxy *proxy) const {
#endif
    proxy->walkStraight(distance*CM_TO_M, numSamplesPerStep);
    lastX = params.getMaxStepLength()*M_TO_CM / numSamplesPerStep;
    lastY = 0.0f;
    lastH = 0.0f;
    return 0.0f; //Hack to fix warning
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
#ifdef NAOQI1
  virtual const float execute(ALPtr<ALMotionProxy> proxy) const {
#else
  virtual const float execute(ALMotionProxy *proxy) const {
#endif
    proxy->walkSideways(distance*CM_TO_M, numSamplesPerStep);
    lastX = 0.0f;
    // It takes us two steps to complete a full step of size MaxStepSize
    lastY = params.getMaxStepSide()*M_TO_CM / (2.0f * numSamplesPerStep);
    lastH = 0.0f;
    return 0.0f; //Hack to fix warning
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
#ifdef NAOQI1
  virtual const float execute(ALPtr<ALMotionProxy> proxy) const {
#else
  virtual const float execute(ALMotionProxy *proxy) const {
#endif
    proxy->turn(angle, numSamplesPerStep);
    lastX = 0.0f;
    lastY = 0.0f;
    // It takes us two steps to complete a full step of size MaxStepSize
    lastH = (params.getMaxStepTurn()*TO_DEG) / (2.0f * numSamplesPerStep);
    return 0.0f; //Hack to fix warning
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
#ifdef NAOQI1
  virtual const float execute(ALPtr<ALMotionProxy> proxy) const {
#else
  virtual const float execute(ALMotionProxy *proxy) const {
#endif
    proxy->walkArc(angle, radius*CM_TO_M, numSamplesPerStep);
    const float arcLength = radius * angle;
    const int numSteps = ceil(arcLength / (params.getMaxStepLength()*M_TO_CM));
    const int numTotalSamples = numSamplesPerStep * numSteps;
    lastX = (radius - cos(angle) * radius) / (numTotalSamples);
    lastY = (sin(angle) * radius) / (numTotalSamples);
    // It takes us two steps to complete a full step of size MaxStepSize
    lastH = (angle / numTotalSamples)*TO_DEG;
    return 0.0f; //Hack to fix warning
  }

 private:
  const float angle;
  const float radius;
};

#endif
