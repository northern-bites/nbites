/**
 * @brief The motion module is responsible for mediating between
 *        the various motion providers, which are responsible for
 *        computing new joint angles and joint stiffnesses, and
 *        the joint enactor.
 *
 * @author Ellis Ratner <eratner@bowdoin.edu> (with many details
           taken from various parts of the old system.)
 * @date   March 2013
 */
#pragma once

#include "RoboGrams.h"
#include "MotionConstants.h"

// Motion providers
#include "bhwalk/BHWalkProvider.h"
#include "ScriptedProvider.h"
#include "NullBodyProvider.h"

// Motion commands
#include "BodyJointCommand.h"
#include "WalkCommand.h"
#include "FreezeCommand.h"
#include "UnfreezeCommand.h"

// Messages
#include "InertialState.pb.h"
#include "OdometryData.pb.h"
#include "PMotion.pb.h"

#include <vector>

namespace man
{
namespace motion
{
/**
 * @class MotionModule
 */
class MotionModule : public portals::Module
{
public:
    MotionModule();

    virtual ~MotionModule();

    /**
     * @brief Enables motion commands.
     */
    void start();

    /**
     * @brief Disables motion commands.
     */
    void stop();

    /**
     * @brief Reset the odometry.
     */
    void resetOdometry();

    const std::vector<float> getNextJoints() const;
    const std::vector<float> getNextStiffness() const;
    void signalNextFrame();
    //void sendMotionCommand(const HeadJointCommand::ptr command);

    // Body Joint commands (sweet moves)
    void sendMotionCommand(const BodyJointCommand::ptr command);
    void sendMotionCommand(const std::vector<BodyJointCommand::ptr> commands);
    void sendMotionCommand(messages::ScriptedMove script);

    // Walk Commands (set speeds)
    void sendMotionCommand(const WalkCommand::ptr command);
    void sendMotionCommand(messages::WalkCommand command);

    //void sendMotionCommand(const SetHeadCommand::ptr command);
    //void sendMotionCommand(const CoordHeadCommand::ptr command);
    void sendMotionCommand(const FreezeCommand::ptr command);
    void sendMotionCommand(const UnfreezeCommand::ptr command);
    void sendMotionCommand(const StepCommand::ptr command);

    // Odometry
    void sendMotionCommand(const DestinationCommand::ptr command);
    void sendMotionCommand(messages::DestinationWalk command);

    //void stopHeadMoves() { headProvider.requestStop(); }
    void stopBodyMoves() { curProvider->requestStop(); }

    bool isWalkActive() { return walkProvider.isWalkActive(); }
    bool isStanding()   { return walkProvider.isStanding(); }
    //bool isHeadActive() { return headProvider.isActive(); }
    bool isBodyActive() { return curProvider->isActive();}

    void resetWalkProvider(){ walkProvider.hardReset(); }
    void resetScriptedProvider(){ scriptedProvider.hardReset(); }

    int getFrameCount() const { return frameCount; }

    bool calibrated() { return walkProvider.calibrated(); }

    /**
     * @brief Generates a JointAngles message from a series
     *        of joint angles. Useful for debugging.
     */
    messages::JointAngles genJointCommand(float headYaw,
                          float headPitch,
                          float lShoulderPitch,
                          float lShoulderRoll,
                          float lElbowYaw,
                          float lElbowRoll,
                          float lWristYaw,
                          float lHand,
                          float rShoulderPitch,
                          float rShoulderRoll,
                          float rElbowYaw,
                          float rElbowRoll,
                          float rWristYaw,
                          float rHand,
                          float lHipYawPitch,
                          float rHipYawPitch,
                          float lHipRoll,
                          float lHipPitch,
                          float lKneePitch,
                          float lAnklePitch,
                          float lAnkleRoll,
                          float rHipRoll,
                          float rHipPitch,
                          float rKneePitch,
                          float rAnklePitch,
                          float rAnkleRoll);

    /**
     * @brief Mainly for debugging, allows a sequence of
     *        scripted commands specified in a particular
     *        format to be read from a file and converted
     *        to a vector of BodyJointCommands.
     *
     * @return a vector of BodyJointCommand boost shared
     *         pointers.
     */
    std::vector<BodyJointCommand::ptr> readScriptedSequence(
        const std::string& file);

    /* Input/Output related to executing motion commands. */
    portals::InPortal<messages::JointAngles>   jointsInput_;
    portals::InPortal<messages::InertialState> inertialsInput_;
    portals::InPortal<messages::FSR>           fsrInput_;
    portals::InPortal<messages::MotionCommand> commandInput_;

    portals::OutPortal<messages::JointAngles>  jointsOutput_;
    portals::OutPortal<messages::JointAngles>  stiffnessOutput_;

    /* Control interface with motion users. */
    portals::OutPortal<messages::OdometryData> odometryOutput_;

private:
    void preProcess();
    void processJoints();
    void processStiffness();
    bool postProcess();
    void preProcessHead();
    void preProcessBody();
    void processHeadJoints();
    void processBodyJoints();
    void clipHeadJoints(std::vector<float>& joints);
    void safetyCheckJoints();
    void swapBodyProvider();
    void swapHeadProvider();
    int realityCheckJoints();

    void processMotionInput();

    static std::vector<float> getBodyJointsFromProvider(MotionProvider* provider);
    std::vector<BodyJointCommand::ptr> generateNextBodyProviderTransitions();

    /**
     * @brief Sets the new joint and stiffness messages for
     *        JointEnactorModule to send to the DCM for
     *        execution.
     */
    void setJointsAndStiffness();

    /**
     * @brief Updates the odometry messages for interested
     *        modules to access easily.
     */
    void updateOdometry();

    BHWalkProvider walkProvider;
    ScriptedProvider scriptedProvider;
    //HeadProvider headProvider;
    //NullHeadProvider nullHeadProvider;
    NullBodyProvider nullBodyProvider;

    MotionProvider*         curProvider;
    MotionProvider*         nextProvider;

    MotionProvider*         curHeadProvider;
    MotionProvider*         nextHeadProvider;

    // Last information retrieved from the sensors.
    std::vector<float>      sensorAngles;
    std::vector<float>      sensorStiffnesses;
    messages::FSR           sensorFSRs;
    messages::InertialState sensorInertials;
    messages::MotionCommand motionCommand;

    std::vector<float>      nextJoints;
    std::vector<float>      nextStiffnesses;
    std::vector<float>      lastJoints;

    int frameCount;
    bool running;
    mutable bool newJoints; //Way to track if we ever use the same joints twice
    mutable bool newInputJoints;

    bool readyToSend;

    bool noWalkTransitionCommand;

    void run_();

    bool running_;
};
} // namespace motion
} // namespace man
