#include "LocalizationModule.h"

namespace man
{
    namespace localization
    {
    LocalizationModule::LocalizationModule()
        : portals::Module(),
          output(base())
    {
        motionModel = boost::shared_ptr<MotionSystem>(new MotionSystem());
//        visionModel = boost::shared_ptr<VisionSystem>(new VisionSystem());

        // particleFilter = boost::shared_ptr<ParticleFilter>(new ParticleFilter(motionModel,
        //                                                                visionModel));
    }

    LocalizationModule::~LocalizationModule()
    {
    }

    void LocalizationModule::update()
    {
        messages::RobotLocation jokeOutput;
        jokeOutput.set_x(1);
        jokeOutput.set_y(2);
        jokeOutput.set_h(3);

        portals::Message<messages::RobotLocation> locMessage(0);
        *locMessage.get() = messages::RobotLocation();
        locMessage.get()->CopyFrom(jokeOutput);
        output.setMessage(locMessage);
        // particleFilter.update(motionInput.message(), visionInput.message());
        // output.setMessage(const particleFilter.getCurrentEstimate());
    }

    void LocalizationModule::run_()
    {
        update();
    }

    } // namespace localization
} // namespace localization
