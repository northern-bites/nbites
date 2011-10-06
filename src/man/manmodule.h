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
#include "vision/Profiler.h"

// ..::: Version Information ::
/** Major release version */
#define MANMODULE_VERSION_MAJOR    "1"

/** Minor release version */
#define MANMODULE_VERSION_MINOR    "0"

//#define ALVALUE_STRING( val ) ((val.getType() == ALValue::TypeString) ? std::string(val) : std::string("") )
//#define ALVALUE_DOUBLE( val ) ((val.getType() == ALValue::TypeDouble || val.getType() == ALValue::TypeInt) ? double(val) : 0.0 )
//#define ALVALUE_INT( val ) ((val.getType() == ALValue::TypeInt || val.getType() == ALValue::TypeDouble) ? int(val) : 0)

#endif // MANMODULE_H
