#include "LocalizationModule.h"
#include "Profiler.h"

namespace man
{
namespace localization
{
static const float ODOMETRY_HEADING_FRICTION_FACTOR = 2.5;
static const float ODOMETRY_X_FRICTION_FACTOR = 1.12;
static const float ODOMETRY_Y_FRICTION_FACTOR = 1;

LocalizationModule::LocalizationModule()
    : portals::Module(),
      output(base()),
      particleOutput(base())
{
    particleFilter = new ParticleFilter();
    // Chooose on the field looking up as a random initial
    particleFilter->resetLocTo(100,100,0);
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

    // Maybe B-Human has a different coordinate frame for odometry...
    // This is fucking absurd but I'm going to rotate the x and y by the h...
    float sinH, cosH;
    sincosf(motionInput.message().h(), &sinH, &cosH);
    float rotatedX =   cosH*motionInput.message().x()
                     + sinH*motionInput.message().y();
    float rotatedY =   cosH*motionInput.message().y()
                     - sinH*motionInput.message().x();

    curOdometry.set_x(rotatedX * ODOMETRY_X_FRICTION_FACTOR);
    curOdometry.set_y(rotatedY * ODOMETRY_Y_FRICTION_FACTOR);
    curOdometry.set_h(motionInput.message().h() * ODOMETRY_HEADING_FRICTION_FACTOR);

    deltaOdometry.set_x(curOdometry.x() - lastOdometry.x());
    deltaOdometry.set_y(curOdometry.y() - lastOdometry.y());
    deltaOdometry.set_h(curOdometry.h() - lastOdometry.h());

    // Ensure deltaOdometry is reasonable (initial fix lost in git?)
    if((fabs(deltaOdometry.x()) > 3.f) || (fabs(deltaOdometry.y()) > 3.f))
    {
        deltaOdometry.set_x(0.f);
        deltaOdometry.set_y(0.f);
        deltaOdometry.set_h(0.f);
    }

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
    // Profiler
    PROF_ENTER(P_SELF_LOC);

    // Get new information
    motionInput.latch();
    visionInput.latch();
    resetInput.latch();

    // Update the filter
    update();

    // Profiler
    PROF_EXIT(P_SELF_LOC);
}

} // namespace localization
} // namespace man
