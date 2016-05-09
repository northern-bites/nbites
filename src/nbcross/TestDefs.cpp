//
//  TestDefs.cpp
//  tool8-separate
//
//  Created by Philip Koch on 4/18/16.
//  Copyright © 2016 pkoch. All rights reserved.
//

#include <stdio.h>
#include "nbcross.hpp"

namespace nbl {
    namespace nbcross {

        void RPCFunctionTest::defineCall(const std::vector<nbl::logptr> &arguments) {
            NBL_PRINT("test always!");
        }

        void RPCFunctionTest2::defineCall(const std::vector<nbl::logptr> &arguments) {
            NBL_PRINT("test 2 got %z args.",
                      arguments.size());
        }
    }
}