#include "LocalizationModule.h"
#include "Profiler.h"
#include "RoboCupGameControlData.h"

#include "DebugConfig.h"

#include "HighResTimer.h"

#include "ParticleStruct.h"

#include "Logging.hpp"
#include "Control.hpp"
using nbl::SExpr;
using nbl::NBLog;

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
            std::cout << "RESET LOC ON " << i << std::endl;
            lastReset[i] = resetInput[i].message().timestamp();

            std::cout << "resetInput: " << resetInput[i].message().x() << ", " << resetInput[i].message().x() << ", " << resetInput[i].message().x() << std::endl;

            // See PenaltyStates.py in the manualPlacement state
            if ((int)(resetInput[i].message().x()) == 999) // &&
                // (int)(resetInput[i].message().y()) == 999 &&
                // (int)(resetInput[i].message().h()) == 999)
            {
                std::cout << "Resetting Loc to Manual Placement Values" << std::endl;
                std::vector<particleLocationStruct> particleVector;

                // Coordinates include green padding around field
                particleVector.push_back(particleLocationStruct());
                particleVector.at(0).x = 160;
                particleVector.at(0).y = 160;
                particleVector.at(0).h = 0;

                particleVector.push_back(particleLocationStruct());
                particleVector.at(1).x = 160;
                particleVector.at(1).y = 320;
                particleVector.at(1).h = 0;

                particleVector.push_back(particleLocationStruct());
                particleVector.at(2).x = 160;
                particleVector.at(2).y = 420;
                particleVector.at(2).h = 0;

                particleVector.push_back(particleLocationStruct());
                particleVector.at(3).x = 160;
                particleVector.at(3).y = 600;
                particleVector.at(3).h = 0;

                particleVector.push_back(particleLocationStruct());
                particleVector.at(4).x = 420;
                particleVector.at(4).y = 360;
                particleVector.at(4).h = 0;

                std::cout << "Made my vector of structs, calling resetLocToMany" << std::endl;

                particleFilter->resetLocToMany(particleVector);
            } else {
                std::cout << "No special values, resetting loc to the one point" << std::endl;
                particleFilter->resetLocTo(resetInput[i].message().x(),
                                           resetInput[i].message().y(),
                                           resetInput[i].message().h());
            }
            break;
        }
    }
#endif

    // Save odometry and lines
    curOdometry.set_x(motionInput.message().x());
    curOdometry.set_y(motionInput.message().y());
    curOdometry.set_h(motionInput.message().h());
    curVision = visionInput.message();

    curBall = ballInput.message();

    // TODO: use CC
    messages::CenterCircle curCircle = curVision.circle();

    const messages::FilteredBall* ball = NULL;
#ifndef OFFLINE
    bool inSet = (STATE_SET == gameStateInput.message().state());
    if (inSet)
        ball = &curBall;
#endif

    // Update filter
    particleFilter->update(curOdometry, curVision, ball);

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
    if(control::check(control::flags::locswarm)) {
        ++log_index;
        std::string log_from = "loc";

        portals::Message<messages::ParticleSwarm> swarmMessage(&particleFilter->
                                                           getCurrentSwarm());
        particleOutput.setMessage(swarmMessage);

        messages::RobotLocation rl = *output.getMessage(true).get();
        messages::ParticleSwarm ps = *particleOutput.getMessage(true).get();
        messages::Vision vm = curVision;

        nbl::logptr theLog = nbl::Log::emptyLog();
        theLog->logClass = "locswarm";
        theLog->createdWhen = time(NULL);

        theLog->addBlockFromProtobuf(rl, "locswarm", 0, 0);
        theLog->addBlockFromProtobuf(ps, "locswarm", 0, 0);
        theLog->addBlockFromProtobuf(vm, "locswarm", 0, 0);

        nbl::NBLog(theLog);

//        std::string rl_buf;
//        std::string ps_buf;
//        std::string vm_buf;
//        std::string log_buf;
//
//        rl.SerializeToString(&rl_buf);
//        ps.SerializeToString(&ps_buf);
//        vm.SerializeToString(&vm_buf);
//
//        log_buf.append(rl_buf);
//        log_buf.append(ps_buf);
//        log_buf.append(vm_buf);
//
//        std::vector<SExpr> contents;
//
//        SExpr naoLocation("location", log_from, clock(), log_index, rl_buf.length());
//        contents.push_back(naoLocation);
//
//        SExpr naoSwarm("swarm",log_from,clock(),log_index,ps_buf.length());
//        contents.push_back(naoSwarm);
//
//        SExpr naoVision("vision",log_from,clock(),log_index,vm_buf.length());
//        contents.push_back(naoVision);
//
//        NBLog(NBL_SMALL_BUFFER,"LOCSWARM",contents,log_buf);
    }
#endif

}

void LocalizationModule::run_()
{
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
    PROF_EXIT(P_SELF_LOC);
}

} // namespace localization
} // namespace man
