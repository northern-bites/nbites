#pragma once

#include "RoboGrams.h"
#include "DiagramThread.h"
#include "DebugConfig.h"
#include "sensors/SensorsModule.h"
#include "comm/CommModule.h"
#include "image/TranscriberModule.h"
#include "vision/VisionModule.h"
#include "guardian/GuardianModule.h"
#include "audio/AudioEnactorModule.h"
#include "balltrack/BallTrackModule.h"
#include "behaviors/BehaviorsModule.h"
#include "jointenactor/JointEnactorModule.h"
#include "motion/MotionModule.h"
#include "gamestate/GameStateModule.h"
#include "localization/LocalizationModule.h"
#include "sharedball/SharedBall.h"
#include "arms/ArmContactModule.h"
#include "obstacle/ObstacleModule.h"
#include "ParamReader.h"

#include <vector>

namespace man {

class Man
{
public:
    Man();
    virtual ~Man();
    // Let us close camera driver before destroying
    void preClose();

private:
    void startSubThreads();
    void startAndCheckThread(DiagramThread& thread);

    ParamReader param;
    int playerNum, teamNum;
    std::string robotName;
    DiagramThread sensorsThread;
    sensors::SensorsModule sensors;
    jointenactor::JointEnactorModule jointEnactor;
    motion::MotionModule motion;
    arms::ArmContactModule arms;

    DiagramThread guardianThread;
    guardian::GuardianModule guardian;
    audio::AudioEnactorModule audio;

    DiagramThread commThread;
    comm::CommModule comm;

    DiagramThread cognitionThread;
    image::TranscriberModule topTranscriber;
    image::TranscriberModule bottomTranscriber;
    vision::VisionModule vision;
    localization::LocalizationModule localization;
    balltrack::BallTrackModule ballTrack;
    obstacle::ObstacleModule obstacle;
    gamestate::GameStateModule gamestate;
    behaviors::BehaviorsModule behaviors;
    context::SharedBallModule sharedBall;
};

}
