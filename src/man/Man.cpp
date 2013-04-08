#include "Man.h"
#include "Common.h"
#include <iostream>
#include "RobotConfig.h"

SET_POOL_SIZE(messages::WorldModel,  15);
SET_POOL_SIZE(messages::JointAngles, 15);

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
    motion.requestInput_.wireTo(&behaviors.motionRequestOut, true);

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
    behaviors.localizationIn.wireTo(&localization.output);
    behaviors.filteredBallIn.wireTo(&ballTrack.ballLocationOutput);
    behaviors.gameStateIn.wireTo(&gamestate.gameStateOutput);
    behaviors.visionFieldIn.wireTo(&vision.vision_field);
    behaviors.visionRobotIn.wireTo(&vision.vision_robot);
    behaviors.visionObstacleIn.wireTo(&vision.vision_obstacle);
    behaviors.motionStatusIn.wireTo(&motion.motionStatusOutput_, true);
    behaviors.odometryIn.wireTo(&motion.odometryOutput_, true);
    behaviors.sonarStateIn.wireTo(&sensors.sonarsOutput_, true);
    behaviors.footBumperStateIn.wireTo(&sensors.footbumperOutput_, true);
    behaviors.jointsIn.wireTo(&sensors.jointsOutput_, true);
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
