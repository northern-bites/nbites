//
//  Control.hpp
//
//  Created by Philip Koch on 4/14/16.
//

#ifndef Control_hpp
#define Control_hpp

#include <stdio.h>
#include "Logging.hpp"
#include "Log.hpp"
#include "LogRPC.hpp"

using nblog::rpc::RPCFunctionMap;
using nblog::rpc::RPCFunctionPtr;

namespace control {
    namespace flags {

#define FLAG_XM(n) n
        typedef enum {
#include "flags.defs.h"
        } flag_e;
#undef FLAG_XM
    }

    bool check(flags::flag_e f);
    bool set(flags::flag_e f, bool v);

    const char * stringFor(flags::flag_e f);

    //defined in Control.cpp
    class ControlHandler : public nblog::LogConsumer {
        RPCFunctionMap map;
    public:
        ControlHandler();
        void consumeLog(nblog::logptr ptrTo);
        void printFunctions();
    };

    extern std::vector<nblog::rpc::RPCFunctionPtr> setupControlFunctions();
}

#endif /* Control_hpp */
