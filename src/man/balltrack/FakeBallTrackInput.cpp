#include "FakeBallTrackInput.h"

namespace man{
    namespace balltrack
    {

    FakeBallInputModule::FakeBallInputModule()
        :portals::Module(),
         fVisionBallOutput(base()),
         fMotionOutput(base())
    {
       // Set the current Location
        currentLocation.set_x(INITIAL_X);
        currentLocation.set_y(INITIAL_Y);
        currentLocation.set_h(INITIAL_H);

        // For now assume the robot isnt moving
        currentOdometry.set_x(0.f);
        currentOdometry.set_y(0.f);
        currentOdometry.set_h(0.f);

        curBallDist = INITIAL_BALL_DIST;
        curBallBear = INITIAL_BALL_BEAR;

        timestamp = 0;
    }

    FakeBallInputModule::~FakeBallInputModule(){}

    void FakeBallInputModule::run_()
    {
        timestamp++;

        // Generate and post the motion message
        updateNoisyMotion();

        portals::Message<messages::Motion> odometryMessage(0);
        *odometryMessage.get() = messages::Motion();
        odometryMessage.get()->CopyFrom(noisyMotion);
        fMotionOutput.setMessage(odometryMessage);

        // Generate and post the vision message
        updateNoisyVision();
        portals::Message<messages::VisionBall> visionBallMessage(0);
        *visionBallMessage.get() = messages::VisionBall();
        visionBallMessage.get()->CopyFrom(currentBallObsv);
        fVisionBallOutput.setMessage(visionBallMessage);
    }

    /*
     * @todo make this do more after basic KF testing
     */
    void FakeBallInputModule::updateNoisyMotion()
    {
        noisyMotion.set_timestamp((google::protobuf::int64) timestamp);
        noisyMotion.mutable_odometry()->CopyFrom(currentOdometry);
    }

    void FakeBallInputModule::updateNoisyVision()
    {
        noisyVision.set_timestamp((google::protobuf::int64) timestamp);

        // Generate a ball observation
        currentBallObsv.set_distance(curBallDist);
        currentBallObsv.set_bearing(curBallBear);
        currentBallObsv.set_distance_sd(DIST_STD_DEV);
        currentBallObsv.set_bearing_sd(BEAR_STD_DEV);
    }

    } //namespace balltrack
} // namespace man
