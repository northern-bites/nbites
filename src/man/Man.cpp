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
      vision(),
      ballTrack(),
      leds(broker),
      behaviors(MY_TEAM_NUMBER, MY_PLAYER_NUMBER),
      gamestate(MY_TEAM_NUMBER, MY_PLAYER_NUMBER)
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
    motion.commandInput_.wireTo(&behaviors.motionCommandOut , true);

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
    //cognitionThread.addModule(ballTrack);
    cognitionThread.addModule(leds);
    cognitionThread.addModule(behaviors);
    cognitionThread.addModule(gamestate);
    vision.topImageIn.wireTo(&imageTranscriber.topImageOut);
    vision.bottomImageIn.wireTo(&imageTranscriber.bottomImageOut);
    vision.joint_angles.wireTo(&sensors.jointsOutput_, true);
    vision.inertial_state.wireTo(&sensors.inertialsOutput_, true);
    leds.ledCommandsIn.wireTo(&behaviors.ledCommandOut);
    gamestate.commInput.wireTo(&comm._gameStateOutput, true);
    gamestate.buttonPressInput.wireTo(&guardian.advanceStateOutput, true);
    gamestate.initialStateInput.wireTo(&guardian.initialStateOutput, true);
    gamestate.switchTeamInput.wireTo(&guardian.switchTeamOutput, true);
    gamestate.switchKickOffInput.wireTo(&guardian.switchKickOffOutput, true);
    //behaviors.filteredBallIn.wireTo(&ballTrack.ballLocationOutput, true);
    behaviors.gameStateIn.wireTo(&gamestate.gameStateOutput);
    behaviors.visionBallIn.wireTo(&vision.vision_ball);
    behaviors.visionFieldIn.wireTo(&vision.vision_field);
    behaviors.visionRobotIn.wireTo(&vision.vision_robot);
    behaviors.motionStatusIn.wireTo(&motion.motionStatusOutput_, true);
    behaviors.sonarStateIn.wireTo(&sensors.sonarsOutput_, true);
    behaviors.footBumperStateIn.wireTo(&sensors.footbumperOutput_, true);
    for (int i = 0; i < NUM_PLAYERS_PER_TEAM; ++i)
    {
        behaviors.worldModelIn[i].wireTo(comm._worldModels[i], true);
    }


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
