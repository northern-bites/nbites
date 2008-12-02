#ifndef _JointCommand_h_DEFINED
#define _JointCommand_h_DEFINED

/**
 * Class to store joint commands specifically for the head.
 * Pass in a vector with target angles and the amount of time the robot needs
 * to take to get there. If the vector pointer is NULL, then that means we
 * are queueing up idle time on the head.
 */

#include <vector>
#include <string>

#include "almotionproxy.h"         // for INTERPOLATION_TYPE

#include "WalkCommand.h"
#include "Kinematics.h"
using namespace Kinematics;

class HeadJointCommand {
 public:
  HeadJointCommand(const float time, const std::vector<float> *joints,
		   const Kinematics::InterpolationType _type);
  HeadJointCommand(const HeadJointCommand &other);
  ~HeadJointCommand();

  const float getDuration() const { return duration; }
  const std::vector<float>* const getJoints(void) const { return headJoints; }
  const Kinematics::InterpolationType getType() const { return type; }

 private:
  const float duration;
  //const skew_function skew_func;
  const std::vector<float> *headJoints;
  const Kinematics::InterpolationType type;
};

#endif
