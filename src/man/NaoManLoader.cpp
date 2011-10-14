#include <dlfcn.h>
#include <iostream>

#include "NaoManLoader.h"

using namespace std;
using boost::shared_ptr;

static shared_ptr<TMan> man_pointer;

START_FUNCTION_EXPORT

void loadMan(shared_ptr<Profiler> profiler,
             shared_ptr<Sensors> sensors,
             shared_ptr<RoboGuardian> guardian,
             shared_ptr<Transcriber> transcriber,
             shared_ptr<ThreadedImageTranscriber> imageTranscriber,
             shared_ptr<MotionEnactor> enactor,
             shared_ptr<Lights> lights,
             shared_ptr<Speech> speech) {

    man_pointer = shared_ptr<TMan>(new TMan(profiler, sensors, guardian,
                                            transcriber, imageTranscriber,
                                            enactor, lights, speech));
    man_pointer->startSubThreads();
}

void unloadMan() {
    man_pointer->stopSubThreads();
    //decrements the man_pointer count by one, effectively destructing man
    man_pointer.reset();
}

END_FUNCTION_EXPORT
