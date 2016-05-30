//
//  Control.cpp
//  tool8-separate
//
//  Created by Philip Koch on 4/14/16.
//

#include "Control.hpp"
#include "LogRPC.hpp"

using namespace nbl::rpc;

namespace control {

    RPC_MAKE_FUNCTION_GROUP_VISIBLE(Control)

    ControlHandler::ControlHandler() {
        mapfromFunctions(map, getControlVector());

        for (int i = flags::_START_EXTERNAL_; i < flags::_END_EXTERNAL_; ++i) {
            set((flags::flag_e) i, false);
        }

        set(flags::logToStream, true);
    }

    void ControlHandler::consumeLog(nbl::logptr ptrTo) {

        RPCFunctionBase * fptr = functionFromMap(map, ptrTo);
        if (!IS_PTR_VALID(fptr)) {
            NBL_ERROR("ControlHandler::consumeLog() cannot use log: %s",
                      ptrTo->logClass.c_str()
                      );

            return;
        }

        NBL_PRINT("calling %s", fptr->getName().c_str());
        rpc_return ret = fptr->call(ptrTo);

        if (!ret.argumentsValid) {
            NBL_ERROR("ControlHandler::consumeLog() call failed: %s",
                      fptr->getName().c_str());
            return;
        }

        NBL_ASSERT(IS_PTR_VALID(ret.returns));
        nbl::NBLog(ret.returns, nbl::Q_CONTROL);
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
        if (f < flags::num_flags)
            return flags::string_flags[f];
        else return "INVALID FLAG INDEX";
    }

}