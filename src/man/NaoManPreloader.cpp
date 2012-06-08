
#include "NaoManPreloader.h"

#include <iostream> //cout
#include <assert.h>

using namespace std;
using namespace AL;

START_FUNCTION_EXPORT

//This is what Aldebaran will call when it loads this module
//Note: this is the point of entry for our code
int _createModule(boost::shared_ptr<ALBroker> pBroker) {
    ALModule::createModule<NaoManPreloader>(pBroker, "NaoManPreloader");
    return 0;
}

//Aldebaran apparently never calls this - Octavian
int _closeModule() {
    return 0;
}

END_FUNCTION_EXPORT

NaoManPreloader::NaoManPreloader(boost::shared_ptr<AL::ALBroker> pBroker,
                                 const std::string& pName) :
                ALModule(pBroker, pName), broker(pBroker) {

    this->setModuleDescription("A module that kicks ass.");
    createMan();
}

NaoManPreloader::~NaoManPreloader() {
    debug_preloader_out << "Destroying the man preloader" << endl;
    destroyMan();
}

void NaoManPreloader::createMan() {
    importLibMan();
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
    this->closeLibMan();
}

void NaoManPreloader::launchMan() {
    debug_preloader_out << "Launching man loader ... " << endl;
    assert(loadMan != NULL);
    (*loadMan)(broker);
}

