#include "FakeLocInputModule.h"

namespace man
{
    namespace fakeInput
    {
        FakeLocInputModule::FakeLocInputModule()
            :portals::Module(),
             fMotionOutput(base()),
             fVisionOutput(base())
        {
            // Set the initial, current, and final Location
            initialLocation.set_x(INITIAL_X);
            initialLocation.set_y(INITIAL_Y);
            initialLocation.set_h(INITIAL_H);

            currentLocation.set_x(initialLocation.x());
            currentLocation.set_y(initialLocation.y());
            currentLocation.set_h(initialLocation.h());

            finalLocation.set_x(FINAL_X);
            finalLocation.set_y(FINAL_Y);
            finalLocation.set_h(FINAL_H);

            timestamp = 0;
            frames = NUM_FRAMES;

        }

        FakeLocInputModule::~FakeLocInputModule(){}

        void FakeLocInputModule::calcDeltaMotion()
        {
            odometry.set_x((initialLocation.x() + initialLocation.x()) / frames);
            odometry.set_y((initialLocation.y() + initialLocation.y()) / frames);
            odometry.set_h((initialLocation.h() + initialLocation.h()) / frames);
        }

        void FakeLocInputModule::genNoisyOdometry()
        {
            //Determine how much noise to add (10%)
            float xVariance = (float) std::abs(odometry.x() * .05);
            float yVariance = (float) std::abs(odometry.y() * .05);
            float hVariance = (float) std::abs(odometry.h() * .05);

            // Create random number generators
            boost::mt19937 rng;
            boost::uniform_real<float> xVarRange(-1 * xVariance, xVariance);
            boost::uniform_real<float> yVarRange(-1 * yVariance, yVariance);
            boost::uniform_real<float> hVarRange(-1 * hVariance, hVariance);
            boost::variate_generator<boost::mt19937&, boost::uniform_real<float> > xNoise(rng, xVarRange);
            boost::variate_generator<boost::mt19937&, boost::uniform_real<float> > yNoise(rng, yVarRange);
            boost::variate_generator<boost::mt19937&, boost::uniform_real<float> > hNoise(rng, hVarRange);

            noisyOdometry.set_x(xNoise());
            noisyOdometry.set_y(yNoise());
            noisyOdometry.set_h(hNoise());
        }

        void FakeLocInputModule::addGoalObservation(bool rightGoal)
        {
            messages::PVisualGoalPost goalObservation;
            messages::PVisualDetection goalVisualDetection;
        }


        void FakeLocInputModule::genVisualDetection(messages::PVisualDetection &visualDetection)
        {
            // Choose concrete_coords randomly for the observation
            //Create a random number generator
            boost::mt19937 rng;
            boost::uniform_real<float> observationRange(MIN_OBS_DIST, MAX_OBS_DIST);
            boost::variate_generator<boost::mt19937&, boost::uniform_real<float> > randOffset(rng, observationRange);
            // @TODO one line? Don't think so but try
            messages::Point concreteCoords;
            concreteCoords.set_x(currentLocation.x() + randOffset());
            concreteCoords.set_y(currentLocation.y() + randOffset());
            visualDetection.add_concrete_coords()->CopyFrom(concreteCoords);

            // Fill in the Visual Detections distance and bearing with noise
            // Use a gaussian since that's the assumption for a particle filter

            float translatedX = concreteCoords.x() - currentLocation.x();
            float translatedY = concreteCoords.y() - currentLocation.y();

            float calcDistance = std::sqrt(NBMath::square(translatedX) + NBMath::square(translatedY));
            float calcBearing = NBMath::safe_atan2(translatedY, translatedX);

            boost::normal_distribution<float> distDistrib(calcDistance, DIST_STD_DEV);
            boost::normal_distribution<float> bearingDistrib(calcBearing, BEAR_STD_DEV);

            boost::variate_generator<boost::mt19937&,
                                     boost::normal_distribution<float> > distanceGen(rng, distDistrib);
            boost::variate_generator<boost::mt19937&,
                                     boost::normal_distribution<float> > bearingGen(rng, bearingDistrib);

            visualDetection.set_distance(distanceGen());
            visualDetection.set_bearing(bearingGen());
        }

        void FakeLocInputModule::addCornerObservation()
        {
            messages::PVisualCorner cornerObservation;
            // Each corner observation needs a visual detection
            messages::PVisualDetection cornerVisualDetection;

            // add the visual detection to the visual corner
            cornerObservation.mutable_visual_detection()->CopyFrom(cornerVisualDetection);

            // add the visual corner to the noisyVision message
            noisyVision.add_visual_corner()->CopyFrom(cornerObservation);
        }

        void FakeLocInputModule::genNoisyVision()
        {
            // Clear the current noisy vision
            noisyVision.Clear();

            // Increment the timestamp
            noisyVision.set_timestamp((google::protobuf::int64) timestamp);

            // Add a corner observation
            addCornerObservation();
        }

        void FakeLocInputModule::run_()
        {
            noisyMotion.set_timestamp((google::protobuf::int64) timestamp);
            noisyMotion.mutable_odometry()->CopyFrom(noisyOdometry);

            // Create a Message for the motion portal, set it appropriatly
            portals::Message<messages::Motion> odometryMessage(0);
            *odometryMessage.get() = messages::Motion();
            odometryMessage.get()->mutable_odometry()->CopyFrom(noisyMotion);
            fMotionOutput.setMessage(odometryMessage);

            // Set the correct actual location now that we've 'traveled' there
            currentLocation.set_x(currentLocation.x() + odometry.x());
            currentLocation.set_y(currentLocation.y() + odometry.y());
            currentLocation.set_h(currentLocation.h() + odometry.h());

            // Generate observations for this new location


            // increment timestamp, set negative if gen'd all frames so we terminate
            timestamp++;
            if (timestamp > frames)
                timestamp = -1;
        }


    } //namespace fakeInput
} // namespace man
