#include "Man.h"
#include "Common.h"
#include "Camera.h"
#include <iostream>
#include "RobotConfig.h"

namespace man {

Man::Man(boost::shared_ptr<AL::ALBroker> broker, const std::string &name)
    : AL::ALModule(broker, name),
      sensorsThread("sensors", SENSORS_FRAME_LENGTH_uS),
      sensors(broker),
      guardianThread("guardian", GUARDIAN_FRAME_LENGTH_uS),
      guardian(),
      audio(broker),
      commThread("comm", COMM_FRAME_LENGTH_uS),
      comm(MY_TEAM_NUMBER, MY_PLAYER_NUMBER),
      cognitionThread("cognition", COGNITION_FRAME_LENGTH_uS),
      topTranscriber(*new image::ImageTranscriber(Camera::TOP)),
      bottomTranscriber(*new image::ImageTranscriber(Camera::BOTTOM)),
      topConverter(Camera::TOP),
      bottomConverter(Camera::BOTTOM),
      vision(),
      ballTrack(),
      leds(broker),
      behaviors()
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

    // Turn ON the finalize method for images, which we've specialized
    portals::Message<messages::YUVImage>::setFinalize(true);
    portals::Message<messages::ThresholdImage>::setFinalize(true);
    portals::Message<messages::PackedImage16>::setFinalize(true);
    portals::Message<messages::PackedImage8>::setFinalize(true);

    cognitionThread.addModule(topTranscriber   );
    cognitionThread.addModule(bottomTranscriber);
    cognitionThread.addModule(topConverter     );
    cognitionThread.addModule(bottomConverter  );
    topConverter   .imageIn.wireTo(&   topTranscriber.imageOut);
    bottomConverter.imageIn.wireTo(&bottomTranscriber.imageOut);


#ifdef LOG_IMAGES
    cognitionThread.log<messages::YUVImage>(&topTranscriber.imageOut,
                                            "top");
    cognitionThread.log<messages::YUVImage>(&bottomTranscriber.imageOut,
                                            "bottom");
#endif

    cognitionThread.addModule(vision);
    vision.topThrImage.wireTo(&topConverter.thrImage);
	vision.topYImage.wireTo(&topConverter.yImage);
	vision.topUImage.wireTo(&topConverter.uImage);
	vision.topVImage.wireTo(&topConverter.vImage);

	vision.botThrImage.wireTo(&bottomConverter.thrImage);
	vision.botYImage.wireTo(&bottomConverter.yImage);
	vision.botUImage.wireTo(&bottomConverter.uImage);
	vision.botVImage.wireTo(&bottomConverter.vImage);

	vision.joint_angles.wireTo(&sensors.jointsOutput_, true);
    vision.inertial_state.wireTo(&sensors.inertialsOutput_, true);

    //cognitionThread.addModule(ballTrack);
    cognitionThread.addModule(leds);
    cognitionThread.addModule(behaviors);
    leds.ledCommandsIn.wireTo(&behaviors.ledCommandOut, false);
    behaviors.gameStateIn.wireTo(&comm._gameStateOutput, true);
    //behaviors.filteredBallIn.wireTo(&ballTrack.ballLocationOutput, true);

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
