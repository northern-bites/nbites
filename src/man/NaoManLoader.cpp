#include <dlfcn.h>
#include <iostream>

#include "NaoManLoader.h"

#include "TMan.h"

#include "Profiler.h"
#include "ALSpeech.h"
#include "Sensors.h"
#include "RoboGuardian.h"
#include "ALTranscriber.h"
#include "ALImageTranscriber.h"
#include "NaoEnactor.h"
#include "NaoLights.h"

using namespace std;
using boost::shared_ptr;

static shared_ptr<TMan> man_pointer;

START_FUNCTION_EXPORT

void loadMan(AL::ALPtr<AL::ALBroker> broker) {

    shared_ptr<Speech> speech(new ALSpeech(broker));
    shared_ptr<Sensors> sensors(new Sensors(speech));
    shared_ptr<Transcriber> transcriber(new ALTranscriber(broker, sensors));
    shared_ptr<ThreadedImageTranscriber>
        imageTranscriber(new ALImageTranscriber(sensors, broker));
    shared_ptr<MotionEnactor>
        enactor(new NaoEnactor(sensors, transcriber, broker));
    shared_ptr<Lights> lights(new NaoLights(broker));

    man_pointer = shared_ptr<TMan>(new TMan(sensors, transcriber,
                                            imageTranscriber,
                                            enactor, lights, speech));
    man_pointer->startSubThreads();
}

void unloadMan() {
    man_pointer->stopSubThreads();
    //decrements the man_pointer count by one, effectively destructing man
    man_pointer.reset();
}

END_FUNCTION_EXPORT
