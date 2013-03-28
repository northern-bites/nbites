#include "Man.h"
#include "Common.h"
#include <iostream>
#include "RobotConfig.h"

namespace man {

Man::Man(boost::shared_ptr<AL::ALBroker> broker, const std::string &name)
    : AL::ALModule(broker, name),
      sensorsThread("sensors", SENSORS_FRAME_LENGTH_uS),
      sensors(broker),
      jointEnactor(broker),
      motion(),
      guardianThread("guardian", GUARDIAN_FRAME_LENGTH_uS),
      guardian(),
      audio(broker),
      commThread("comm", COMM_FRAME_LENGTH_uS),
      comm(MY_TEAM_NUMBER, MY_PLAYER_NUMBER),
	  cognitionThread("cognition", COGNITION_FRAME_LENGTH_uS),
	  imageTranscriber(),
	  vision()
{
    setModuleDescription("The Northern Bites' soccer player.");

    /** Sensors **/
    sensorsThread.addModule(sensors);
    sensorsThread.addModule(jointEnactor);
    sensorsThread.addModule(motion);

    motion.jointsInput_.wireTo(&sensors.jointsOutput_);
    motion.inertialsInput_.wireTo(&sensors.inertialsOutput_);
    motion.fsrInput_.wireTo(&sensors.fsrOutput_);

    jointEnactor.jointsInput_.wireTo(&motion.jointsOutput_);
    jointEnactor.stiffnessInput_.wireTo(&motion.stiffnessOutput_);

    motion.start();

    /** Guardian **/
    guardianThread.addModule(guardian);
    guardianThread.addModule(audio);
    guardian.temperaturesInput.wireTo(&sensors.temperatureOutput_, true);
    guardian.chestButtonInput.wireTo(&sensors.chestboardButtonOutput_, true);
    guardian.footBumperInput.wireTo(&sensors.footbumperOutput_, true);
    guardian.inertialInput.wireTo(&sensors.inertialsOutput_, true);
    guardian.fsrInput.wireTo(&sensors.fsrOutput_, true);
    guardian.batteryInput.wireTo(&sensors.batteryOutput_, true);
    audio.audioIn.wireTo(&guardian.audioOutput);

    /** Comm **/
    commThread.addModule(comm);

	/** Cognition **/
	cognitionThread.addModule(imageTranscriber);
	cognitionThread.addModule(vision);
	vision.topImageIn.wireTo(&imageTranscriber.topImageOut);
	vision.bottomImageIn.wireTo(&imageTranscriber.bottomImageOut);
	vision.joint_angles.wireTo(&sensors.jointsOutput_, true);
	vision.inertial_state.wireTo(&sensors.inertialsOutput_, true);

    startSubThreads();

    // Test stand up.
    std::vector<float> angles(Kinematics::NUM_JOINTS);
    std::vector<float> stiffness(Kinematics::NUM_JOINTS);

    angles[0] = 0.0f;
    angles[1] = 0.0f;
    angles[2] = TO_RAD * 90.0f;
    angles[3] = TO_RAD * 10.0f;
    angles[4] = TO_RAD * -90.0f;
    angles[5] = TO_RAD * -10.0f;
    angles[6] = 0.0f;
    angles[7] = 0.0f;
    angles[8] = TO_RAD * -22.3f;
    angles[9] = TO_RAD * 43.5f;
    angles[10] = TO_RAD * -21.2f;
    angles[11] = 0;
    angles[12] = 0;
    angles[13] = 0;
    angles[14] = TO_RAD * -22.3f;
    angles[15] = TO_RAD * 43.5f;
    angles[16] = TO_RAD * -21.2f;
    angles[17] = 0;
    angles[18] = TO_RAD * 90.0f;
    angles[19] = TO_RAD * -10.0f;
    angles[20] = TO_RAD * 82.0f;
    angles[21] = TO_RAD * 13.2f;

    const float O = 0.85f;
    const float A = 0.2f;

    stiffness[0] = O;
    stiffness[1] = O;
    stiffness[2] = O;
    stiffness[3] = O;
    stiffness[4] = A;
    stiffness[5] = A;
    stiffness[6] = A;
    stiffness[7] = A;
    stiffness[8] = O;
    stiffness[9] = O;
    stiffness[10] = O;
    stiffness[11] = O;
    stiffness[12] = O;
    stiffness[13] = O;
    stiffness[14] = O;
    stiffness[15] = O;
    stiffness[16] = O;
    stiffness[17] = O;
    stiffness[18] = A;
    stiffness[19] = A;
    stiffness[20] = A;
    stiffness[21] = A;

    motion::BodyJointCommand::ptr standUpCommand(
	new motion::BodyJointCommand(
	    3.0, 
	    angles,
	    stiffness,
	    Kinematics::INTERPOLATION_SMOOTH
	    )
	);
    
    motion.sendMotionCommand(standUpCommand);
}

Man::~Man()
{
}

void Man::startSubThreads()
{
    startAndCheckThread(sensorsThread);
    startAndCheckThread(guardianThread);
    startAndCheckThread(commThread);
    startAndCheckThread(cognitionThread);
}

void Man::startAndCheckThread(DiagramThread& thread)
{
    if(thread.start())
    {
        std::cout << thread.getName() << "thread failed to start." <<
            std::endl;
    }
}

}
