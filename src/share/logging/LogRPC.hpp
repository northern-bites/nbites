//
//  LogRPC.hpp
//  tool8-separate
//
//  Created by Philip Koch on 4/16/16.
//  Copyright © 2016 pkoch. All rights reserved.
//

#ifndef LogRPC_hpp
#define LogRPC_hpp

#include <stdio.h>
#include "Log.hpp"
#include "nblogio.h"

#include "utilities.hpp"

namespace nbl {
    namespace rpc {

        typedef struct {
            bool argumentsValid;
            logptr returns;
        } rpc_return;

        class RPCFunctionBase {

        private:
            const std::vector<const std::string> reqArgClasses;
            const bool allowsVariableArguments;
            const std::string functionName;

            //internal use
            std::vector<logptr> returnVector;

        protected:
            //May be called at any time by derived class.
            void RETURN(logptr log);
            RPCFunctionBase(const std::string& name, bool ava, std::initializer_list<const std::string> rac);

            //In case derived class needs specialized destruction.
            virtual ~RPCFunctionBase(){}

            //May be overridden by derived class to customize setup, since constructor is not typically visible.
            virtual void initializeFunction(){}

            //MUST be overridden by all derived classes.
            virtual void defineCall(const std::vector<logptr>& arguments) = 0;

        public:

            const std::string& getName();
            const std::string describe();
            //returns < first=argumentsAccepted, second=packedReturnLogs >
            rpc_return call(logptr callingLog);
        };

        typedef std::shared_ptr<RPCFunctionBase> RPCFunctionPtr;

        typedef std::map<const std::string, RPCFunctionPtr> RPCFunctionMap;

        void mapfromFunctions(RPCFunctionMap& map, std::vector<RPCFunctionPtr> functions);

        RPCFunctionPtr functionFromMap(RPCFunctionMap& map, logptr callLog);

//Does most of the work of declaring a RPC function derivative, leaves class definition open.
//Arguments after vaAllowed are required log classes, and must be single-constructor equivalent to std::string.
#define RPC_FUNCTION_DECLARE(name, vaAllowed, ...)   \
    class RPCFunction ## name : public nbl::rpc::RPCFunctionBase {    \
        public:     RPCFunction ## name() :                             \
                            nbl::rpc::RPCFunctionBase( #name , vaAllowed, { __VA_ARGS__ } ) \
                            { initializeFunction(); }                                          \
                    void defineCall(const std::vector<nbl::logptr>& arguments);

#define RPC_FUNCTION_END    };

//Like RPC_FUNCTION_DECLARE, but closes class definition.  Sufficient for most RPC functions.
#define RPC_FUNCTION_DECLARE_COMPLETE(name, vaAllowed, ...) \
    RPC_FUNCTION_DECLARE(name, vaAllowed, ## __VA_ARGS__) RPC_FUNCTION_END

//Helper macro for defining RPCFunction::defineCall()
#define RPC_FUNCTION_DEFINE(name)   \
    void RPCFunction ## name ::defineCall(const std::vector<nbl::logptr>& arguments)

//Helper macro for create smart pointer instance of derivative RPCFunction
#define RPC_FUNCTION_INSTANTIATE(name)   \
        (std::make_shared< RPCFunction ## name >())

        /*
         One way to centralize declaring RPC functions is to put them in an x-macro, like so:
         
         #define CONTROL_FUNCTION_SET    \
            CONTROL_FUNCTION_MAKE(Func1, true, "","")     \
            CONTROL_FUNCTION_MAKE(Func2, false, "", "")

         #define CONTROL_FUNCTION_MAKE(name, va, ...) \
            RPC_FUNCTION_DECLARE_COMPLETE(name, va, ## __VA_ARGS__)
         
            CONTROL_FUNCTION_SET

         */
    }
}

#endif /* LogRPC_hpp */
