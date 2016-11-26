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

using nbl::rpc::RPCFunctionMap;

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
    class ControlHandler : public nbl::LogConsumer {
        RPCFunctionMap map;
    public:
        ControlHandler();
        void consumeLog(nbl::logptr ptrTo);
        void printFunctions();
    };

    struct RobotCommandStruct {
        bool adjustHead;
        float adjustedHeadZ,adjustedHeadY;

        bool walkCommand, walkStop;
        float walkHeading,walkX,walkY;

        bool doSweetMove;
        int sweetMoveID;

        bool logInfo,logImage;
    };
    
    extern struct RobotCommandStruct latestCommand;
}



#endif /* Control_hpp */
