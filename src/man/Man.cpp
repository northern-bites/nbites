#include "Man.h"

#include <iostream>

#include "Common.h"
#include "Profiler.h"
#include "Camera.h"

#include "log/log_header.h"
#include "log/log_sf.h"

#ifndef OFFLINE
SET_POOL_SIZE(messages::WorldModel,  24);
SET_POOL_SIZE(messages::JointAngles, 24);
SET_POOL_SIZE(messages::InertialState, 16);
SET_POOL_SIZE(messages::PackedImage16, 16);
SET_POOL_SIZE(messages::YUVImage, 16);
SET_POOL_SIZE(messages::RobotLocation, 16);
#endif

namespace man {
    
    Man::Man(boost::shared_ptr<AL::ALBroker> broker, const std::string &name)
    : AL::ALModule(broker, name),
    param("/home/nao/nbites/lib/parameters.json"),
    playerNum(param.getParam<int>("playerNumber")),
    teamNum(param.getParam<int>("teamNumber")),
    sensorsThread("sensors", SENSORS_FRAME_LENGTH_uS),
    sensors(broker),
    jointEnactor(broker),
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
    topConverter(TOP_TABLE_PATHNAME),
    bottomConverter(BOTTOM_TABLE_PATHNAME),
    vision(),
    localization(),
    ballTrack(),
    obstacle(),
    gamestate(teamNum, playerNum),
    behaviors(teamNum, playerNum),
    leds(broker),
    sharedBall(playerNum)
    {
        setModuleDescription("The Northern Bites' soccer player.");
        
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
        cognitionThread.addModule(topConverter);
        cognitionThread.addModule(bottomConverter);
        cognitionThread.addModule(vision);
        cognitionThread.addModule(localization);
        cognitionThread.addModule(ballTrack);
        cognitionThread.addModule(obstacle);
        cognitionThread.addModule(gamestate);
        cognitionThread.addModule(behaviors);
        cognitionThread.addModule(leds);
        cognitionThread.addModule(sharedBall);
        
        topTranscriber.jointsIn.wireTo(&sensors.jointsOutput_, true);
        topTranscriber.inertsIn.wireTo(&sensors.inertialsOutput_, true);
        bottomTranscriber.jointsIn.wireTo(&sensors.jointsOutput_, true);
        bottomTranscriber.inertsIn.wireTo(&sensors.inertialsOutput_, true);
        
        topConverter.imageIn.wireTo(&topTranscriber.imageOut);
        bottomConverter.imageIn.wireTo(&bottomTranscriber.imageOut);
        
        vision.topThrImage.wireTo(&topConverter.thrImage);
        vision.topYImage.wireTo(&topConverter.yImage);
        vision.topUImage.wireTo(&topConverter.uImage);
        vision.topVImage.wireTo(&topConverter.vImage);
        
        vision.botThrImage.wireTo(&bottomConverter.thrImage);
        vision.botYImage.wireTo(&bottomConverter.yImage);
        vision.botUImage.wireTo(&bottomConverter.uImage);
        vision.botVImage.wireTo(&bottomConverter.vImage);
        
        vision.joint_angles.wireTo(&topTranscriber.jointsOut, true);
        vision.inertial_state.wireTo(&topTranscriber.inertsOut, true);
        
        localization.visionInput.wireTo(&vision.vision_field);
        localization.motionInput.wireTo(&motion.odometryOutput_, true);
        localization.resetInput[0].wireTo(&behaviors.resetLocOut, true);
        localization.resetInput[1].wireTo(&sharedBall.sharedBallReset, true);
        localization.gameStateInput.wireTo(&gamestate.gameStateOutput);
        localization.ballInput.wireTo(&ballTrack.ballLocationOutput);
        
        ballTrack.visionBallInput.wireTo(&vision.vision_ball);
        ballTrack.odometryInput.wireTo(&motion.odometryOutput_, true);
        ballTrack.localizationInput.wireTo(&localization.output, true);
        
        for (int i = 0; i < NUM_PLAYERS_PER_TEAM; ++i)
        {
            sharedBall.worldModelIn[i].wireTo(comm._worldModels[i], true);
        }
        sharedBall.locIn.wireTo(&localization.output);
        sharedBall.ballIn.wireTo(&ballTrack.ballLocationOutput);
        
        obstacle.armContactIn.wireTo(&arms.contactOut, true);
        obstacle.visionIn.wireTo(&vision.vision_obstacle, true);
        obstacle.sonarIn.wireTo(&sensors.sonarsOutput_, true);
        
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
        behaviors.fallStatusIn.wireTo(&guardian.fallStatusOutput, true);
        behaviors.motionStatusIn.wireTo(&motion.motionStatusOutput_, true);
        behaviors.odometryIn.wireTo(&motion.odometryOutput_, true);
        behaviors.jointsIn.wireTo(&sensors.jointsOutput_, true);
        behaviors.stiffStatusIn.wireTo(&sensors.stiffStatusOutput_, true);
        behaviors.obstacleIn.wireTo(&obstacle.obstacleOut);
        behaviors.sharedBallIn.wireTo(&sharedBall.sharedBallOutput);
        behaviors.sharedFlipIn.wireTo(&sharedBall.sharedBallReset, true);
        
        for (int i = 0; i < NUM_PLAYERS_PER_TEAM; ++i)
        {
            behaviors.worldModelIn[i].wireTo(comm._worldModels[i], true);
        }
        
        leds.ledCommandsIn.wireTo(&behaviors.ledCommandOut);
        
#ifdef USE_LOGGING
        /*
         log threads should have low CPU time if nothing is being logged.
         
         That being said, should probably not init (i.e. start threads)
         if not necessary.
         */
        printf("nblog::log_main_init()\n");
        nblog::log_main_init();
        
        /*
         SPECIFIC MODULE LOGGING
         */
//#ifdef LOG_SENSORS
        sensorsThread.log<messages::JointAngles>((nbsf::SENSORS), &sensors.jointsOutput_,
                                                 "proto-JointAngles from=jointsOutput");
        sensorsThread.log<messages::JointAngles>((nbsf::SENSORS), &sensors.temperatureOutput_,
                                                 "proto-JointAngles from=temperatureOutput");
        sensorsThread.log<messages::ButtonState>((nbsf::SENSORS), &sensors.chestboardButtonOutput_,
                                                 "proto-ButtonState from=chestboardButtonOutput");
        sensorsThread.log<messages::FootBumperState>((nbsf::SENSORS), &sensors.footbumperOutput_,
                                                     "proto-FootBumperState from=footbumperOutput");
        sensorsThread.log<messages::InertialState>((nbsf::SENSORS), &sensors.inertialsOutput_,
                                                   "proto-InertialState from=inertialsOutput");
        sensorsThread.log<messages::SonarState>((nbsf::SENSORS), &sensors.sonarsOutput_,
                                                "proto-SonarState from=sonarsOutput");
        sensorsThread.log<messages::FSR>((nbsf::SENSORS), &sensors.fsrOutput_,
                                         "proto-FSR from=fsrOutput");
        sensorsThread.log<messages::BatteryState>((nbsf::SENSORS), &sensors.batteryOutput_,
                                                  "proto-BatteryState from=batteryOutput");
//#endif
        
//#ifdef LOG_GUARDIAN
        guardianThread.log<messages::StiffnessControl>((nbsf::GUARDIAN), &guardian.stiffnessControlOutput,
                                                       "proto-StiffnessControl from=stiffnessControlOutput");
        guardianThread.log<messages::FeetOnGround>((nbsf::GUARDIAN), &guardian.feetOnGroundOutput,
                                                   "proto-FeetOnGround from=feetOnGroundOutput");
        guardianThread.log<messages::FallStatus>((nbsf::GUARDIAN), &guardian.fallStatusOutput,
                                                 "proto-FallStatus from=fallStatusOutput");
        guardianThread.log<messages::AudioCommand>((nbsf::GUARDIAN), &guardian.audioOutput,
                                                   "proto-AudioCommand from=audioOutput");
//#endif
        
//#ifdef LOG_LOCATION
        cognitionThread.log<messages::RobotLocation>((nbsf::LOCATION), &localization.output, "proto-RobotLocation from=cognition");
//#endif
        
//#ifdef LOG_ODOMETRY
        cognitionThread.log<messages::RobotLocation>((nbsf::ODOMETRY), &motion.odometryOutput_, "proto-RobotLocation from=odometryOutput");
//#endif
        
//#ifdef LOG_OBSERVATIONS
        cognitionThread.log<messages::VisionField>((nbsf::OBSERVATIONS), &vision.vision_field, "proto-VisionField from=observations");
//#endif
        
//#ifdef LOG_LOCALIZATION
        cognitionThread.log<messages::ParticleSwarm>((nbsf::LOCALIZATION), &localization.particleOutput, "proto-ParticleSwarm from=localization");
//#endif
        
//#ifdef LOG_BALLTRACK
        cognitionThread.log<messages::FilteredBall>((nbsf::BALLTRACK), &ballTrack.ballLocationOutput, "proto-FilteredBall from=ballLocationOutput");
        cognitionThread.log<messages::VisionBall>((nbsf::BALLTRACK), &vision.vision_ball, "proto-VisionBall from=vision_ball");
//#endif
        
//#ifdef LOG_IMAGES
        cognitionThread.log<messages::YUVImage>((nbsf::IMAGES), &topTranscriber.imageOut,
                                                "YUVImage from=top");
        cognitionThread.log<messages::YUVImage>((nbsf::IMAGES), &bottomTranscriber.imageOut,
                                                "YUVImage from=bot");
//#endif
        
//#ifdef LOG_VISION
        cognitionThread.log<messages::VisionField>((nbsf::VISION), &vision.vision_field,
                                                   "proto-VisionField from=vision");
        cognitionThread.log<messages::VisionBall>((nbsf::VISION), &vision.vision_ball,
                                                  "proto-VisionBall from=vision");
        cognitionThread.log<messages::VisionRobot>((nbsf::VISION), &vision.vision_robot,
                                                   "proto-VisionRobot from=vision");
        cognitionThread.log<messages::VisionObstacle>((nbsf::VISION), &vision.vision_obstacle,
                                                      "proto-VisionObstacle from=vision");
        cognitionThread.log<messages::JointAngles>((nbsf::VISION), &vision.joint_angles_out,
                                                   "proto-JointAngles from=vision");
        cognitionThread.log<messages::InertialState>((nbsf::VISION), &vision.inertial_state_out,
                                                     "proto-InertialState from=vision");
//#endif
        
        
#endif //USE_LOGGING
        
#ifdef USE_TIME_PROFILING
        Profiler::getInstance()->profileFrames(1400);
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
