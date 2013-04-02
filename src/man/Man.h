#pragma once

#include <alcommon/almodule.h>
#include <alcommon/albroker.h>
#include <boost/shared_ptr.hpp>

#include "RoboGrams.h"
#include "DiagramThread.h"
#include "DebugConfig.h"
#include "sensors/SensorsModule.h"
#include "comm/CommModule.h"
#include "vision/VisionModule.h"
#include "image/ImageTranscriberModule.h"
#include "guardian/GuardianModule.h"
#include "audio/AudioEnactorModule.h"
#include "led/LedEnactorModule.h"
#include "balltrack/BallTrackModule.h"
#include "behaviors/BehaviorsModule.h"
#include "jointenactor/JointEnactorModule.h"
#include "newmotion/MotionModule.h"
#include "gamestate/GameStateModule.h"

#include <vector>

>>>>>>> wils/overhaul

namespace man {

class Man : public AL::ALModule
{
public:
    Man(boost::shared_ptr<AL::ALBroker> broker, const std::string &name);
    virtual ~Man();


private:
    void startSubThreads();
    void startAndCheckThread(DiagramThread& thread);

    DiagramThread sensorsThread;
    sensors::SensorsModule sensors;
    jointenactor::JointEnactorModule jointEnactor;
    motion::MotionModule motion;

    DiagramThread guardianThread;
    guardian::GuardianModule guardian;
    audio::AudioEnactorModule audio;

    DiagramThread commThread;
    comm::CommModule comm;

    DiagramThread cognitionThread;
    image::ImageTranscriberModule imageTranscriber;
    vision::VisionModule vision;
    balltrack::BallTrackModule ballTrack;
    led::LedEnactorModule leds;
    behaviors::BehaviorsModule behaviors;
    gamestate::GameStateModule gamestate;
};

}
