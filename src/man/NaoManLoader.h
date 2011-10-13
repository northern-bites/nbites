/**
 * Naoqi module to load man.
 *
 * @author The Northern Bites - Bowdoin College, Brunswick, ME
 *
**/

#ifndef MANMODULE_H
#define MANMODULE_H

#include <boost/shared_ptr.hpp>

#include <alcommon/albroker.h>
#include <alcommon/alproxy.h>
#include <alcore/alptr.h>

#include "manconfig.h"
#include "include/ExportDefines.h"

#include "ALSpeech.h"
#include "Sensors.h"

typedef void (*loadManMethod)(AL::ALPtr<AL::ALBroker> broker,
                              boost::shared_ptr<Speech> speech,
                              boost::shared_ptr<Sensors> sensors);

START_FUNCTION_EXPORT

void loadMan(AL::ALPtr<AL::ALBroker> broker, boost::shared_ptr<Speech> speech,
             boost::shared_ptr<Sensors> sensors);

END_FUNCTION_EXPORT

#endif // MANMODULE_H
