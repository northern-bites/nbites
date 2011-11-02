
#include "OfflineManPreloader.h"

#include <iostream> //cout
#include <assert.h>

using namespace std;
using namespace man::corpus;

OfflineManPreloader::OfflineManPreloader(OfflineManController::ptr offlineControl) :
                offlineControl(offlineControl) {
    createMan();
}

OfflineManPreloader::~OfflineManPreloader() {
    debug_preloader_out << "Destroying the man preloader" << endl;
    destroyMan();
}

void OfflineManPreloader::createMan() {
    importLibMan();
    linkManLoaderMethods();
    launchMan();
}

void OfflineManPreloader::reloadMan() {
    debug_preloader_out << "Trying to reload man ... " << endl;
    destroyMan();
    createMan();
}

void OfflineManPreloader::destroyMan() {
    debug_preloader_out << "Trying to destroy man ... " << endl;
    (*unloadMan)();
    this->closeLibMan();
}

void OfflineManPreloader::launchMan() {
    debug_preloader_out << "Launching man loader ... " << endl;
    assert(loadMan != NULL);
    (*loadMan)(offlineControl);
}

