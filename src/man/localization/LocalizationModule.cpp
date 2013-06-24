#include "LocalizationModule.h"
#include "Profiler.h"
#include "RoboCupGameControlData.h"

namespace man {
namespace localization {

LocalizationModule::LocalizationModule()
    : portals::Module(),
      output(base()),
      particleOutput(base())
{
    particleFilter = new ParticleFilter();
    // Chooose on the field looking up as a random initial
    particleFilter->resetLocTo(110,658,-1.5);
}

LocalizationModule::~LocalizationModule()
{
    delete particleFilter;
}

void LocalizationModule::update()
{
#ifndef OFFLINE
    // Modify based on control portal
    if (lastReset != resetInput.message().timestamp())
    {
        lastReset = resetInput.message().timestamp();
        particleFilter->resetLocTo(resetInput.message().x(),
                                   resetInput.message().y(),
                                   resetInput.message().h());
    }
#endif

    curOdometry.set_x(motionInput.message().x());
    curOdometry.set_y(motionInput.message().y());
    curOdometry.set_h(motionInput.message().h());

#ifndef OFFLINE
    bool inSet = (STATE_SET == gameStateInput.message().state());
    // Update the Particle Filter with the new observations/odometry

    if (inSet)
        particleFilter->update(curOdometry, visionInput.message(), ballInput.message());
    else
#endif
        particleFilter->update(curOdometry, visionInput.message());

    // Update the locMessage and the swarm (if logging)
    portals::Message<messages::RobotLocation> locMessage(&particleFilter->
                                                         getCurrentEstimate());
#if defined( LOG_LOCALIZATION) || defined(OFFLINE)
    portals::Message<messages::ParticleSwarm> swarmMessage(&particleFilter->
                                                           getCurrentSwarm());
    particleOutput.setMessage(swarmMessage);
#endif

    output.setMessage(locMessage);
}

void LocalizationModule::run_()
{
    // Profiler
    PROF_ENTER(P_SELF_LOC);

    motionInput.latch();
    visionInput.latch();
#ifndef OFFLINE
    gameStateInput.latch();
    ballInput.latch();
    resetInput.latch();
#endif

    update();

    // Profiler
    PROF_EXIT(P_SELF_LOC);
}

} // namespace localization
} // namespace man
