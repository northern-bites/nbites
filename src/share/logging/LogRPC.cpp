//
//  LogRPC.cpp
//  tool8-separate
//
//  Created by Philip Koch on 4/16/16.
//  Copyright © 2016 pkoch. All rights reserved.
//

#include "LogRPC.hpp"

#define NBL_LOGGING_LEVEL NBL_INFO_LEVEL

using nbl::CONSTANTS;

namespace nbl {
    namespace rpc {
        RPCFunctionBase::RPCFunctionBase(const std::string& name, bool ava, std::initializer_list<const std::string> rac) :
            functionName(name), allowsVariableArguments(ava), reqArgClasses(rac) {}

        const std::string& RPCFunctionBase::getName() {return functionName;}

        const std::string RPCFunctionBase::describe() {
            std::ostringstream oss;
            oss << utilities::format("RPCFunction%s: [name=%s] (",
                                     getName().c_str() ,
                                     getName().c_str() );

            for (int i = 0; i < reqArgClasses.size(); ++i) {
//                NBL_WHATIS(oss.str().c_str());

                if (i != 0) oss << ", ";
                
                oss << reqArgClasses[i];
            }

            if (allowsVariableArguments) {
                if (reqArgClasses.empty()) {
                    oss << "...";
                } else {
                    oss << ", ...";
                }
            }

            oss << ")";

            return oss.str();
        }

        const rpc_return RPC_CALL_FAILED = {false, NULL};

        rpc_return RPCFunctionBase::call(logptr callingLog) {

            if ( callingLog->logClass != CONSTANTS.LogClass_RPC_Call() ) {
                NBL_ERROR("RPCFunctionBase::call() cannot use non- RPC_Call log: '%s'",
                          callingLog->logClass.c_str());
                return RPC_CALL_FAILED;
            }

            const std::string name = callingLog->toplevelDictionary[CONSTANTS.RPC_NAME()].asString();
            const std::string key = callingLog->toplevelDictionary[CONSTANTS.RPC_KEY()].asString();

            if (name != functionName) {
                NBL_ERROR("RPCFunctionBase::call() [name=%s] cannot use RPC_Call of different function: %s" ,
                          functionName.c_str(),
                          name.c_str() );
                return RPC_CALL_FAILED;
            }

            std::vector<logptr> arguments;
            for (Block b : callingLog->blocks) {
                arguments.push_back(b.parseAsLog());
            }

            if (arguments.size() < reqArgClasses.size() ) {
                NBL_ERROR("RPCFunctionBase::call() [function=%s] %d arguments given to function that requires at least %d",
                          functionName.c_str(),
                          arguments.size(), reqArgClasses.size());

                return RPC_CALL_FAILED;
            }

            if (arguments.size() > reqArgClasses.size() && !allowsVariableArguments) {
                NBL_ERROR("RPCFunctionBase::call() [function=%s] too many arguments supplied to non-variable argument function",
                          functionName.c_str());

                return RPC_CALL_FAILED;
            }

            for (int i = 0; i < arguments.size(); ++i) {
                if (!IS_PTR_VALID(arguments[i])) {
                    NBL_ERROR("RPCFunctionBase::call() [function=%s] given NULL argument",
                              functionName.c_str());

                    return RPC_CALL_FAILED;
                }
            }

            for (int i = 0; i < reqArgClasses.size(); ++i) {
                if ( !(reqArgClasses[i] == arguments[i]->logClass) ) {
                    NBL_ERROR("RPCFunctionBase::call() [function=%s] argument %d '%s' does not match '%s'",
                              functionName.c_str(),
                              i, reqArgClasses[i].c_str(),
                              arguments[i]->logClass.c_str());

                    return RPC_CALL_FAILED;
                }
            }

            returnVector.clear();

            defineCall(arguments);

            logptr retLog = Log::explicitLog({}, {}, CONSTANTS.LogClass_RPC_Return() );

            for (logptr p : returnVector) {
                retLog->addBlockFromLog(*p);
            }

            retLog->toplevelDictionary[CONSTANTS.RPC_NAME()] = json::String(functionName);
            retLog->toplevelDictionary[CONSTANTS.RPC_KEY()] = json::String(key);

            returnVector.clear();
            return {true, retLog};
        }

        void RPCFunctionBase::RETURN(logptr log) {
            NBL_ASSERT(IS_PTR_VALID(log))
            returnVector.push_back(log);
        }

        void mapfromFunctions(RPCFunctionMap& map, std::vector<RPCFunctionPtr> functions) {
            for (RPCFunctionPtr fp : functions) {
                map[fp->getName()] = fp;
            }
        }

        RPCFunctionPtr functionFromMap(RPCFunctionMap& map, logptr callLog) {
            if (callLog->logClass == nbl::CONSTANTS.LogClass_RPC_Call()) {
                std::string name = callLog->toplevelDictionary[nbl::CONSTANTS.RPC_NAME()].asString();

                if (map.find(name) != map.end()) {
                    return map[name];
                } else {
                    NBL_WARN("functionFromMap() could not find function '%s'",
                             name.c_str());
                    return NULL;
                }

            } else {
                NBL_ERROR("functionFromMap() cannot call function with non- RPC_Call log: type=%s",
                          callLog->logClass.c_str());
                return NULL;
            }
        }
    }
}