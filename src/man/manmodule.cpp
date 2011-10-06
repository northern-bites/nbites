#include <signal.h> //for kill
#include <sys/types.h> //for pid_t
#include "manmodule.h"

//NBites includes

#include "TMan.h"
#include "NaoEnactor.h"

typedef NaoEnactor EnactorT;
typedef TMan ALMan;

#include "ALTranscriber.h"
#include "ALImageTranscriber.h"

#include "NaoLights.h"
#include "ALSpeech.h"

using namespace std;
using namespace AL;
using boost::shared_ptr;

static shared_ptr<ALMan> man_pointer;

#ifdef __cplusplus
extern "C" //required so that the compiler doesn't mangle the name
{
#endif

class NaoManLoader: public AL::ALModule {
public:

    NaoManLoader(AL::ALPtr<AL::ALBroker> pBroker, const std::string& pName) :
        ALModule(pBroker, pName) {

        this->setModuleDescription("A module that kicks ass.");

        static shared_ptr<Synchro> synchro(new Synchro());
#ifdef USE_ALSPEECH
        static shared_ptr<Speech> speech(new ALSpeech(pBroker));
#else
        static shared_ptr<Speech> speech(new Speech());
#endif
        static shared_ptr<Sensors> sensors(new Sensors(speech));

        static shared_ptr<ALTranscriber>
                transcriber(new ALTranscriber(pBroker, sensors));

        static shared_ptr<ALImageTranscriber>
                imageTranscriber(new ALImageTranscriber(synchro, sensors,
                                                        pBroker));
        static shared_ptr<Profiler>
                profiler(new Profiler(&thread_micro_time, &process_micro_time,
                                      &monotonic_micro_time));
        static shared_ptr<EnactorT> enactor(new EnactorT(sensors, transcriber,
                                                         pBroker));

        static shared_ptr<Lights> lights(new NaoLights(pBroker));

        //setLedsProxy(AL::ALPtr<AL::ALLedsProxy>(new AL::ALLedsProxy(broker)));

        man_pointer = boost::shared_ptr<ALMan>(new ALMan(profiler, sensors,
                                                         transcriber,
                                                         imageTranscriber,
                                                         enactor, synchro,
                                                         lights, speech));
        man_pointer->startSubThreads();
    }

    virtual ~NaoManLoader() {
        cout << "Destroying the man loader" << endl;
        man_pointer->stopSubThreads();
    }

};

int _createModule(ALPtr<ALBroker> pBroker) {
    AL::ALModule::createModule<NaoManLoader>(pBroker, "NaoManLoader");
    return 0;
}

//Aldebaran apparently never calls this - Octavian
int _closeModule() {
    return 0;
}

# ifdef __cplusplus
}
# endif

