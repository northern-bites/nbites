/**
 * Naoqi module to act as a buffer between naoqi and the man loader
 *
 * @author The Northern Bites - Bowdoin College, Brunswick, ME
 *
**/

#pragma once

#include <alcommon/almodule.h>

#include "NaoManLoader.h" // for loadManMethod
#include "include/ExportDefines.h"

#include "ManPreloader.h"

START_FUNCTION_EXPORT

//This is what Aldebaran will call when it loads this module
//Note: this is the point of entry for our code
int _createModule(AL::ALPtr<AL::ALBroker> pBroker);

//Aldebaran apparently never calls this - Octavian
int _closeModule();

END_FUNCTION_EXPORT

class NaoManPreloader: public AL::ALModule,
                       public ManPreloader<loadManMethod, unloadManMethod> {

public:

    NaoManPreloader(AL::ALPtr<AL::ALBroker> pBroker, const std::string& pName);
    virtual ~NaoManPreloader();

private:
    void reloadMan();
    void createMan();
    void destroyMan();

protected:
    //call the loadMan function from the man library
    void launchMan();

private:
    AL::ALPtr<AL::ALBroker> broker;

};
