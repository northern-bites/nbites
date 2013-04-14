#pragma once
/**
 * Class to store joint commands specifically for the head.
 * Pass in a vector with target angles and the amount of time the robot needs
 * to take to get there. If the vector pointer is NULL, then that means we
 * are queueing up idle time on the head.
 */

#include <vector>
#include <string>
#include <boost/shared_ptr.hpp>

#include "Kinematics.h"
#include "JointCommand.h"
#include "MotionConstants.h"

namespace man
{
namespace motion
{

class HeadJointCommand : public JointCommand
{
public:
    typedef boost::shared_ptr<HeadJointCommand> ptr;

    HeadJointCommand(const float time,
                     const std::vector<float>& joints,
                     const std::vector<float>& head_stiffness,
                     const Kinematics::InterpolationType _type);
    virtual ~HeadJointCommand();

    virtual const std::vector<float>&
    getJoints(Kinematics::ChainID chain) const {
        if (chain == Kinematics::HEAD_CHAIN) {
            return headJoints;
        } else {
            return noJoints;
        }
    }
private:
    HeadJointCommand(const HeadJointCommand &other);
    const std::vector<float> headJoints;
    virtual void setChainList();
    const std::vector<float> noJoints;
};

} // namespace motion
} // namespace man
