#include "LocalizationModule.h"

namespace man
{
    namespace localization
    {
    LocalizationModule::LocalizationModule()
        : portals::Module(),
          output(base())
    {
        // motionModel = new MotionSystem();
        // visionModel = new VisionSystem();
        // particleFilter = new ParticleFilter(motionModel, visionModel);
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
