#include "LocalizationModule.h"

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
        // Chooose on the field looking up as a random initial
        particleFilter->resetLocTo(100,100,0);

        std::cout << "Logging localization? ";
#ifdef LOG_LOCALIZATION
        std::cout << "Yes." << std::endl;
#else
        std::cout << "No." << std::endl;
#endif
    }

    LocalizationModule::~LocalizationModule()
    {
        delete particleFilter;
    }

    void LocalizationModule::update()
    {
        // Modify based on control portal
        if (lastReset != resetInput.message().timestamp())
        {
            lastReset = resetInput.message().timestamp();
            particleFilter->resetLocTo(resetInput.message().x(),
                                       resetInput.message().y(),
                                       resetInput.message().h());
        }

        // Calculate the deltaX,Y,H (PF takes increments from robot frame)
        lastOdometry.set_x(curOdometry.x());
        lastOdometry.set_y(curOdometry.y());
        lastOdometry.set_h(curOdometry.h());

        curOdometry.set_x(motionInput.message().x());
        curOdometry.set_y(motionInput.message().y());
        curOdometry.set_h(motionInput.message().h());

        deltaOdometry.set_x(curOdometry.x() - lastOdometry.x());
        deltaOdometry.set_y(curOdometry.y() - lastOdometry.y());
        deltaOdometry.set_h(curOdometry.h() - lastOdometry.h());

        // Update the Particle Filter with the new observations/odometry
        particleFilter->update(deltaOdometry, visionInput.message());

        // Update the locMessage and the swarm (if logging)
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
        // Get new information
        motionInput.latch();
        visionInput.latch();
        resetInput.latch();

        // Update the filter
        update();
    }

    } // namespace localization
} // namespace man
