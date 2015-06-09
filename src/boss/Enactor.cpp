#include "Enactor.h"
#include <iostream>

#include "JointNames.h"
#include "SensorTypes.h"
#include <alerror/alerror.h>

namespace boss {
namespace enactor {

Enactor::Enactor(boost::shared_ptr<AL::DCMProxy> dcm_) :
    dcm(dcm_)
{
    initEnactor();
}

Enactor::~Enactor()
{
    // TODO
}

void Enactor::command(messages::JointAngles angles, messages::JointAngles stiffness)
{
    std::vector<float> jointAngles = Kinematics::toJointAngles(angles);
    std::vector<float> jointStiffnesses = Kinematics::toJointAngles(stiffness);

    jointAngles.erase(jointAngles.begin() + Kinematics::R_HIP_YAW_PITCH);
    jointStiffnesses.erase(jointStiffnesses.begin() + Kinematics::R_HIP_YAW_PITCH);

    for (unsigned int i = 0; i < jointAngles.size(); ++i)
    {
        jointCommand[5][i][0] = jointAngles[i];
        stiffnessCommand[5][i][0] = jointStiffnesses[i];
    }

    try
    {
        jointCommand[4][0] = dcm->getTime(0);
        dcm->setAlias(jointCommand);
    }
    catch (AL::ALError e) {
        std::cout << "Couldn't set joint angles becasue: " << e.toString() << std::endl;
    }
    try
    {
        stiffnessCommand[4][0] = dcm->getTime(0);
        dcm->setAlias(stiffnessCommand);
    }
    catch (AL::ALError e) {
        std::cout << "Couldn't set stiffness becasue: " << e.toString() << std::endl;
    }
}

void Enactor::noStiff()
{
    for (unsigned int i = 0; i < 25; ++i)
    {
        stiffnessCommand[5][i][0] = -1.0;
    }

    try
    {
        stiffnessCommand[4][0] = dcm->getTime(0);
        dcm->setAlias(stiffnessCommand);
    }
    catch (AL::ALError e) {
        std::cout << "Couldn't kill stiffness becasue: " << e.toString() << std::endl;
    }

}

// Based on (stolen from) the original JointEnactorModule by Ellis Ratner
void Enactor::initEnactor()
{
    AL::ALValue aliases;

    // Create aliases for issuing joint angle
    // commands to the joint actuators.
    aliases.arraySetSize(2);
    aliases[0] = std::string("jointActuator");
    aliases[1].arraySetSize(25);

    aliases[1][sensors::HeadYaw]         = std::string("Device/SubDeviceList/HeadYaw/Position/Actuator/Value");
    aliases[1][sensors::HeadPitch]       = std::string("Device/SubDeviceList/HeadPitch/Position/Actuator/Value");
    aliases[1][sensors::LShoulderPitch]  = std::string("Device/SubDeviceList/LShoulderPitch/Position/Actuator/Value");
    aliases[1][sensors::LShoulderRoll]   = std::string("Device/SubDeviceList/LShoulderRoll/Position/Actuator/Value");
    aliases[1][sensors::LElbowYaw]       = std::string("Device/SubDeviceList/LElbowYaw/Position/Actuator/Value");
    aliases[1][sensors::LElbowRoll]      = std::string("Device/SubDeviceList/LElbowRoll/Position/Actuator/Value");
    aliases[1][sensors::LHipYawPitch]    = std::string("Device/SubDeviceList/LHipYawPitch/Position/Actuator/Value");
    aliases[1][sensors::LHipRoll]        = std::string("Device/SubDeviceList/LHipRoll/Position/Actuator/Value");
    aliases[1][sensors::LHipPitch]       = std::string("Device/SubDeviceList/LHipPitch/Position/Actuator/Value");
    aliases[1][sensors::LKneePitch]      = std::string("Device/SubDeviceList/LKneePitch/Position/Actuator/Value");
    aliases[1][sensors::LAnklePitch]     = std::string("Device/SubDeviceList/LAnklePitch/Position/Actuator/Value");
    aliases[1][sensors::LAnkleRoll]      = std::string("Device/SubDeviceList/LAnkleRoll/Position/Actuator/Value");
    aliases[1][sensors::RHipRoll]        = std::string("Device/SubDeviceList/RHipRoll/Position/Actuator/Value");
    aliases[1][sensors::RHipPitch]       = std::string("Device/SubDeviceList/RHipPitch/Position/Actuator/Value");
    aliases[1][sensors::RKneePitch]      = std::string("Device/SubDeviceList/RKneePitch/Position/Actuator/Value");
    aliases[1][sensors::RAnklePitch]     = std::string("Device/SubDeviceList/RAnklePitch/Position/Actuator/Value");
    aliases[1][sensors::RAnkleRoll]      = std::string("Device/SubDeviceList/RAnkleRoll/Position/Actuator/Value");
    aliases[1][sensors::RShoulderPitch]  = std::string("Device/SubDeviceList/RShoulderPitch/Position/Actuator/Value");
    aliases[1][sensors::RShoulderRoll]   = std::string("Device/SubDeviceList/RShoulderRoll/Position/Actuator/Value");
    aliases[1][sensors::RElbowYaw]       = std::string("Device/SubDeviceList/RElbowYaw/Position/Actuator/Value");
    aliases[1][sensors::RElbowRoll]      = std::string("Device/SubDeviceList/RElbowRoll/Position/Actuator/Value");
    aliases[1][sensors::LWristYaw]       = std::string("Device/SubDeviceList/LWristYaw/Position/Actuator/Value");
    aliases[1][sensors::LHand]           = std::string("Device/SubDeviceList/LHand/Position/Actuator/Value");
    aliases[1][sensors::RWristYaw]       = std::string("Device/SubDeviceList/RWristYaw/Position/Actuator/Value");
    aliases[1][sensors::RHand]           = std::string("Device/SubDeviceList/RHand/Position/Actuator/Value");

    try
    {
        dcm->createAlias(aliases);
    }
    catch(const AL::ALError& e)
    {
        std::cout << "Error creating joint actuator aliases: "
                  << e.toString()
                  << std::endl;
    }

    // Create aliases for issuing stiffness commands
    // to the joint actuators.
    aliases.clear();
    aliases.arraySetSize(2);
    aliases[0] = std::string("jointStiffness");
    aliases[1].arraySetSize(25);

    aliases[1][sensors::HeadYaw]          = std::string("Device/SubDeviceList/HeadYaw/Hardness/Actuator/Value");
    aliases[1][sensors::HeadPitch]        = std::string("Device/SubDeviceList/HeadPitch/Hardness/Actuator/Value");
    aliases[1][sensors::LShoulderPitch]  = std::string("Device/SubDeviceList/LShoulderPitch/Hardness/Actuator/Value");
    aliases[1][sensors::LShoulderRoll]   = std::string("Device/SubDeviceList/LShoulderRoll/Hardness/Actuator/Value");
    aliases[1][sensors::LElbowYaw]       = std::string("Device/SubDeviceList/LElbowYaw/Hardness/Actuator/Value");
    aliases[1][sensors::LElbowRoll]      = std::string("Device/SubDeviceList/LElbowRoll/Hardness/Actuator/Value");
    aliases[1][sensors::LHipYawPitch]   = std::string("Device/SubDeviceList/LHipYawPitch/Hardness/Actuator/Value");
    aliases[1][sensors::LHipRoll]        = std::string("Device/SubDeviceList/LHipRoll/Hardness/Actuator/Value");
    aliases[1][sensors::LHipPitch]       = std::string("Device/SubDeviceList/LHipPitch/Hardness/Actuator/Value");
    aliases[1][sensors::LKneePitch]      = std::string("Device/SubDeviceList/LKneePitch/Hardness/Actuator/Value");
    aliases[1][sensors::LAnklePitch]     = std::string("Device/SubDeviceList/LAnklePitch/Hardness/Actuator/Value");
    aliases[1][sensors::LAnkleRoll]      = std::string("Device/SubDeviceList/LAnkleRoll/Hardness/Actuator/Value");
    aliases[1][sensors::RHipRoll]        = std::string("Device/SubDeviceList/RHipRoll/Hardness/Actuator/Value");
    aliases[1][sensors::RHipPitch]       = std::string("Device/SubDeviceList/RHipPitch/Hardness/Actuator/Value");
    aliases[1][sensors::RKneePitch]      = std::string("Device/SubDeviceList/RKneePitch/Hardness/Actuator/Value");
    aliases[1][sensors::RAnklePitch]     = std::string("Device/SubDeviceList/RAnklePitch/Hardness/Actuator/Value");
    aliases[1][sensors::RAnkleRoll]      = std::string("Device/SubDeviceList/RAnkleRoll/Hardness/Actuator/Value");
    aliases[1][sensors::RShoulderPitch]  = std::string("Device/SubDeviceList/RShoulderPitch/Hardness/Actuator/Value");
    aliases[1][sensors::RShoulderRoll]   = std::string("Device/SubDeviceList/RShoulderRoll/Hardness/Actuator/Value");
    aliases[1][sensors::RElbowYaw]       = std::string("Device/SubDeviceList/RElbowYaw/Hardness/Actuator/Value");
    aliases[1][sensors::RElbowRoll]      = std::string("Device/SubDeviceList/RElbowRoll/Hardness/Actuator/Value");
    aliases[1][sensors::LWristYaw]       = std::string("Device/SubDeviceList/LWristYaw/Hardness/Actuator/Value");
    aliases[1][sensors::LHand]            = std::string("Device/SubDeviceList/LHand/Hardness/Actuator/Value");
    aliases[1][sensors::RWristYaw]       = std::string("Device/SubDeviceList/RWristYaw/Hardness/Actuator/Value");
    aliases[1][sensors::RHand]            = std::string("Device/SubDeviceList/RHand/Hardness/Actuator/Value");

    try
    {
        dcm->createAlias(aliases);
    }
    catch(const AL::ALError& e)
    {
        std::cout << "Error creating joint stiffness aliases: "
                  << e.toString()
                  << std::endl;
    }

    // Initialize joint command.
    jointCommand.arraySetSize(6);
    jointCommand[0] = std::string("jointActuator");
    jointCommand[1] = std::string("ClearAll");
    jointCommand[2] = std::string("time-separate");
    jointCommand[3] = 0; // Importance level.
    jointCommand[4].arraySetSize(1);
    jointCommand[5].arraySetSize(sensors::NUM_SENSOR_VALUES);
    // Set default joint angle values.
    for(int i = 0; i < sensors::NUM_SENSOR_VALUES; ++i)
    {
        jointCommand[5][i].arraySetSize(1);
        jointCommand[5][i][0] = 0.0f; // This will be the new joint angle.
    }

    // Initialize stiffness command.
    stiffnessCommand.arraySetSize(6);
    stiffnessCommand[0] = std::string("jointStiffness");
    stiffnessCommand[1] = std::string("ClearAll");
    stiffnessCommand[2] = std::string("time-separate");
    stiffnessCommand[3] = 0; // Importance level.
    stiffnessCommand[4].arraySetSize(1);
    stiffnessCommand[5].arraySetSize(sensors::NUM_SENSOR_VALUES);
    // Set default joint stiffness values.
    for(int i = 0; i < sensors::NUM_SENSOR_VALUES; ++i)
    {
        stiffnessCommand[5][i].arraySetSize(1);
        stiffnessCommand[5][i][0] = 0.0f; // This will be the new joint stiffness.
    }
}

}
}
