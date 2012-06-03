#include "NaoManLoader.h"

#include "TMan.h"

#include "manconfig.h"

#include "Profiler.h"
#include "corpus/Sensors.h"
#include "corpus/RoboGuardian.h"
#include "corpus/alconnect/NaoEnactor.h"
#include "corpus/alconnect/ALTranscriber.h"
#include "corpus/V4L2ImageTranscriber.h"
#include "corpus/alconnect/ALSpeech.h"
#include "corpus/alconnect/NaoLights.h"

using namespace std;
using boost::shared_ptr;
using namespace man::corpus;

static shared_ptr<TMan> man_pointer;

START_FUNCTION_EXPORT

void loadMan(boost::shared_ptr<AL::ALBroker> broker) {

#ifdef USE_ALSPEECH
    shared_ptr<Speech> speech(new ALSpeech(broker));
#else
    shared_ptr<Speech> speech(new Speech());
#endif
    shared_ptr<Sensors> sensors(new Sensors(speech));
    shared_ptr<Transcriber> transcriber(new ALTranscriber(broker, sensors));
    shared_ptr<ThreadedImageTranscriber>
        imageTranscriber(new V4L2ImageTranscriber(sensors));
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
