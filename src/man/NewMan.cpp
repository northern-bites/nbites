#include "NewMan.h"
#include <iostream>
#include "RobotConfig.h"

/* TESTING (Ellis) */
#include "newmotion/BodyJointCommand.h"

namespace man {

Man::Man(boost::shared_ptr<AL::ALBroker> broker, const std::string &name)
    : AL::ALModule(broker, name),
      sensorsThread("sensors"),
      sensors(broker),
      jointEnactor(broker),
      motion(),
      commThread("comm"),
      comm(MY_TEAM_NUMBER, MY_PLAYER_NUMBER)
{
    setModuleDescription("The Northern Bites' soccer player.");

    /** Sensors **/
    sensorsThread.addModule(sensors);
    sensorsThread.log<messages::JointAngles>(&sensors.jointsOutput_, "joints");
    sensorsThread.addModule(jointEnactor);
    sensorsThread.addModule(motion);

    motion.jointsInput_.wireTo(&sensors.jointsOutput_);
    motion.inertialsInput_.wireTo(&sensors.inertialsOutput_);
    motion.fsrInput_.wireTo(&sensors.fsrOutput_);

    jointEnactor.jointsInput_.wireTo(&motion.jointsOutput_);
    jointEnactor.stiffnessInput_.wireTo(&motion.stiffnessOutput_);

    jointEnactor.enableMotion();
    motion.start();

    /** Comm **/
    commThread.addModule(comm);

    startSubThreads();

    std::vector<float> bodyJoints(Kinematics::NUM_BODY_JOINTS);
    bodyJoints[0] = 90;
    bodyJoints[1] = 10;
    bodyJoints[2] = -90;
    bodyJoints[3] = -10;
    bodyJoints[4] = 0;
    bodyJoints[5] = 0;
    bodyJoints[6] = -22.3;
    bodyJoints[7] = 43.5;
    bodyJoints[8] = -21.2;
    bodyJoints[9] = 0;
    bodyJoints[10] = 0;
    bodyJoints[11] = 0;
    bodyJoints[12] = -22.3;
    bodyJoints[13] = 43.5;
    bodyJoints[14] = -21.2;
    bodyJoints[15] = 0;
    bodyJoints[16] = 90;
    bodyJoints[17] = -10;
    bodyJoints[18] = 82;
    bodyJoints[19] = 13.2;

    const float O = 0.85f;
    const float A = 0.2f;

    std::vector<float> bodyStiffness(Kinematics::NUM_BODY_JOINTS);
    bodyStiffness[0] = O;
    bodyStiffness[1] = O;
    bodyStiffness[2] = A;
    bodyStiffness[3] = A;
    bodyStiffness[4] = A;
    bodyStiffness[5] = A;
    bodyStiffness[6] = O;
    bodyStiffness[7] = O;
    bodyStiffness[8] = O;
    bodyStiffness[9] = O;
    bodyStiffness[10] = O;
    bodyStiffness[11] = O;
    bodyStiffness[12] = O;
    bodyStiffness[13] = O;
    bodyStiffness[14] = O;
    bodyStiffness[15] = O;
    bodyStiffness[16] = A;
    bodyStiffness[17] = A;
    bodyStiffness[18] = A;
    bodyStiffness[19] = A;

    motion::BodyJointCommand::ptr bjc
	= motion::BodyJointCommand::ptr(
	    new motion::BodyJointCommand(3.0f,
					 bodyJoints,
					 bodyStiffness,
					 Kinematics::INTERPOLATION_SMOOTH
		)
	    );

    motion.sendMotionCommand(bjc);
}

Man::~Man()
{
}

void Man::startSubThreads()
{
    sensorsThread.start();
    commThread.start();
}

}
