/*
 * @class MotionSwitchboardInterface
 *
 * An interface for the MotionSwitchboard
 *
 * Created mainly so that the MotionEnactor driving the switchboard does not
 * need to link against motion/ know about the motion implementation
 *
 * @author Octavian Neamtu
 */

#pragma once

#include <vector>

#include "ClassHelper.h"
#include "MotionConstants.h"

class MotionSwitchboardInterface {

protected:
    MotionSwitchboardInterface() {}

public:
    ADD_NULL_INSTANCE(MotionSwitchboardInterface);

    virtual ~MotionSwitchboardInterface() {}

    virtual const std::vector<float> getNextJoints() const {
        static const std::vector<float> default_joints(
                MotionConstants::SIT_DOWN_ANGLES,
                MotionConstants::SIT_DOWN_ANGLES + Kinematics::NUM_JOINTS);
        return default_joints;
    }
    virtual const std::vector<float> getNextStiffness() const {
        return MotionConstants::OFF_STIFFNESSES;
    }
    virtual void signalNextFrame() {}
};


