#include "LocalizationModule.h"

#include "Profiler.h"
#include "RoboCupGameControlData.h"
#include "DebugConfig.h"
#include "../log/logging.h"
#include "../control/control.h"

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
}

LocalizationModule::~LocalizationModule()
{
    delete particleFilter;
}

void LocalizationModule::update()
{
    // Reset filter if behaviors or shared ball make a request
#ifndef OFFLINE
    for (int i = 0; i < 2; i++) {
        if (lastReset[i] != resetInput[i].message().timestamp())
        {
            std::cout << "Reset loc on " << i << std::endl;
            lastReset[i] = resetInput[i].message().timestamp();
            particleFilter->resetLocTo(resetInput[i].message().x(),
                                       resetInput[i].message().y(),
                                       resetInput[i].message().h());
            break;
        }
    }
#endif

    // Save vision message, so can be updated for debug purposes, see
    // updateFieldForDebug in ParticleFilter.cpp
    curVision = visionInput.message();

    // Ball is only a landmark if in set
    const messages::FilteredBall* ball = NULL;
#ifndef OFFLINE
    bool inSet = (STATE_SET == gameStateInput.message().state());
    if (inSet)
        ball = &ballInput.message();
#endif

    // Update filter
    particleFilter->update(motionInput.message(), curVision, ball);

    // Output loc estimate
    portals::Message<messages::RobotLocation> locMessage(&particleFilter->getCurrentEstimate());
    output.setMessage(locMessage);

    // Output swarm
    portals::Message<messages::ParticleSwarm> swarmMessage(&particleFilter->getCurrentSwarm());
    particleOutput.setMessage(swarmMessage);

    // Logging
#ifdef USE_LOGGING
    if(control::flags[control::LOCALIZATION]) {
        ++log_index;
        std::string log_from = "loc";

        messages::RobotLocation rl = *output.getMessage(true).get();
        messages::ParticleSwarm ps = *particleOutput.getMessage(true).get();
        messages::Vision vm = curVision;

        std::string rl_buf;
        std::string ps_buf;
        std::string vm_buf;
        std::string log_buf;

        rl.SerializeToString(&rl_buf);
        ps.SerializeToString(&ps_buf);
        vm.SerializeToString(&vm_buf);

        log_buf.append(rl_buf);
        log_buf.append(ps_buf);
        log_buf.append(vm_buf);

        std::vector<SExpr> contents;

        SExpr naoLocation("location", log_from, clock(), log_index, rl_buf.length());
        contents.push_back(naoLocation);

        SExpr naoSwarm("swarm", log_from, clock(), log_index, ps_buf.length());
        contents.push_back(naoSwarm);

        SExpr naoVision("vision", log_from, clock(), log_index, vm_buf.length());
        contents.push_back(naoVision);

        NBLog(NBL_SMALL_BUFFER, "LOCSWARM", contents, log_buf);
    }
#endif
}

void LocalizationModule::run_()
{
    PROF_ENTER(P_SELF_LOC);

    // Latch inputs
    motionInput.latch();
    visionInput.latch();
#ifndef OFFLINE
    gameStateInput.latch();
    ballInput.latch();
    resetInput[0].latch();
    resetInput[1].latch();
#endif

    // Update loc
    update();

    PROF_EXIT(P_SELF_LOC);
}

} // namespace localization
} // namespace man
