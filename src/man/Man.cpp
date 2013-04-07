#include "Man.h"
#include "Common.h"
#include <iostream>
#include "RobotConfig.h"

SET_POOL_SIZE(messages::WorldModel, 15);

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
      vision(),
      localization(),
      ballTrack(),
      gamestate(MY_TEAM_NUMBER, MY_PLAYER_NUMBER),
      behaviors(MY_TEAM_NUMBER, MY_PLAYER_NUMBER),
      leds(broker)
{
    setModuleDescription("The Northern Bites' soccer player.");

    /** Sensors **/
    sensorsThread.addModule(sensors);
#ifdef LOG_SENSORS
    sensorsThread.log<messages::JointAngles>(&sensors.jointsOutput_,
                                             "joints");
    sensorsThread.log<messages::JointAngles>(&sensors.temperatureOutput_,
                                             "temperatures");
    sensorsThread.log<messages::ButtonState>(&sensors.chestboardButtonOutput_,
                                             "chestbutton");
    sensorsThread.log<messages::FootBumperState>(&sensors.footbumperOutput_,
                                                 "footbumper");
    sensorsThread.log<messages::InertialState>(&sensors.inertialsOutput_,
                                               "inertials");
    sensorsThread.log<messages::SonarState>(&sensors.sonarsOutput_,
                                            "sonars");
    sensorsThread.log<messages::FSR>(&sensors.fsrOutput_,
                                     "fsrs");
    sensorsThread.log<messages::BatteryState>(&sensors.batteryOutput_,
                                              "battery");
#endif
    sensorsThread.addModule(jointEnactor);
    sensorsThread.addModule(motion);

    motion.jointsInput_.wireTo(&sensors.jointsOutput_);
    motion.inertialsInput_.wireTo(&sensors.inertialsOutput_);
    motion.fsrInput_.wireTo(&sensors.fsrOutput_);
    motion.stiffnessInput_.wireTo(&guardian.stiffnessControlOutput, true);
    motion.bodyCommandInput_.wireTo(&behaviors.bodyMotionCommandOut, true);
    motion.headCommandInput_.wireTo(&behaviors.headMotionCommandOut, true);

    jointEnactor.jointsInput_.wireTo(&motion.jointsOutput_);
    jointEnactor.stiffnessInput_.wireTo(&motion.stiffnessOutput_);

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
#ifdef LOG_GUARDIAN
    guardianThread.log<messages::StiffnessControl>(
        &guardian.stiffnessControlOutput,
        "stiffness");
    guardianThread.log<messages::FeetOnGround>(
        &guardian.feetOnGroundOutput,
        "feetground");
    guardianThread.log<messages::FallStatus>(
        &guardian.fallStatusOutput,
        "fall");
    guardianThread.log<messages::AudioCommand>(
        &guardian.audioOutput,
        "audio");
#endif

    /** Comm **/
    commThread.addModule(comm);
#ifdef LOG_COMM
    commThread.log<messages::GameState>(&comm._gameStateOutput, "gamestate");
#endif

    /** Cognition **/
    cognitionThread.addModule(imageTranscriber);
    cognitionThread.addModule(vision);
    cognitionThread.addModule(localization);
    cognitionThread.addModule(ballTrack);
    cognitionThread.addModule(gamestate);
    cognitionThread.addModule(behaviors);
    cognitionThread.addModule(leds);
    vision.topImageIn.wireTo(&imageTranscriber.topImageOut);
    vision.bottomImageIn.wireTo(&imageTranscriber.bottomImageOut);
    vision.joint_angles.wireTo(&sensors.jointsOutput_, true);
    vision.inertial_state.wireTo(&sensors.inertialsOutput_, true);
    localization.visionInput.wireTo(&vision.vision_field);
    localization.motionInput.wireTo(&motion.odometryOutput_, true);
    ballTrack.visionBallInput.wireTo(&vision.vision_ball);
    gamestate.commInput.wireTo(&comm._gameStateOutput, true);
    gamestate.buttonPressInput.wireTo(&guardian.advanceStateOutput, true);
    gamestate.initialStateInput.wireTo(&guardian.initialStateOutput, true);
    gamestate.switchTeamInput.wireTo(&guardian.switchTeamOutput, true);
    gamestate.switchKickOffInput.wireTo(&guardian.switchKickOffOutput, true);
    //behaviors.localizationInput.wireTo(&localization.output);
    behaviors.filteredBallIn.wireTo(&ballTrack.ballLocationOutput);
    behaviors.gameStateIn.wireTo(&gamestate.gameStateOutput);
    behaviors.visionFieldIn.wireTo(&vision.vision_field);
    behaviors.visionRobotIn.wireTo(&vision.vision_robot);
    behaviors.motionStatusIn.wireTo(&motion.motionStatusOutput_, true);
    behaviors.odometryIn.wireTo(&motion.odometryOutput_, true);
    behaviors.sonarStateIn.wireTo(&sensors.sonarsOutput_, true);
    behaviors.footBumperStateIn.wireTo(&sensors.footbumperOutput_, true);
    for (int i = 0; i < NUM_PLAYERS_PER_TEAM; ++i)
    {
        behaviors.worldModelIn[i].wireTo(comm._worldModels[i], true);
    }
    leds.ledCommandsIn.wireTo(&behaviors.ledCommandOut);

#ifdef LOG_VISION
    cognitionThread.log<messages::VisionField>(&vision.vision_field,
                                               "field");
    cognitionThread.log<messages::VisionBall>(&vision.vision_ball,
                                              "ball");
    cognitionThread.log<messages::VisionRobot>(&vision.vision_robot,
                                               "robot");
    cognitionThread.log<messages::VisionObstacle>(&vision.vision_obstacle,
                                                  "obstacle");
#endif





    // Test stand up.
    std::vector<float> angles(Kinematics::NUM_JOINTS, 0.0f);
    std::vector<float> stiffness(Kinematics::NUM_JOINTS, 0.0f);

    angles[0] = TO_RAD * 90.0f;   // L_SHOULDER_PITCH
    angles[1] = TO_RAD * 10.0f;   // L_SHOULDER_ROLL
    angles[2] = TO_RAD * -90.0f;  // L_ELBOW_YAW
    angles[3] = TO_RAD * -10.0f;  // L_ELBOW_ROLL
    angles[4] = 0.0f;             // L_HIP_YAW_PITCH
    angles[5] = 0.0f;             // L_HIP_ROLL
    angles[6] = TO_RAD * -22.3f;  // L_HIP_PITCH
    angles[7] = TO_RAD * 43.5f;   // L_KNEE_PITCH
    angles[8] = TO_RAD * -21.2f;  // L_ANKLE_PITCH
    angles[9] = 0.0f;             // L_ANKLE_ROLL
    angles[10] = 0.0f;            // R_HIP_YAW_PITCH
    angles[11] = 0.0f;            // R_HIP_ROLL
    angles[12] = TO_RAD * -22.3f; // R_HIP_PITCH
    angles[13] = TO_RAD * 43.5f;  // R_KNEE_PITCH
    angles[14] = TO_RAD * -21.2f; // R_ANKLE_PITCH
    angles[15] = 0.0f;            // R_ANKLE_ROLL
    angles[16] = TO_RAD * 90.2f;  // R_SHOULDER_PITCH
    angles[17] = TO_RAD * -10.0f; // R_SHOULDER_ROLL
    angles[18] = TO_RAD * 90.0f;  // R_ELBOW_YAW
    angles[19] = TO_RAD * 10.0f;  // R_ELBOW_ROLL

    // These values were taken from SweetMoves.py.
    const float O = 0.85f;
    const float A = 0.2f;

    stiffness[0] = O;             // HEAD_YAW
    stiffness[1] = O;             // HEAD_PITCH
    stiffness[2] = A;             // ...(same as above)
    stiffness[3] = A;
    stiffness[4] = A;
    stiffness[5] = A;
    stiffness[6] = O;
    stiffness[7] = O;
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

     // make the protobuff for testing
    messages::BodyJointCommand standUpProtoCommand;
    messages::JointAngles standJointAngles;
    messages::JointAngles standJointStiffness;

    standJointAngles.set_l_shoulder_pitch(angles[0]);
    standJointAngles.set_l_shoulder_roll(angles[1]);
    standJointAngles.set_l_elbow_yaw(angles[2]);
    standJointAngles.set_l_elbow_roll(angles[3]);
    standJointAngles.set_l_hip_yaw_pitch(angles[4]);
    standJointAngles.set_l_hip_roll(angles[5]);
    standJointAngles.set_l_hip_pitch(angles[6]);
    standJointAngles.set_l_knee_pitch(angles[7]);
    standJointAngles.set_l_ankle_pitch(angles[8]);
    standJointAngles.set_l_ankle_roll(angles[9]);
    standJointAngles.set_r_hip_yaw_pitch(angles[10]);
    standJointAngles.set_r_hip_roll(angles[11]);
    standJointAngles.set_r_hip_pitch(angles[12]);
    standJointAngles.set_r_knee_pitch(angles[13]);
    standJointAngles.set_r_ankle_pitch(angles[14]);
    standJointAngles.set_r_ankle_roll(angles[15]);
    standJointAngles.set_r_shoulder_pitch(angles[16]);
    standJointAngles.set_r_shoulder_roll(angles[17]);
    standJointAngles.set_r_elbow_yaw(angles[18]);
    standJointAngles.set_r_elbow_roll(angles[19]);


    standJointStiffness.set_head_yaw(stiffness[0]);
    standJointStiffness.set_head_pitch(stiffness[1]);
    standJointStiffness.set_l_shoulder_pitch(stiffness[2]);
    standJointStiffness.set_l_shoulder_roll(stiffness[3]);
    standJointStiffness.set_l_elbow_yaw(stiffness[4]);
    standJointStiffness.set_l_elbow_roll(stiffness[5]);
    standJointStiffness.set_l_hip_yaw_pitch(stiffness[6]);
    standJointStiffness.set_l_hip_roll(stiffness[7]);
    standJointStiffness.set_l_hip_pitch(stiffness[8]);
    standJointStiffness.set_l_knee_pitch(stiffness[9]);
    standJointStiffness.set_l_ankle_pitch(stiffness[10]);
    standJointStiffness.set_l_ankle_roll(stiffness[11]);
    standJointStiffness.set_r_hip_yaw_pitch(stiffness[12]);
    standJointStiffness.set_r_hip_roll(stiffness[13]);
    standJointStiffness.set_r_hip_pitch(stiffness[14]);
    standJointStiffness.set_r_knee_pitch(stiffness[15]);
    standJointStiffness.set_r_ankle_pitch(stiffness[16]);
    standJointStiffness.set_r_ankle_roll(stiffness[17]);
    standJointStiffness.set_r_shoulder_pitch(stiffness[18]);
    standJointStiffness.set_r_shoulder_roll(stiffness[19]);
    standJointStiffness.set_r_elbow_yaw(stiffness[20]);
    standJointStiffness.set_r_elbow_roll(stiffness[21]);

    standUpProtoCommand.set_time(3.f);
    standUpProtoCommand.mutable_angles()->CopyFrom(standJointAngles);
    standUpProtoCommand.mutable_stiffness()->CopyFrom(standJointStiffness);
    standUpProtoCommand.set_interpolation(messages::BodyJointCommand::SMOOTH);

    messages::ScriptedMove sweetMove;
    sweetMove.add_commands()->CopyFrom(standUpProtoCommand);


    angles[0] = 0.f;
    angles[16] = 0.f;
    stiffness[2] = O;
    stiffness[18] = O;

    // make the protobuff for testing
    messages::BodyJointCommand standArmProtoCommand;

    standJointAngles.set_l_shoulder_pitch(angles[0]);
    standJointAngles.set_r_shoulder_pitch(angles[16]);
    standJointStiffness.set_l_shoulder_pitch(stiffness[0]);
    standJointStiffness.set_r_shoulder_pitch(stiffness[16]);

    standArmProtoCommand.set_time(3.f);
    standArmProtoCommand.mutable_angles()->CopyFrom(standJointAngles);
    standArmProtoCommand.mutable_stiffness()->CopyFrom(standJointStiffness);
    standArmProtoCommand.set_interpolation(messages::BodyJointCommand::SMOOTH);

    sweetMove.add_commands()->CopyFrom(standArmProtoCommand);


    messages::WalkCommand testWalk;
    testWalk.set_x_percent(.5f);
    testWalk.set_y_percent(0.f);
    testWalk.set_h_percent(0.f);

    messages::DestinationWalk testDestWalk;
    testDestWalk.set_rel_x(50.f);
    testDestWalk.set_rel_y(0.f);
    testDestWalk.set_rel_h(0.f);
    testDestWalk.set_gain(.5f);

    motion.start();
//    motion.sendMotionCommand(testWalk);
    motion.sendMotionCommand(testDestWalk);
    motion.sendMotionCommand(sweetMove);

    startSubThreads();
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
