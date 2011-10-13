
#include "NaoManPreloader.h"

#include <dlfcn.h> //dlopen, dlsym
#include <stdlib.h> //exit
#include <iostream> //cout
#include <assert.h>

using namespace std;
using namespace AL;

START_FUNCTION_EXPORT

//This is what Aldebaran will call when it loads this module
//Note: this is the point of entry for our code
int _createModule(ALPtr<ALBroker> pBroker) {
    ALModule::createModule<NaoManPreloader>(pBroker, "NaoManPreloader");
    return 0;
}

//Aldebaran apparently never calls this - Octavian
int _closeModule() {
    return 0;
}

END_FUNCTION_EXPORT

static string LIBMAN_NAME="libman.so";
static string LOAD_MAN_METHOD_NAME="loadMan";
static string UNLOAD_MAN_METHOD_NAME="unloadMan";

NaoManPreloader::NaoManPreloader(AL::ALPtr<AL::ALBroker> pBroker,
                                 const std::string& pName) :
                ALModule(pBroker, pName), broker(pBroker),
                speech(new Speech()),
                sensors(new Sensors(speech)),
                guardian(new RoboGuardian(sensors, this)){

    this->setModuleDescription("A module that kicks ass.");
    guardian->start();
    createMan();
}

NaoManPreloader::~NaoManPreloader() {
    //TODO: this should call the man destructor in some way
    debug_preloader_out << "Destroying the man preloader" << endl;
    guardian->stop();
    guardian->waitForThreadToFinish();
}

void NaoManPreloader::createMan() {
    importMan();
    linkManLoaderMethods();
    launchMan();
}

void NaoManPreloader::reloadMan() {
    debug_preloader_out << "Trying to reload man ... " << endl;
    destroyMan();
    createMan();
}

void NaoManPreloader::destroyMan() {
    debug_preloader_out << "Trying to destroy man ... " << endl;
    (*unloadMan)();
    if (dlclose(libman_handle) != 0) {
        cout << dlerror() << endl;
        std::exit(1);
    }
}

void NaoManPreloader::importMan() {
    debug_preloader_out << "Importing " + LIBMAN_NAME + " ... ";
    libman_handle = dlopen(LIBMAN_NAME.c_str(),
                           RTLD_LAZY);
    if (!libman_handle)
    {
        cout << dlerror() << endl;
        std::exit(1);
    }
    debug_preloader_out << "done"<<endl;
}

void NaoManPreloader::linkManLoaderMethods() {

    loadMan = linkToManMethod<loadManMethod>(LOAD_MAN_METHOD_NAME);
    unloadMan = linkToManMethod<unloadManMethod>(UNLOAD_MAN_METHOD_NAME);
}

template <class T>
T NaoManPreloader::linkToManMethod(std::string name) {
    debug_preloader_out << "Manually linking to " + name + " ... ";
    T method = reinterpret_cast<T>(
            dlsym(libman_handle, name.c_str()));
    if (method == NULL)
    {
        cout << dlerror() << endl;
        std::exit(1);
    }
    return method;
}

void NaoManPreloader::launchMan() {
    debug_preloader_out << "Launching man loader ... " << endl;
    assert(loadMan != NULL);
    (*loadMan)(broker, speech, sensors, guardian);
}

