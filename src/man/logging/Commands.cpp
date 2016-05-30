//
//  Commands.cpp
//  tool8-separate
//
//  Created by Philip Koch on 4/14/16.
//

#include "Control.hpp"
#include "LogRPC.hpp"

using nbl::SharedConstants;

namespace control {
    //helper functions may be defined here

    RPC_MAKE_FUNCTION_GROUP(Control)

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

        nbl::logptr flagLog = nbl::Log::explicitLog({}, {}, nbl::SharedConstants::LogClass_Flags());

        flagLog->addBlockFromJson(flags, "createFlagStateLog()", nbl::IMAGE_INDEX_INVALID, clock());

        return flagLog;
    }

#define CONTROL_FUNCTION(name, va, ...) \
    RPC_FUNCTION_DECLARE_COMPLETE(name, va, ## __VA_ARGS__) \
    RPC_FUNCTION_STATIC_ADD(name, RPC_FUNCTION_INSTANTIATE(name), Control)\
    RPC_FUNCTION_DEFINE(name)

    CONTROL_FUNCTION(SetFlag, false, SharedConstants::LogClass_Null())
        (const std::vector<nbl::logptr> &arguments) {
            NBL_PRINT("RPCFunctionSetFlag::call()");
            uint8_t index = arguments[0]->blocks[0].data[0];
            uint8_t value = arguments[0]->blocks[0].data[1];
            NBL_PRINT("RPCFunctionSetFlag: %s -> %d",
                      stringFor((flags::flag_e) index), value);

            set( (flags::flag_e) index, value);
            RETURN(createFlagStateLog());
        }

    CONTROL_FUNCTION(GetFlags, false)
        (const std::vector<nbl::logptr> &arguments) {
            NBL_PRINT("RPCFunctionGetFlag::call()");
            RETURN(createFlagStateLog());
        }

}





