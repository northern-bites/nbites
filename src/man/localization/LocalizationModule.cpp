#include "LocalizationModule.h"
#include "Profiler.h"

namespace man
{
    namespace localization
    {
    LocalizationModule::LocalizationModule()
        : portals::Module(),
          output(base()),
          particleOutput(base())
    {
        particleFilter = new ParticleFilter();
        particleFilter->resetLocTo(100,100,0);

        std::cout << "Logging localization? ";
#ifdef LOG_LOCALIZATION
        std::cout << "Yes." << std::endl;
#else
        std::cout << "No." << std::endl;
#endif

        //Note: All the RobotLocation messages default to zero values to start (yay!)
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

        // NOTE: Particle Filter wants to get deltaX, deltaY, etc...
        lastOdometry.set_x(curOdometry.x());
        lastOdometry.set_y(curOdometry.y());
        lastOdometry.set_h(curOdometry.h());

        curOdometry.set_x(motionInput.message().x());
        curOdometry.set_y(motionInput.message().y());
        curOdometry.set_h(motionInput.message().h());

        deltaOdometry.set_x(curOdometry.x() - lastOdometry.x());
        deltaOdometry.set_y(curOdometry.y() - lastOdometry.y());
        deltaOdometry.set_h(curOdometry.h() - lastOdometry.h());

        particleFilter->update(deltaOdometry, visionInput.message());

        portals::Message<messages::RobotLocation> locMessage(&particleFilter->
                                                             getCurrentEstimate());

#ifdef LOG_LOCALIZATION
        portals::Message<messages::ParticleSwarm> swarmMessage(&particleFilter->
                                                               getCurrentSwarm());
        particleOutput.setMessage(swarmMessage);
#endif

        output.setMessage(locMessage);
    }

    void LocalizationModule::run_()
    {
        PROF_ENTER(P_LOC);
        update();
        PROF_EXIT(P_LOC);
    }

    } // namespace localization
} // namespace man
