//
//  Use this file to import your target's public headers that you would like to expose to Swift.
//

/* we only expose one Objective C header to swift.  We also make sure that header doesn't include and weird Robocup c++ code because swift can't handle that.  The c++ code can be used in RobotConnection.mm instead. */

#import "RobotConnection.h"
