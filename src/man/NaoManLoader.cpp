#include <signal.h> //for kill
#include <sys/types.h> //for pid_t
#include <dlfcn.h>
#include "NaoManLoader.h"

//NBites includes

#include "TMan.h"
#include "NaoEnactor.h"

typedef NaoEnactor EnactorT;
typedef TMan ALMan;

#include "ALTranscriber.h"
#include "ALImageTranscriber.h"

#include "NaoLights.h"
#include "ALSpeech.h"

#include "vision/Profiler.h"
#include "include/ExportDefines.h"

using namespace std;
using namespace AL;
using boost::shared_ptr;

static shared_ptr<ALMan> man_pointer;

START_FUNCTION_EXPORT

void loadMan(ALPtr<ALBroker> broker) {
    shared_ptr<Synchro> synchro(new Synchro());
#ifdef USE_ALSPEECH
    shared_ptr<Speech> speech(new ALSpeech(broker));
#else
    shared_ptr<Speech> speech(new Speech());
#endif
    shared_ptr<Sensors> sensors(new Sensors(speech));

    shared_ptr<ALTranscriber>
    transcriber(new ALTranscriber(broker, sensors));

    shared_ptr<ALImageTranscriber>
        imageTranscriber(new ALImageTranscriber(synchro, sensors,
                                            broker));
    shared_ptr<Profiler>
        profiler(new Profiler(&thread_micro_time, &process_micro_time,
                          &monotonic_micro_time));
    cout << " I'm betting we get this far" << endl;
    shared_ptr<EnactorT> enactor(new EnactorT(sensors, transcriber,
                                                     broker));

    shared_ptr<Lights> lights(new NaoLights(broker));

    //setLedsProxy(AL::ALPtr<AL::ALLedsProxy>(new AL::ALLedsProxy(broker)));

    man_pointer = boost::shared_ptr<ALMan>(new ALMan(profiler, sensors,
                                                     transcriber,
                                                     imageTranscriber,
                                                     enactor, synchro,
                                                     lights, speech));
    man_pointer->startSubThreads();
}

END_FUNCTION_EXPORT
