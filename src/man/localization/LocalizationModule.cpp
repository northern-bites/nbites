#include "LocalizationModule.h"
#include "Profiler.h"
#include "RoboCupGameControlData.h"

#include "DebugConfig.h"
#include "../log/logging.h"
#include "../control/control.h"
#include "nbdebug.h"

using nblog::SExpr;
using nblog::NBLog;

namespace man {
namespace localization {

LocalizationModule::LocalizationModule()
    : portals::Module(),
      output(base()),
      particleOutput(base()),
      log_index(0)
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
    for (int i = 0; i < 2; i++) {
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

    curOdometry.set_x(motionInput.message().x());
    curOdometry.set_y(motionInput.message().y());
    curOdometry.set_h(motionInput.message().h());

#ifndef OFFLINE
    bool inSet = (STATE_SET == gameStateInput.message().state());
    // Update the Particle Filter with the new observations/odometry

    if (inSet && (!gameStateInput.message().have_remote_gc() || 
        gameStateInput.message().secs_remaining() != 600))
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

#ifdef USE_LOGGING
    if(control::flags[control::LOCALIZATION]) {
        ++log_index;
        std::string log_from = "loc";

        messages::RobotLocation rl = *output.getMessage(true).get();
        messages::ParticleSwarm ps = *particleOutput.getMessage(true).get();

        std::string rl_buf;
        std::string ps_buf;
        std::string log_buf;

        rl.SerializeToString(&rl_buf);
        ps.SerializeToString(&ps_buf);

        log_buf.append(rl_buf);
        log_buf.append(ps_buf);

        std::vector<SExpr> contents;

        SExpr naoLocation("location", log_from, clock(), log_index, rl_buf.length());
        naoLocation.append(SExpr("locProto",rl_buf));
        //naoLocation.append(SExpr("x",rl.x()));
        //naoLocation.append(SExpr("y",rl.y()));
        //naoLocation.append(SExpr("h",rl.h()));

        std::cout<<"[DEBUG] LOCATION"<<std::endl;
        std::cout<<naoLocation.print()<<std::endl;

        contents.push_back(naoLocation);

        SExpr naoSwarm("swarm",log_from,clock(),log_index,ps_buf.length());
        naoSwarm.append(SExpr("partProto",ps_buf));

        std::cout<<"[DEBUG] SWARM"<<std::endl;
        std::cout<<naoSwarm.print()<<std::endl;

        contents.push_back(naoSwarm);
        NBLog(NBL_SMALL_BUFFER,"LOCSWARM",contents,log_buf);
    }
#endif

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
    resetInput[0].latch();
    resetInput[1].latch();
#endif

    update();

    // Profiler
    PROF_EXIT(P_SELF_LOC);
}

} // namespace localization
} // namespace man
