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

#include "ManPreloaderInterface.h"
#include "NullStream.h"

class NaoManPreloader;

#include "Profiler.h"
#include "ALSpeech.h"
#include "Sensors.h"
#include "RoboGuardian.h"
#include "ALTranscriber.h"
#include "ALImageTranscriber.h"
#include "NaoEnactor.h"
#include "NaoLights.h"

START_FUNCTION_EXPORT

//This is what Aldebaran will call when it loads this module
//Note: this is the point of entry for our code
int _createModule(AL::ALPtr<AL::ALBroker> pBroker);

//Aldebaran apparently never calls this - Octavian
int _closeModule();

END_FUNCTION_EXPORT

#define DEBUG_PRELOADER
#ifdef DEBUG_PRELOADER
#define debug_preloader_out cout
#else
#define debug_preloader_out (*NullStream::NullInstance())
#endif

class NaoManPreloader: public AL::ALModule, public ManPreloaderInterface {

public:

    NaoManPreloader(AL::ALPtr<AL::ALBroker> pBroker, const std::string& pName);
    virtual ~NaoManPreloader();

private:
    void reloadMan();
    void createMan();
    void destroyMan();

protected:

    //imports the man library
    void importMan();
    //links the functions that are used to create man
    void linkManLoaderMethods();
    //call the loadMan function from the man library
    void launchMan();
    //manually links to a method exported in man
    template <class T> T linkToManMethod(std::string name);

private:
    AL::ALPtr<AL::ALBroker> broker;
    //man link variables
    void* libman_handle;
    loadManMethod loadMan;
    unloadManMethod unloadMan;

    boost::shared_ptr<Profiler> profiler;
    boost::shared_ptr<Sensors> sensors;
    boost::shared_ptr<RoboGuardian> guardian;
    boost::shared_ptr<ALTranscriber> transcriber;
    boost::shared_ptr<ALImageTranscriber> imageTranscriber;
    boost::shared_ptr<NaoEnactor> enactor;
    boost::shared_ptr<NaoLights> lights;
    boost::shared_ptr<Speech> speech;
};
