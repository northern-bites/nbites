/**
 * MotionModule
 * @todo
 *
 */
#pragma once

#include "RoboGrams.h"
#include "MotionConstants.h"

// Motion providers 
#include "bhwalk/BHWalkProvider.h"
//#include "ScriptedProvider.h"
#include "NullBodyProvider.h"

// Motion commands
#include "BodyJointCommand.h"
#include "WalkCommand.h"
#include "FreezeCommand.h"
#include "UnfreezeCommand.h"

// Messages
#include "JointAngles.pb.h"
#include "InertialState.pb.h"

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

	void start();

	void stop();

	void resetOdometry();

	const std::vector<float> getNextJoints() const;
	const std::vector<float> getNextStiffness() const;
	void signalNextFrame();
	//void sendMotionCommand(const HeadJointCommand::ptr command);
	void sendMotionCommand(const BodyJointCommand::ptr command);
	void sendMotionCommand(const WalkCommand::ptr command);
	//void sendMotionCommand(const Gait::ptr command);
	//void sendMotionCommand(const SetHeadCommand::ptr command);
	//void sendMotionCommand(const CoordHeadCommand::ptr command);
	void sendMotionCommand(const FreezeCommand::ptr command);
	void sendMotionCommand(const UnfreezeCommand::ptr command);
	//void sendMotionCommand(const StepCommand::ptr command);
	void sendMotionCommand(const DestinationCommand::ptr command);
	//void stopHeadMoves() { headProvider.requestStop(); }
	void stopBodyMoves() { curProvider->requestStop(); }

	bool isWalkActive() { return walkProvider.isWalkActive(); }
	bool isStanding()   { return walkProvider.isStanding(); }
	//bool isHeadActive() { return headProvider.isActive(); }
	bool isBodyActive() { return curProvider->isActive();}

	void resetWalkProvider(){ walkProvider.hardReset(); }
	//void resetScriptedProvider(){ scriptedProvider.hardReset(); }

	//MotionModel getOdometryUpdate() const { return walkProvider.getOdometryUpdate(); }

	int getFrameCount() const { return frameCount; }

	bool calibrated() { return walkProvider.calibrated(); }

	portals::InPortal<messages::JointAngles>   jointsInput_;
	portals::InPortal<messages::InertialState> inertialsInput_;
	portals::InPortal<messages::FSR>           fsrInput_; 

	portals::OutPortal<messages::JointAngles>  jointsOutput_;
	portals::OutPortal<messages::JointAngles>  stiffnessOutput_;

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

	static std::vector<float> getBodyJointsFromProvider(MotionProvider* provider);
	std::vector<BodyJointCommand::ptr> generateNextBodyProviderTransitions();

	/**
	 * @brief Sets the new joint and stiffness messages for 
	 *        JointEnactorModule to send to the DCM for 
	 *        execution. 
	 */
	void setJointsAndStiffness();

	BHWalkProvider walkProvider;
	//ScriptedProvider scriptedProvider;
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



