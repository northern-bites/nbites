//
//  Commands.cpp
//  tool8-separate
//
//  Created by Philip Koch on 4/14/16.
//  Copyright © 2016 pkoch. All rights reserved.
//

#include "Control.hpp"
#include "LogRPC.hpp"

using nbl::CONSTANTS;

namespace control {
    //helper functions may be defined here

#define CONTROL_FUNCTION_SET                                    \
    CONTROL_FUNCTION(SetFlag, false, CONSTANTS.LogClass_Null()) \
    CONTROL_FUNCTION(GetFlags, false)                            \

#define CONTROL_FUNCTION(name, va, ...) \
    RPC_FUNCTION_DECLARE_COMPLETE(name, va, ## __VA_ARGS__)

    CONTROL_FUNCTION_SET

    nbl::logptr createFlagStateLog() {
        json::Array flags;
        for (int i = flags::_START_EXTERNAL_ + 1; i < flags::_END_EXTERNAL_; ++i) {
            flags::flag_e f = (flags::flag_e) i;
            json::Object attrs;
            attrs["index"] = json::Number(f);
            attrs["value"] = json::Boolean(check(f));
            attrs["name"] = json::String(stringFor(f));

            flags.push_back(attrs);
        }

        nbl::logptr flagLog = nbl::Log::explicitLog({}, {}, nbl::CONSTANTS.LogClass_Flags());

        flagLog->addBlockFromJson(flags, "createFlagStateLog()", nbl::IMAGE_INDEX_INVALID, clock());

        return flagLog;
    }
//
//COMMAND_LIST_START
//
//    COMMAND_START(setFlag)
//    void execute(nbl::logptr arg) {
//        uint8_t index = arg->blocks[0].data[0];
//        uint8_t value = arg->blocks[0].data[1];
//
//        set( (flags::flag_e) index, value);
//    }
//    COMMAND_END(setFlag)
//
//    COMMAND_START(getFlags)
//    void execute(nbl::logptr arg) {
//        returnLog(createFlagStateLog());
//    }
//    COMMAND_END(getFlags)
//
//
//COMMAND_LIST_END

    void RPCFunctionGetFlags::defineCall(const std::vector<nbl::logptr> &arguments) {
        NBL_PRINT("RPCFunctionGetFlag::call()");
        RETURN(createFlagStateLog());
    }

    void RPCFunctionSetFlag::defineCall(const std::vector<nbl::logptr> &arguments) {

        uint8_t index = arguments[0]->blocks[0].data[0];
        uint8_t value = arguments[0]->blocks[0].data[1];

        set( (flags::flag_e) index, value);
    }

#undef CONTROL_FUNCTION
#define CONTROL_FUNCTION(name, va, ...) \
    RPC_FUNCTION_INSTANTIATE(name)  ,

    std::vector<nbl::rpc::RPCFunctionPtr> setupControlFunctions() {
        return {
            CONTROL_FUNCTION_SET
        };
    }


}