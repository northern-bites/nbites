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
    // TODO delete?
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
    for (int i = 0; i < 1; i++) {
        if (lastReset[i] != resetInput[i].message().timestamp())
        {
            std::cout<<"RESET LOC ON "<<i<<std::endl;
            lastReset[i] = resetInput[i].message().timestamp();
            particleFilter->resetLocTo(resetInput[i].message().x(),
                                       resetInput[i].message().y(),
                                       resetInput[i].message().h());
            break;
        }
    }
#endif

    // Save odometry
    curOdometry.set_x(motionInput.message().x());
    curOdometry.set_y(motionInput.message().y());
    curOdometry.set_h(motionInput.message().h());

#ifndef OFFLINE
    // bool inSet = (STATE_SET == gameStateInput.message().state());
    // // Update the Particle Filter with the new observations/odometry

    // if (inSet && (!gameStateInput.message().have_remote_gc() || 
    //     gameStateInput.message().secs_remaining() != 600))
    //     particleFilter->update(curOdometry, visionInput.message(), ballInput.message());
    // else
#endif

    // Update filter
    particleFilter->update(curOdometry, visionInput.message());

    // Update outportals
#if defined( LOG_LOCALIZATION) || defined(OFFLINE)
    portals::Message<messages::ParticleSwarm> swarmMessage(&particleFilter->getCurrentSwarm());
    particleOutput.setMessage(swarmMessage);
#endif

    portals::Message<messages::RobotLocation> locMessage(&particleFilter->getCurrentEstimate());
    output.setMessage(locMessage);
}

void LocalizationModule::run_()
{
    PROF_ENTER(P_SELF_LOC);

    motionInput.latch();
    visionInput.latch();
#ifndef OFFLINE
    gameStateInput.latch();
    // ballInput.latch();
    resetInput[0].latch();
    // resetInput[1].latch();
#endif

    update();

    PROF_EXIT(P_SELF_LOC);
}

} // namespace localization
} // namespace man
