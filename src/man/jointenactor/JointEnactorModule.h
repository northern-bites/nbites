/**
 * @brief The joint enactor module communicates synchronously with the
 *        DCM to control the angles and stiffnesses of the joint
 *        actuators.
 * @author Ellis Ratner <eratner@bowdoin.edu>
 * @date   January 2013
 */
#pragma once

#include <string>
#include <iostream>

#include "RoboGrams.h"
#include "SharedData.h"

#include "PMotion.pb.h"
#include "LedCommand.pb.h"

namespace man
{
namespace jointenactor
{

/**
 * @class JointEnactorModule
 */
class JointEnactorModule : public portals::Module
{
public:
    JointEnactorModule();
    virtual ~JointEnactorModule();

    // Allows clients to set the stiffnesses of all joints.
    portals::InPortal<messages::JointAngles> stiffnessInput_;

    // Accepts motion commands (i.e. joint angles.)
    portals::InPortal<messages::JointAngles> jointsInput_;

    portals::InPortal<messages::LedCommand> ledsInput_;

private:
    void run_();
    void writeCommand();

    int shared_fd;
    SharedData* shared;

    uint64_t commandIndex;
    uint64_t lastRead;

    messages::JointAngles latestJointAngles_;
    messages::JointAngles latestStiffness_;
    messages::LedCommand latestLeds_;
};

} // namespace jointenactor
} // namespace man
