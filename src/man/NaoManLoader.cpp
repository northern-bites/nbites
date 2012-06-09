#include "NaoManLoader.h"

#include "TMan.h"

#include "manconfig.h"

#include "Profiler.h"
#include "corpus/Sensors.h"
#include "corpus/RoboGuardian.h"
#include "corpus/alconnect/NaoEnactor.h"
#include "corpus/alconnect/ALTranscriber.h"
#include "corpus/NaoImageTranscriber.h"
#include "corpus/alconnect/ALSpeech.h"
#include "corpus/alconnect/NaoLights.h"
#include "corpus/Camera.h"

#include "memory/RobotMemory.h"

using namespace std;
using boost::shared_ptr;
using namespace man::corpus;
using namespace man::memory;

static boost::shared_ptr<TMan> man_pointer;

START_FUNCTION_EXPORT

void loadMan(boost::shared_ptr<AL::ALBroker> broker) {

#ifdef USE_ALSPEECH
    boost::shared_ptr<Speech> speech(new ALSpeech(broker));
#else
    boost::shared_ptr<Speech> speech(new Speech());
#endif
    RobotMemory::ptr memory(new RobotMemory());
    boost::shared_ptr<Sensors> sensors(new Sensors(speech,
                                                  memory->get<MVisionSensors>(),
                                                  memory->get<MMotionSensors>()));
    boost::shared_ptr<Transcriber> transcriber(new ALTranscriber(broker, sensors));
    boost::shared_ptr<ThreadedImageTranscriber>
        imageTranscriber(new NaoImageTranscriber(sensors, "ImageTranscriber", memory->get<MRawImages>()));
    boost::shared_ptr<MotionEnactor>
        enactor(new NaoEnactor(sensors, transcriber, broker));
    boost::shared_ptr<Lights> lights(new NaoLights(broker));

    man_pointer = boost::shared_ptr<TMan>(new TMan(memory, sensors, transcriber,
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
