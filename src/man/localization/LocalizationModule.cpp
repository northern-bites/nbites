#include "LocalizationModule.h"

namespace man
{
    namespace localization
    {
    LocalizationModule::LocalizationModule()
        : portals::Module(),
          output(base())
    {
        particleFilter = new ParticleFilter();
        particleFilter->resetLocTo(100,100,0);
    }

    LocalizationModule::~LocalizationModule()
    {
        delete particleFilter;
    }

    void LocalizationModule::update()
    {
        motionInput.latch();
        visionInput.latch();
        resetInput.latch();

        if (lastReset != resetInput.message().timestamp())
        {
            lastReset = resetInput.message().timestamp();
            particleFilter->resetLocTo(resetInput.message().x(),
                                       resetInput.message().y(),
                                       resetInput.message().h());
        }

        //std::cout << "Prepare to update filter \n";
        particleFilter->update(motionInput.message(), visionInput.message());
        //std::cout << "Updated filter \n";
        portals::Message<messages::RobotLocation> locMessage(&particleFilter->
                                                             getCurrentEstimate());
        output.setMessage(locMessage);
    }

    void LocalizationModule::run_()
    {
        update();
    }

    } // namespace localization
} // namespace localization
