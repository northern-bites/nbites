/**
 * Naoqi module to load man.
 *
 * @author The Northern Bites - Bowdoin College, Brunswick, ME
 *
**/

#ifndef MANMODULE_H
#define MANMODULE_H

// ..::: Headers ::
#include <fstream>
#include <sstream>

#include "alcommon/albroker.h"
#include "alcommon/albrokermanager.h"
#include "alcommon/almodule.h"
#include "alcommon/alproxy.h"
#include "alcore/alerror.h"
#include "alcore/alptr.h"
#include "alcore/altypes.h"
#include "altools/alxplatform.h"

#include "manconfig.h"
#include "include/ExportDefines.h"

typedef void (*loadManMethod)(AL::ALPtr<AL::ALBroker> broker);

START_FUNCTION_EXPORT

void loadMan(AL::ALPtr<AL::ALBroker> broker);

END_FUNCTION_EXPORT

#endif // MANMODULE_H
