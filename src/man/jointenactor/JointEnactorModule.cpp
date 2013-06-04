#include "JointEnactorModule.h"
#include "Profiler.h"

namespace man {
namespace jointenactor{

JointEnactorModule::JointEnactorModule(boost::shared_ptr<AL::ALBroker> broker)
    : portals::Module(), broker_(broker), motionEnabled_(true)
{
    start();
}

JointEnactorModule::~JointEnactorModule()
{
    stop();
}

void JointEnactorModule::enableMotion()
{
    motionEnabled_ = true;
}

void JointEnactorModule::disableMotion()
{
    motionEnabled_ = false;
}

void JointEnactorModule::start()
{
    std::cout << "(JointEnactorModule) Starting the joint enactor module." << std::endl;

    signed long isDCMRunning;

    try
    {
        // Get the DCM proxy.
        dcmProxy_ = broker_->getDcmProxy();
    }
    catch (AL::ALError& e)
    {
        throw ALERROR("JointEnactorModule", "start()", "Failed to create DCM proxy: " + e.toString());
    }

    // Is the DCM running?
    // try
    // {
    //     isDCMRunning = broker_->getProxy("ALLauncher")->call<bool>("isModulePresent", std::string("DCM"));
    // }
    // catch (AL::ALError& e)
    // {
    //     throw ALERROR("JointEnactorModule", "start()", "Unable to connect to the DCM: " + e.toString());
    // }

    // if(!isDCMRunning)
    // {
    //     throw ALERROR("JointEnactorModule", "start()", "There is no DCM running!");
    // }

    // If the DCM is running and we have a proxy to it,
    // initialize aliases and then connect synchronously
    // to the DCM loop.
    initialize();
    connectToDCMLoop();
}

void JointEnactorModule::initialize()
{
    AL::ALValue aliases;
    // Create aliases for issuing joint angle
    // commands to the joint actuators.
    aliases.arraySetSize(2);
    aliases[0] = std::string("jointActuator");
    aliases[1].arraySetSize(26);

    aliases[1][sensors::HeadYaw]         = std::string("Device/SubDeviceList/HeadYaw/Position/Actuator/Value");
    aliases[1][sensors::HeadPitch]       = std::string("Device/SubDeviceList/HeadPitch/Position/Actuator/Value");
    aliases[1][sensors::LAnklePitch]    = std::string("Device/SubDeviceList/LAnklePitch/Position/Actuator/Value");
    aliases[1][sensors::LAnkleRoll]     = std::string("Device/SubDeviceList/LAnkleRoll/Position/Actuator/Value");
    aliases[1][sensors::LElbowRoll]     = std::string("Device/SubDeviceList/LElbowRoll/Position/Actuator/Value");
    aliases[1][sensors::LElbowYaw]      = std::string("Device/SubDeviceList/LElbowYaw/Position/Actuator/Value");
    aliases[1][sensors::LHand]           = std::string("Device/SubDeviceList/LHand/Position/Actuator/Value");
    aliases[1][sensors::LHipPitch]      = std::string("Device/SubDeviceList/LHipPitch/Position/Actuator/Value");
    aliases[1][sensors::LHipRoll]       = std::string("Device/SubDeviceList/LHipRoll/Position/Actuator/Value");
    aliases[1][sensors::LHipYawPitch]  = std::string("Device/SubDeviceList/LHipYawPitch/Position/Actuator/Value");
    aliases[1][sensors::LKneePitch]     = std::string("Device/SubDeviceList/LKneePitch/Position/Actuator/Value");
    aliases[1][sensors::LShoulderPitch] = std::string("Device/SubDeviceList/LShoulderPitch/Position/Actuator/Value");
    aliases[1][sensors::LShoulderRoll]  = std::string("Device/SubDeviceList/LShoulderRoll/Position/Actuator/Value");
    aliases[1][sensors::LWristYaw]      = std::string("Device/SubDeviceList/LWristYaw/Position/Actuator/Value");
    aliases[1][sensors::RAnklePitch]    = std::string("Device/SubDeviceList/RAnklePitch/Position/Actuator/Value");
    aliases[1][sensors::RAnkleRoll]     = std::string("Device/SubDeviceList/RAnkleRoll/Position/Actuator/Value");
    aliases[1][sensors::RElbowRoll]     = std::string("Device/SubDeviceList/RElbowRoll/Position/Actuator/Value");
    aliases[1][sensors::RElbowYaw]      = std::string("Device/SubDeviceList/RElbowYaw/Position/Actuator/Value");
    aliases[1][sensors::RHand]           = std::string("Device/SubDeviceList/RHand/Position/Actuator/Value");
    aliases[1][sensors::RHipPitch]      = std::string("Device/SubDeviceList/RHipPitch/Position/Actuator/Value");
    aliases[1][sensors::RHipRoll]       = std::string("Device/SubDeviceList/RHipRoll/Position/Actuator/Value");
    aliases[1][sensors::RHipYawPitch]    = std::string("Device/SubDeviceList/RHipYawPitch/Position/Actuator/Value");
    aliases[1][sensors::RKneePitch]     = std::string("Device/SubDeviceList/RKneePitch/Position/Actuator/Value");
    aliases[1][sensors::RShoulderPitch] = std::string("Device/SubDeviceList/RShoulderPitch/Position/Actuator/Value");
    aliases[1][sensors::RShoulderRoll]  = std::string("Device/SubDeviceList/RShoulderRoll/Position/Actuator/Value");
    aliases[1][sensors::RWristYaw]      = std::string("Device/SubDeviceList/RWristYaw/Position/Actuator/Value");

    try
    {
        dcmProxy_->createAlias(aliases);
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
    aliases[1].arraySetSize(26);

    aliases[1][sensors::HeadPitch]        = std::string("Device/SubDeviceList/HeadPitch/Hardness/Actuator/Value");
    aliases[1][sensors::HeadYaw]          = std::string("Device/SubDeviceList/HeadYaw/Hardness/Actuator/Value");
    aliases[1][sensors::LAnklePitch]     = std::string("Device/SubDeviceList/LAnklePitch/Hardness/Actuator/Value");
    aliases[1][sensors::LAnkleRoll]      = std::string("Device/SubDeviceList/LAnkleRoll/Hardness/Actuator/Value");
    aliases[1][sensors::LElbowRoll]      = std::string("Device/SubDeviceList/LElbowRoll/Hardness/Actuator/Value");
    aliases[1][sensors::LElbowYaw]       = std::string("Device/SubDeviceList/LElbowYaw/Hardness/Actuator/Value");
    aliases[1][sensors::LHand]            = std::string("Device/SubDeviceList/LHand/Hardness/Actuator/Value");
    aliases[1][sensors::LHipPitch]       = std::string("Device/SubDeviceList/LHipPitch/Hardness/Actuator/Value");
    aliases[1][sensors::LHipRoll]        = std::string("Device/SubDeviceList/LHipRoll/Hardness/Actuator/Value");
    aliases[1][sensors::LHipYawPitch]   = std::string("Device/SubDeviceList/LHipYawPitch/Hardness/Actuator/Value");
    aliases[1][sensors::LKneePitch]      = std::string("Device/SubDeviceList/LKneePitch/Hardness/Actuator/Value");
    aliases[1][sensors::LShoulderPitch]  = std::string("Device/SubDeviceList/LShoulderPitch/Hardness/Actuator/Value");
    aliases[1][sensors::LShoulderRoll]   = std::string("Device/SubDeviceList/LShoulderRoll/Hardness/Actuator/Value");
    aliases[1][sensors::LWristYaw]       = std::string("Device/SubDeviceList/LWristYaw/Hardness/Actuator/Value");
    aliases[1][sensors::RAnklePitch]     = std::string("Device/SubDeviceList/RAnklePitch/Hardness/Actuator/Value");
    aliases[1][sensors::RAnkleRoll]      = std::string("Device/SubDeviceList/RAnkleRoll/Hardness/Actuator/Value");
    aliases[1][sensors::RElbowRoll]      = std::string("Device/SubDeviceList/RElbowRoll/Hardness/Actuator/Value");
    aliases[1][sensors::RElbowYaw]       = std::string("Device/SubDeviceList/RElbowYaw/Hardness/Actuator/Value");
    aliases[1][sensors::RHand]            = std::string("Device/SubDeviceList/RHand/Hardness/Actuator/Value");
    aliases[1][sensors::RHipPitch]       = std::string("Device/SubDeviceList/RHipPitch/Hardness/Actuator/Value");
    aliases[1][sensors::RHipYawPitch]    = std::string("Device/SubDeviceList/RHipYawPitch/Hardness/Actuator/Value");
    aliases[1][sensors::RHipRoll]        = std::string("Device/SubDeviceList/RHipRoll/Hardness/Actuator/Value");
    aliases[1][sensors::RKneePitch]      = std::string("Device/SubDeviceList/RKneePitch/Hardness/Actuator/Value");
    aliases[1][sensors::RShoulderPitch]  = std::string("Device/SubDeviceList/RShoulderPitch/Hardness/Actuator/Value");
    aliases[1][sensors::RShoulderRoll]   = std::string("Device/SubDeviceList/RShoulderRoll/Hardness/Actuator/Value");
    aliases[1][sensors::RWristYaw]       = std::string("Device/SubDeviceList/RWristYaw/Hardness/Actuator/Value");

    try
    {
        dcmProxy_->createAlias(aliases);
    }
    catch(const AL::ALError& e)
    {
        std::cout << "Error creating joint stiffness aliases: "
                  << e.toString()
                  << std::endl;
    }

    // Initialize joint command.
    jointCommand_.arraySetSize(6);
    jointCommand_[0] = std::string("jointActuator");
    jointCommand_[1] = std::string("ClearAll");
    jointCommand_[2] = std::string("time-separate");
    jointCommand_[3] = 0; // Importance level.
    jointCommand_[4].arraySetSize(1);
    jointCommand_[5].arraySetSize(sensors::NUM_SENSOR_VALUES);
    // Set default joint angle values.
    for(int i = 0; i < sensors::NUM_SENSOR_VALUES; ++i)
    {
        jointCommand_[5][i].arraySetSize(1);
        jointCommand_[5][i][0] = 0.0f; // This will be the new joint angle.
    }

    // Initialize stiffness command.
    stiffnessCommand_.arraySetSize(6);
    stiffnessCommand_[0] = std::string("jointStiffness");
    stiffnessCommand_[1] = std::string("ClearAll");
    stiffnessCommand_[2] = std::string("time-separate");
    stiffnessCommand_[3] = 0; // Importance level.
    stiffnessCommand_[4].arraySetSize(1);
    stiffnessCommand_[5].arraySetSize(sensors::NUM_SENSOR_VALUES);
    // Set default joint stiffness values.
    for(int i = 0; i < sensors::NUM_SENSOR_VALUES; ++i)
    {
        stiffnessCommand_[5][i].arraySetSize(1);
        stiffnessCommand_[5][i][0] = 0.0f; // This will be the new joint stiffness.
    }
}

void JointEnactorModule::connectToDCMLoop()
{
    try
    {
        dcmPreProcessConnection_ = broker_->getProxy("DCM")->getModule()->atPreProcess(boost::bind(&JointEnactorModule::DCMPreProcessCallback, this));
    }
    catch(const AL::ALError& e)
    {
        std::cout << "Error connecting synchronously to the DCM loop: " + e.toString() << std::endl;
    }
}

void JointEnactorModule::DCMPreProcessCallback()
{
    PROF_ENTER(P_PRE_PROCESS);

    if(motionEnabled_)
    {
        // Send the next joint angles and joint stiffnesses
        // to the DCM for execution.

        std::vector<float> jointAngles = motion::toJointAngles(latestJointAngles_);
        std::vector<float> jointStiffness = motion::toJointAngles(latestStiffness_);

        for(unsigned int i = 0; i < Kinematics::NUM_JOINTS; ++i)
        {
            jointCommand_[5][i][0] = jointAngles[i];
            stiffnessCommand_[5][i][0] = jointStiffness[i];
        }

        // (1) Send next angles.
        PROF_ENTER(P_SEND_JOINTS);
        bool jointsToDCM = true;
        try
        {
            jointCommand_[4][0] = dcmProxy_->getTime(0);
            dcmProxy_->setAlias(jointCommand_);
        }
        catch(const AL::ALError& e)
        {
            std::cout << "Error setting joint angles: "
                      << e.toString() << std::endl;
            jointsToDCM = false;
        }
        PROF_EXIT(P_SEND_JOINTS);

        // (2) Send next stiffnesses.
        PROF_ENTER(P_SEND_HARDNESS);
        try
        {
            stiffnessCommand_[4][0] = dcmProxy_->getTime(0);
            dcmProxy_->setAlias(stiffnessCommand_);
        }
        catch(const AL::ALError& e)
        {
            std::cout << "Error setting hardness: "
                      << e.toString() << std::endl;
            jointsToDCM = false;
        }
        PROF_EXIT(P_SEND_HARDNESS);

        if(jointsToDCM)
            jointsInDCM();
    }

    PROF_EXIT(P_PRE_PROCESS);
}

void JointEnactorModule::newJoints()
{
    if(newJoints_)
        std::cout << "WARNING - DCM missed a frame in motion\n";
    else
        newJoints_ = true;
}

void JointEnactorModule::jointsInDCM()
{
    newJoints_ = false;
}

void JointEnactorModule::stop()
{
    // Disconnect synchronized callback from DCM loop.
    dcmPreProcessConnection_.disconnect();
}

void JointEnactorModule::run_()
{
    PROF_ENTER(P_JOINT_ENACTOR);
    // Update stiffnesses.
    stiffnessInput_.latch();
    latestStiffness_ = stiffnessInput_.message();

    // Update joint angles.
    jointsInput_.latch();
    latestJointAngles_ = jointsInput_.message();

    newJoints();
    PROF_EXIT(P_JOINT_ENACTOR);
}


} // namespace jointenactor
} // namespace man
