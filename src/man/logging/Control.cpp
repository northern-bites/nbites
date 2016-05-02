//
//  Control.cpp
//  tool8-separate
//
//  Created by Philip Koch on 4/14/16.
//  Copyright © 2016 pkoch. All rights reserved.
//

#include "Control.hpp"
#include "LogRPC.hpp"

using namespace nblog::rpc;

namespace control {


    ControlHandler::ControlHandler() {
        mapfromFunctions(map, setupControlFunctions());
    }

    void ControlHandler::consumeLog(nblog::logptr ptrTo) {

        RPCFunctionPtr fptr = functionFromMap(map, ptrTo);
        if (!IS_PTR_VALID(fptr)) {
            NBL_ERROR("ControlHandler::consumeLog() cannot use log: %s",
                      ptrTo->logClass.c_str()
                      );

            return;
        }

        rpc_return ret = fptr->call(ptrTo);

        if (!ret.argumentsValid) {
            NBL_ERROR("ControlHandler::consumeLog() call failed: %s",
                      fptr->getName().c_str());
            return;
        }

        NBL_ASSERT(IS_PTR_VALID(ret.returns));
        nblog::NBLog(ret.returns, nblog::Q_CONTROL);
    }

    void ControlHandler::printFunctions() {
        for (auto pair : map) {
            NBL_PRINT("ControlFunction:\t %s", pair.second->describe().c_str());
        }
    }

    namespace flags {
        //Construct string_flag array.
#define FLAG_XM(n) [n] = #n
        const char * string_flags[] = {
#include "flags.defs.h"
        };
#undef FLAG_XM
    }

    volatile bool byte_flags[flags::num_flags] = {0};

    bool check(flags::flag_e f) {
        return byte_flags[f];
    }

    bool set(flags::flag_e f, bool v) {
        if (f < flags::num_flags) {
            bool temp = byte_flags[f];
            byte_flags[f] = v;
            return temp;
        } else {
            NBL_ERROR("tried to set invalid flag: %d, must be 0<=f<=%d \n",
                      f, flags::num_flags);
            return false;
        }
    }

    const char * stringFor( flags::flag_e f) {
        return flags::string_flags[f];
    }

}