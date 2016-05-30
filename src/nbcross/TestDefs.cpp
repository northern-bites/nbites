//
//  TestDefs.cpp
//  tool8-separate
//
//  Created by Philip Koch on 4/18/16.
//

#include <stdio.h>
#include "nbcross.hpp"

namespace nbl {
    namespace nbcross {

        NBCROSS_FUNCTION(Test, true)
            (const std::vector<nbl::logptr> &arguments) {
            NBL_PRINT("test always!");
        }

        NBCROSS_FUNCTION(Test2, false, SharedConstants::LogClass_Null() )
            (const std::vector<nbl::logptr> &arguments) {
            NBL_PRINT("test 2 got %z args.",
                      arguments.size());
        }
    }
}