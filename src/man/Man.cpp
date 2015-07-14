#include "Man.h"

#include <iostream>

#include "Common.h"
#include "Profiler.h"
#include "Camera.h"

#include "log/logging.h"
#include "Log.h"
#include "control/control.h"

#ifndef OFFLINE
SET_POOL_SIZE(messages::WorldModel,  24);
SET_POOL_SIZE(messages::JointAngles, 24);
SET_POOL_SIZE(messages::InertialState, 16);
SET_POOL_SIZE(messages::YUVImage, 16);
SET_POOL_SIZE(messages::RobotLocation, 16);
SET_POOL_SIZE(messages::Toggle, 16);
#endif

namespace man {

Man::Man() :
    param("/home/nao/nbites/lib/parameters.json"),
    playerNum(param.getParam<int>("playerNumber")),
    teamNum(param.getParam<int>("teamNumber")),
    robotName(param.getParam<std::string>("robotName")),
    sensorsThread("sensors", SENSORS_FRAME_LENGTH_uS),
    sensors(),
    jointEnactor(),
    motion(),
    arms(),
    guardianThread("guardian", GUARDIAN_FRAME_LENGTH_uS),
    guardian(),
    audio(),
    commThread("comm", COMM_FRAME_LENGTH_uS),
    comm(teamNum, playerNum),
    cognitionThread("cognition", COGNITION_FRAME_LENGTH_uS),
    topTranscriber(*new image::ImageTranscriber(Camera::TOP)),
    bottomTranscriber(*new image::ImageTranscriber(Camera::BOTTOM)),
    vision(vision::DEFAULT_TOP_IMAGE_WIDTH,
           vision::DEFAULT_TOP_IMAGE_HEIGHT, robotName),
    localization(),
    ballTrack(),
    obstacle("/home/nao/nbites/Config/obstacleParams.txt", robotName),
    gamestate(teamNum, playerNum),
    behaviors(teamNum, playerNum),
    sharedBall(playerNum),
    naiveBall()
    {
        /** Sensors **/
        sensorsThread.addModule(sensors);
        sensorsThread.addModule(jointEnactor);
        sensorsThread.addModule(motion);
        sensorsThread.addModule(arms);

        sensors.printInput.wireTo(&guardian.printJointsOutput, true);

        motion.jointsInput_.wireTo(&sensors.jointsOutput_);
        motion.currentsInput_.wireTo(&sensors.currentsOutput_);
        motion.inertialsInput_.wireTo(&sensors.inertialsOutput_);
        motion.fsrInput_.wireTo(&sensors.fsrOutput_);
        motion.stiffnessInput_.wireTo(&guardian.stiffnessControlOutput, true);
        motion.bodyCommandInput_.wireTo(&behaviors.bodyMotionCommandOut, true);
        motion.headCommandInput_.wireTo(&behaviors.headMotionCommandOut, true);
        motion.requestInput_.wireTo(&behaviors.motionRequestOut, true);
        motion.fallInput_.wireTo(&guardian.fallStatusOutput, true);

        jointEnactor.jointsInput_.wireTo(&motion.jointsOutput_);
        jointEnactor.stiffnessInput_.wireTo(&motion.stiffnessOutput_);
        jointEnactor.ledsInput_.wireTo(&behaviors.ledCommandOut, true);

        arms.actualJointsIn.wireTo(&sensors.jointsOutput_);
        arms.expectedJointsIn.wireTo(&motion.jointsOutput_);
        arms.handSpeedsIn.wireTo(&motion.handSpeedsOutput_);

        /** Guardian **/
        guardianThread.addModule(guardian);
        guardianThread.addModule(audio);
        guardian.temperaturesInput.wireTo(&sensors.temperatureOutput_, true);
        guardian.chestButtonInput.wireTo(&sensors.chestboardButtonOutput_, true);
        guardian.footBumperInput.wireTo(&sensors.footbumperOutput_, true);
        guardian.inertialInput.wireTo(&sensors.inertialsOutput_, true);
        guardian.fsrInput.wireTo(&sensors.fsrOutput_, true);
        guardian.batteryInput.wireTo(&sensors.batteryOutput_, true);
        guardian.motionStatusIn.wireTo(&motion.motionStatusOutput_, true);
        audio.audioIn.wireTo(&guardian.audioOutput);

        /** Comm **/
        commThread.addModule(comm);
        comm._worldModelInput.wireTo(&behaviors.myWorldModelOut, true);
        comm._gcResponseInput.wireTo(&gamestate.gcResponseOutput, true);

        /** Cognition **/
        // Turn ON the finalize method for images, which we've specialized
        portals::Message<messages::YUVImage>::setFinalize(true);
        portals::Message<messages::ThresholdImage>::setFinalize(true);
        portals::Message<messages::PackedImage16>::setFinalize(true);
        portals::Message<messages::PackedImage8>::setFinalize(true);

        cognitionThread.addModule(topTranscriber);
        cognitionThread.addModule(bottomTranscriber);
        cognitionThread.addModule(vision);
        cognitionThread.addModule(localization);
        cognitionThread.addModule(ballTrack);
        cognitionThread.addModule(obstacle);
        cognitionThread.addModule(gamestate);
        cognitionThread.addModule(behaviors);
        cognitionThread.addModule(sharedBall);
        cognitionThread.addModule(naiveBall);

        topTranscriber.jointsIn.wireTo(&sensors.jointsOutput_, true);
        topTranscriber.inertsIn.wireTo(&sensors.inertialsOutput_, true);
        topTranscriber.naiveBallIn.wireTo(&naiveBall.naiveBallOutput, true);
        topTranscriber.filteredBallIn.wireTo(&ballTrack.ballLocationOutput, true);

        bottomTranscriber.jointsIn.wireTo(&sensors.jointsOutput_, true);
        bottomTranscriber.inertsIn.wireTo(&sensors.inertialsOutput_, true);
        bottomTranscriber.naiveBallIn.wireTo(&naiveBall.naiveBallOutput, true);
        bottomTranscriber.filteredBallIn.wireTo(&ballTrack.ballLocationOutput, true);

        vision.topIn.wireTo(&topTranscriber.imageOut);
        vision.bottomIn.wireTo(&bottomTranscriber.imageOut);
        vision.jointsIn.wireTo(&topTranscriber.jointsOut, true);
        vision.inertsIn.wireTo(&topTranscriber.inertsOut, true);

        localization.motionInput.wireTo(&motion.odometryOutput_, true);
        localization.resetInput[0].wireTo(&behaviors.resetLocOut, true);
        localization.resetInput[1].wireTo(&sharedBall.sharedBallReset, true);
        localization.gameStateInput.wireTo(&gamestate.gameStateOutput);
        localization.visionInput.wireTo(&vision.visionOut);
        localization.ballInput.wireTo(&ballTrack.ballLocationOutput);

        ballTrack.visionInput.wireTo(&vision.visionOut);
        ballTrack.odometryInput.wireTo(&motion.odometryOutput_, true);
        ballTrack.localizationInput.wireTo(&localization.output, true);

        for (int i = 0; i < NUM_PLAYERS_PER_TEAM; ++i)
        {
            sharedBall.worldModelIn[i].wireTo(comm._worldModels[i], true);
        }
        sharedBall.locIn.wireTo(&localization.output);
        sharedBall.ballIn.wireTo(&ballTrack.ballLocationOutput);
        naiveBall.ballIn.wireTo(&ballTrack.ballLocationOutput);

        obstacle.armContactIn.wireTo(&arms.contactOut, true);
        obstacle.visionIn.wireTo(&vision.robotObstacleOut, true);
        obstacle.sonarIn.wireTo(&sensors.sonarsOutput_, true);

        gamestate.commInput.wireTo(&comm._gameStateOutput, true);
        gamestate.buttonPressInput.wireTo(&guardian.advanceStateOutput, true);
        gamestate.initialStateInput.wireTo(&guardian.initialStateOutput, true);
        gamestate.switchTeamInput.wireTo(&guardian.switchTeamOutput, true);
        gamestate.switchKickOffInput.wireTo(&guardian.switchKickOffOutput, true);

        behaviors.localizationIn.wireTo(&localization.output);
        behaviors.filteredBallIn.wireTo(&ballTrack.ballLocationOutput);
        behaviors.gameStateIn.wireTo(&gamestate.gameStateOutput);
        // behaviors.visionFieldIn.wireTo(&vision.linesOut);
        // behaviors.visionRobotIn.wireTo(&vision.vision_robot);
        // behaviors.visionObstacleIn.wireTo(&vision.vision_obstacle);
        behaviors.fallStatusIn.wireTo(&guardian.fallStatusOutput, true);
        behaviors.motionStatusIn.wireTo(&motion.motionStatusOutput_, true);
        behaviors.odometryIn.wireTo(&motion.odometryOutput_, true);
        behaviors.jointsIn.wireTo(&sensors.jointsOutput_, true);
        behaviors.stiffStatusIn.wireTo(&sensors.stiffStatusOutput_, true);
        behaviors.sitDownIn.wireTo(&sensors.sitDownOutput_, true);
        behaviors.visionIn.wireTo(&vision.visionOut, true);
        behaviors.obstacleIn.wireTo(&obstacle.obstacleOut);
        behaviors.sharedBallIn.wireTo(&sharedBall.sharedBallOutput);
        behaviors.sharedFlipIn.wireTo(&sharedBall.sharedBallReset, true);
        behaviors.naiveBallIn.wireTo(&naiveBall.naiveBallOutput);
        for (int i = 0; i < NUM_PLAYERS_PER_TEAM; ++i)
        {
            behaviors.worldModelIn[i].wireTo(comm._worldModels[i], true);
        }

#ifdef USE_LOGGING
        {   //brackets let us hide logging code in certain IDEs.
        /*
         log threads should have low CPU time if nothing is being logged.

         That being said, should probably not init (i.e. start threads)
         if not necessary.
         */
            
            
#ifdef V5_ROBOT
        nblog::HOST_TYPE = nblog::V5ROBOT;
#else
        nblog::HOST_TYPE = nblog::V4ROBOT;
#endif
            
        nblog::HOST_NAME = robotName;
            
        printf("nblog::log_main_init()\n");
        nblog::log_main_init();
        printf("control::control_init()\n");
        control::control_init();

#ifdef START_WITH_FILEIO
#ifndef USE_LOGGING
#error "option START_WITH_FILEIO defined WITHOUT option USE_LOGGING"
#endif
            printf("CONTROL: Starting with fileio flag set!\n");
            control::flags[control::fileio] = 1;
#endif

#ifdef START_WITH_THUMBNAIL
#ifndef USE_LOGGING
#error "option START_WITH_THUMBNAIL defined WITHOUT option USE_LOGGING"
#endif
            printf("CONTROL: Starting with thumbnail flag set!\n");
            control::flags[control::thumbnail] = 1;
#endif

        /*
         SPECIFIC MODULE LOGGING
         
         CALLING NBLOG IN A SPECIFIC LOCATION, as in TRIPOINT, IS ALWAYS MORE EFFICIENT THAN THIS
         */
        sensorsThread.log<messages::JointAngles>((control::SENSORS), &sensors.jointsOutput_,
                                                 "proto-JointAngles", "sensorsThread");
        sensorsThread.log<messages::JointAngles>((control::SENSORS), &sensors.temperatureOutput_,
                                                 "proto-JointAngles", "sensorsThread");
        sensorsThread.log<messages::ButtonState>((control::SENSORS), &sensors.chestboardButtonOutput_,
                                                 "proto-ButtonState", "sensorsThread");
        sensorsThread.log<messages::FootBumperState>((control::SENSORS), &sensors.footbumperOutput_,
                                                     "proto-FootBumperState", "sensorsThread");
        sensorsThread.log<messages::InertialState>((control::SENSORS), &sensors.inertialsOutput_,
                                                   "proto-InertialState", "sensorsThread");
        sensorsThread.log<messages::SonarState>((control::SENSORS), &sensors.sonarsOutput_,
                                                "proto-SonarState", "sensorsThread");
        sensorsThread.log<messages::FSR>((control::SENSORS), &sensors.fsrOutput_,
                                         "proto-FSR", "sensorsThread");
        sensorsThread.log<messages::BatteryState>((control::SENSORS), &sensors.batteryOutput_,
                                                  "proto-BatteryState", "sensorsThread");

        guardianThread.log<messages::StiffnessControl>((control::GUARDIAN), &guardian.stiffnessControlOutput,
                                                       "proto-StiffnessControl", "guardianThread");
        guardianThread.log<messages::FeetOnGround>((control::GUARDIAN), &guardian.feetOnGroundOutput,
                                                   "proto-FeetOnGround", "guardianThread");
        guardianThread.log<messages::FallStatus>((control::GUARDIAN), &guardian.fallStatusOutput,
                                                 "proto-FallStatus", "guardianThread");
        guardianThread.log<messages::AudioCommand>((control::GUARDIAN), &guardian.audioOutput,
                                                   "proto-AudioCommand", "guardianThread");
//         cognitionThread.log<messages::RobotLocation>((control::LOCATION), &localization.output, "proto-RobotLocation", "location");
//         cognitionThread.log<messages::RobotLocation>((control::ODOMETRY), &motion.odometryOutput_, "proto-RobotLocation", "odometry");
//         cognitionThread.log<messages::VisionField>((control::OBSERVATIONS), &vision.vision_field, "proto-VisionField", "observations");
//         cognitionThread.log<messages::ParticleSwarm>((control::LOCALIZATION), &localization.particleOutput, "proto-ParticleSwarm", "localization");
//         cognitionThread.log<messages::FilteredBall>((control::BALLTRACK), &ballTrack.ballLocationOutput, "proto-FilteredBall", "balltrack");
        // cognitionThread.log<messages::VisionBall>((control::BALLTRACK), &vision.vision_ball, "proto-VisionBall", "balltrack");
        cognitionThread.log<messages::Vision>((control::VISION), &vision.visionOut,
                                                   "proto-Vision", "vision");
        // cognitionThread.log<messages::VisionField>((control::VISION), &vision.vision_field,
        //                                            "proto-VisionField", "vision");
        // cognitionThread.log<messages::VisionBall>((control::VISION), &vision.vision_ball,
        //                                           "proto-VisionBall", "vision");
        // cognitionThread.log<messages::VisionRobot>((control::VISION), &vision.vision_robot,
        //                                            "proto-VisionRobot", "vision");
        // cognitionThread.log<messages::VisionObstacle>((control::VISION), &vision.vision_obstacle,
        //                                               "proto-VisionObstacle", "vision");
        // cognitionThread.log<messages::JointAngles>((control::VISION), &vision.joint_angles_out,
        //                                            "proto-JointAngles", "vision");
        // cognitionThread.log<messages::InertialState>((control::VISION), &vision.inertial_state_out,
                                                     // "proto-InertialState", "vision");

        }
#endif //USE_LOGGING

#ifdef USE_TIME_PROFILING
        Profiler::getInstance()->profileFrames(1400);
#endif

        startSubThreads();
        std::cout << "Man built" << std::endl;
    }


Man::~Man()
{
    std::cout << "MAN::Man is being killed" << std::endl;
}

void Man::preClose()
{
    topTranscriber.closeTranscriber();
    bottomTranscriber.closeTranscriber();
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
        std::cout << thread.getName() << "thread failed to start." << std::endl;
    }
}

} // namespace man
