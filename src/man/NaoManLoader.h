/**
 * Naoqi module to create a static instance of man
 * Useful when loading man as a dynamic library
 *
 * @author The Northern Bites - Bowdoin College, Brunswick, ME
 *
**/

#ifndef MANMODULE_H
#define MANMODULE_H

#include <boost/shared_ptr.hpp>

#include "include/ExportDefines.h"

#include "TMan.h"

//This is the method loadMan's signature
typedef void (*loadManMethod)(
        boost::shared_ptr<Profiler> profiler,
        boost::shared_ptr<Sensors> sensors,
        boost::shared_ptr<RoboGuardian> guardian,
        boost::shared_ptr<Transcriber> transcriber,
        boost::shared_ptr<ThreadedImageTranscriber> imageTranscriber,
        boost::shared_ptr<MotionEnactor> enactor,
        boost::shared_ptr<Lights> lights,
        boost::shared_ptr<Speech> speech);

//This is the method unloadMan's signature
typedef void (*unloadManMethod)();

START_FUNCTION_EXPORT

void loadMan(boost::shared_ptr<Profiler> profiler,
             boost::shared_ptr<Sensors> sensors,
             boost::shared_ptr<RoboGuardian> guardian,
             boost::shared_ptr<Transcriber> transcriber,
             boost::shared_ptr<ThreadedImageTranscriber> imageTranscriber,
             boost::shared_ptr<MotionEnactor> enactor,
             boost::shared_ptr<Lights> lights,
             boost::shared_ptr<Speech> speech);

void unloadMan();

END_FUNCTION_EXPORT

#endif // MANMODULE_H
