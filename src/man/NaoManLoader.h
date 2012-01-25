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
#include <alcommon/albroker.h>

#include "include/ExportDefines.h"

//This is the method loadMan's signature
typedef void (*loadManMethod)(AL::ALPtr<AL::ALBroker> pBroker);

//This is the method unloadMan's signature
typedef void (*unloadManMethod)();

START_FUNCTION_EXPORT

void loadMan(AL::ALPtr<AL::ALBroker> pBroker);

void unloadMan();

END_FUNCTION_EXPORT

#endif // MANMODULE_H
