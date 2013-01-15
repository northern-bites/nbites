#include "NaoManLoader.h"

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
using namespace AL;

using boost::shared_ptr;
using namespace man::corpus;
using namespace man::memory;

START_FUNCTION_EXPORT

//This is what Aldebaran will call when it loads this module
//Note: this is the point of entry for our code
int _createModule(boost::shared_ptr<ALBroker> pBroker) {
    ALModule::createModule<NaoManLoader>(pBroker, "NaoManLoader");
    return 0;
}

//Aldebaran apparently never calls this - Octavian
int _closeModule() {
    return 0;
}

END_FUNCTION_EXPORT

NaoManLoader::NaoManLoader(boost::shared_ptr<AL::ALBroker> pBroker,
                                 const std::string& pName) :
                ALModule(pBroker, pName), broker(pBroker) {

    this->setModuleDescription("A module that kicks ass.");
    cout << "Creating the man loader!" << endl;

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

    man = boost::shared_ptr<TMan>(new TMan(memory, sensors, transcriber,
                                            imageTranscriber,
                                            enactor, lights, speech));
    man->startSubThreads();
}

NaoManLoader::~NaoManLoader() {
    cout << "Destroying the man loader!" << endl;
    man->stopSubThreads();
}