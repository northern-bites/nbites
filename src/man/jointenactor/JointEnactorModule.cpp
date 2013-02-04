#include "JointEnactorModule.h"

namespace man
{
    namespace jointenactor
    {
	JointEnactorModule::JointEnactorModule(boost::shared_ptr<AL::ALBroker> broker)
	    : portals::Module(), broker_(broker)
	{
	    start();
	}

	JointEnactorModule::~JointEnactorModule()
	{
	    stop();
	}

	void JointEnactorModule::setStiffness(float stiffness)
	{
	    std::cout << "JointEnactorModule : Setting stiffnessess to " << stiffness << std::endl;
	    AL::ALValue stiffnessCommand;
	    int DCMTime;
	    
	    try
	    {
		// Get the DCM time in 1 second. 
		DCMTime = dcmProxy_->getTime(1000);
	    }
	    catch(const AL::ALError& e)
	    {
		throw ALERROR("JointEnactorModule", "setStiffness", "Error getting DCM time: " + e.toString());
	    }

	    // Linearly merge stiffness values over 1 second from last
	    // stiffness value to the desired new stiffness value. 
	    stiffnessCommand.arraySetSize(3);
	    stiffnessCommand[0] = std::string("jointStiffness");
	    stiffnessCommand[1] = std::string("Merge");
	    stiffnessCommand[2].arraySetSize(1);
	    stiffnessCommand[2][0].arraySetSize(2);
	    stiffnessCommand[2][0][0] = stiffness;
	    stiffnessCommand[2][0][1] = DCMTime;
	    try
	    {
		dcmProxy_->set(stiffnessCommand);
	    }
	    catch (const AL::ALError &e)
	    {
		throw ALERROR("JointEnactorModule", "setStiffness()", "Error when sending stiffness command to DCM: " + e.toString());
	    }
	}

	void JointEnactorModule::start()
	{
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
	    // 	isDCMRunning = broker_->getProxy("ALLauncher")->call<bool>("isModulePresent", std::string("DCM"));
	    // }
	    // catch (AL::ALError& e)
	    // {
	    // 	throw ALERROR("JointEnactorModule", "start()", "Unable to connect to the DCM: " + e.toString());
	    // }

	    // if(!isDCMRunning)
	    // {
	    // 	throw ALERROR("JointEnactorModule", "start()", "There is no DCM running!");
	    // }

	    // If the DCM is running and we have a proxy to it,
	    // initialize aliases and then connect synchronously
	    // to the DCM loop. 
	    initialize();
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
	}

	void JointEnactorModule::connectToDCMLoop()
	{
	    try
	    {
		dcmPreProcessConnection_ = broker_->getProxy("DCM")->getModule()->atPreProcess(boost::bind(&JointEnactorModule::DCMPreProcessCallback, this));
	    }
	}

	void JointEnactorModule::DCMPreProcessCallback()
	{
	    // @todo
	}

	void JointEnactorModule::stop()
	{
	    // Kill the joint stiffnessess.
	    setStiffness(0.0f);
	    // Disconnect synchronized callback from DCM loop. 
	    dcmPreProcessConnection_.disconnect();
	}

	void JointEnactorModule::run_()
	{
	    setStiffness(0.0f);
	}

    } // namespace jointenactor
} // namespace man
