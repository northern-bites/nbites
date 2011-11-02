#include "OfflineManLoader.h"

#include "TMan.h"

#include "Profiler.h"
#include "corpus/Sensors.h"
#include "corpus/RoboGuardian.h"
#include "corpus/offlineconnect/OfflineImageTranscriber.h"
#include "corpus/offlineconnect/OfflineTranscriber.h"
#include "corpus/offlineconnect/OfflineEnactor.h"

using namespace std;
using boost::shared_ptr;
using namespace man::corpus;

static shared_ptr<TMan> man_pointer;

START_FUNCTION_EXPORT

void loadMan(OfflineManController::ptr offlineController) {

    shared_ptr<Speech> speech(new Speech());
    shared_ptr<Sensors> sensors(new Sensors(speech));
    shared_ptr<Transcriber> transcriber(new OfflineTranscriber(sensors,
    		offlineController->getOfflineMemory()->getMVisionSensors(),
    		offlineController->getOfflineMemory()->getMMotionSensors()));
    shared_ptr<ThreadedImageTranscriber>
        imageTranscriber(new OfflineImageTranscriber(sensors,
        	offlineController->getOfflineMemory()->getMImage()));
    shared_ptr<MotionEnactor>
        enactor(new OfflineEnactor());
    shared_ptr<Lights> lights(new Lights());

    man_pointer = shared_ptr<TMan>(new TMan(sensors, transcriber,
                                            imageTranscriber,
                                            enactor, lights, speech));
    offlineController->setImageTranscriber(imageTranscriber);
    offlineController->setManMemory(man_pointer->memory);
    man_pointer->startSubThreads();
}

void unloadMan(OfflineManController::ptr offlineController) {
    man_pointer->stopSubThreads();
    offlineController->resetImageTranscriber();
    offlineController->resetManMemory();
    //decrements the man_pointer count by one, effectively destructing man
    man_pointer.reset();
}

END_FUNCTION_EXPORT
