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
    for (int i = 0; i < 2; i++) {
        if (lastReset[i] != resetInput[i].message().timestamp())
        {
            // std::cout<<"RESET LOC ON "<<i<<std::endl;
            lastReset[i] = resetInput[i].message().timestamp();
            particleFilter->resetLocTo(resetInput[i].message().x(),
                                       resetInput[i].message().y(),
                                       resetInput[i].message().h());
            break;
        }
    }
#endif

    // Save odometry and lines
    curOdometry.set_x(motionInput.message().x());
    curOdometry.set_y(motionInput.message().y());
    curOdometry.set_h(motionInput.message().h());
    curLines = linesInput.message();
    curCorners = cornersInput.message();

#ifndef OFFLINE
    // bool inSet = (STATE_SET == gameStateInput.message().state());
    // // Update the Particle Filter with the new observations/odometry

    // if (inSet && (!gameStateInput.message().have_remote_gc() || 
    //     gameStateInput.message().secs_remaining() != 600))
    //     particleFilter->update(curOdometry, visionInput.message(), ballInput.message());
    // else
#endif

    // Update filter
    particleFilter->update(curOdometry, curLines, curCorners);

//this is part of something old that never executes, check out
//the ifdef below; same code but it is executed when we want to
//to log localization
#if defined( LOG_LOCALIZATION) || defined(OFFLINE)
    portals::Message<messages::ParticleSwarm> swarmMessage(&particleFilter->getCurrentSwarm());
    particleOutput.setMessage(swarmMessage);
#endif

    portals::Message<messages::RobotLocation> locMessage(&particleFilter->getCurrentEstimate());
    output.setMessage(locMessage);

#ifdef USE_LOGGING
    if(control::flags[control::LOCALIZATION]) {
        ++log_index;
        std::string log_from = "loc";

        portals::Message<messages::ParticleSwarm> swarmMessage(&particleFilter->
                                                           getCurrentSwarm());
        particleOutput.setMessage(swarmMessage);

        messages::RobotLocation rl = *output.getMessage(true).get();
        messages::ParticleSwarm ps = *particleOutput.getMessage(true).get();
        messages::FieldLines fl = curLines;

        std::string rl_buf;
        std::string ps_buf;
        std::string fl_buf;
        std::string log_buf;

        rl.SerializeToString(&rl_buf);
        ps.SerializeToString(&ps_buf);
        fl.SerializeToString(&fl_buf);

        log_buf.append(rl_buf);
        log_buf.append(ps_buf);
        log_buf.append(fl_buf);

        std::vector<SExpr> contents;

        SExpr naoLocation("location", log_from, clock(), log_index, rl_buf.length());
        contents.push_back(naoLocation);

        SExpr naoSwarm("swarm",log_from,clock(),log_index,ps_buf.length());
        contents.push_back(naoSwarm);

        SExpr naoFieldLines("fieldlines",log_from,clock(),log_index,fl_buf.length());
        contents.push_back(naoFieldLines);

        NBLog(NBL_SMALL_BUFFER,"LOCSWARM",contents,log_buf);
    }
#endif

}

void LocalizationModule::run_()
{
    PROF_ENTER(P_SELF_LOC);

    motionInput.latch();
    linesInput.latch();
    cornersInput.latch();
#ifndef OFFLINE
    gameStateInput.latch();
    // ballInput.latch();
    resetInput[0].latch();
    resetInput[1].latch();
#endif

    update();

    PROF_EXIT(P_SELF_LOC);
}

} // namespace localization
} // namespace man
