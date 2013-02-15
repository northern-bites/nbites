#include "LocalizationModule.h"

namespace man
{
    namespace localization
    {


    LocalizationModule::LocalizationModule()
    {
//        motionModel = new MotionSystem();
//        visionModel = new VisionSystem();
//        particleFilter = new ParticleFilter(motionModel, visionModel);
    }

    LocalizationModule::~LocalizationModule()
    {
    }

    void LocalizationModule::update()
    {
        // particleFilter.update(motionInput.message(), visionInput.message());
        // output.setMessage(particleFilter.getEstimate());
    }

    void LocalizationModule::run()
    {
        update();
    }

    } // namespace localization
} // namespace localization
