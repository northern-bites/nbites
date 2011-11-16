/**
 * Naoqi module to act as a buffer between qtool (or other offline tools) and
 * the man loader
 *
 * @author The Northern Bites - Bowdoin College, Brunswick, ME
 *
**/

#pragma once

#include "OfflineManLoader.h" // for loadManMethod
#include "ManPreloader.h"
#include "man/corpus/offlineconnect/OfflineManController.h"

class OfflineManPreloader: public ManPreloader<loadManMethod, unloadManMethod> {

public:

    OfflineManPreloader(man::corpus::OfflineManController::ptr offlineControl);
    virtual ~OfflineManPreloader();

    void reloadMan();
    void createMan();
    void destroyMan();

protected:
    //call the loadMan function from the man library
    void launchMan();

private:
    man::corpus::OfflineManController::ptr offlineControl;

};
