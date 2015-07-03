/**
 * @brief The Joint Enactor Module recieves commands from
 *        behaviors (LED commands) and motion (Joint Angles)
 *        and then passes them along to the Boss process using
 *        a segment of shared memory (Defined in SharedData.h)
 *
 * @author Daniel Zeller (Tore apart Ellis Ratners original Module)
 * @date   June 2015
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
    volatile SharedData* shared;

    uint8_t cmndStaging[COMMAND_SIZE];

    messages::JointAngles latestJointAngles_;
    messages::JointAngles latestStiffness_;
    messages::LedCommand latestLeds_;
};

} // namespace jointenactor
} // namespace man
