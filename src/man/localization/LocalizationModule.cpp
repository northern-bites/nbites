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
        visionModel = boost::shared_ptr<VisionSystem>(new VisionSystem());

        particleFilter = boost::shared_ptr<ParticleFilter>(new ParticleFilter(motionModel,
                                                                       visionModel));
    }

    LocalizationModule::~LocalizationModule()
    {
    }

    void LocalizationModule::update()
    {
        // particleFilter.update(motionInput.message(), visionInput.message());
        // output.setMessage(const particleFilter.getCurrentEstimate());
    }

    void LocalizationModule::run_()
    {
        update();
    }

    } // namespace localization
} // namespace localization
