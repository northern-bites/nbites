//
//  LogRPC.hpp
//  tool8-separate
//
//  Created by Philip Koch on 4/16/16.
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
            const std::vector<std::string> reqArgClasses;
            const bool allowsVariableArguments;
            const std::string functionName;

            //internal use
            std::vector<logptr> returnVector;

        protected:
            //May be called at any time by derived class.
            void RETURN(logptr log);
            RPCFunctionBase(const std::string& name, bool ava, std::vector<std::string> rac);

            //In case derived class needs specialized destruction.
            virtual ~RPCFunctionBase(){}

            //May be overridden by derived class to customize setup, since constructor is not typically visible.
            virtual void initializeFunction(){/* no base impl */}

            //MUST be overridden by all derived classes.
            virtual void defineCall(const std::vector<logptr>& arguments) = 0;

        public:

            const std::string& getName();
            const std::string describe();
            //returns < first=argumentsAccepted, second=packedReturnLogs >
            rpc_return call(logptr callingLog);
        };

        typedef std::vector<RPCFunctionBase *> RPCFunctionVector;
        typedef std::map<const std::string, RPCFunctionBase *> RPCFunctionMap;

        void mapfromFunctions(RPCFunctionMap& map, RPCFunctionVector * functions);

        RPCFunctionBase * functionFromMap(RPCFunctionMap& map, logptr callLog);

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
    void RPCFunction ## name ::defineCall

//Helper macro for create smart pointer instance of derivative RPCFunction
#define RPC_FUNCTION_INSTANTIATE(name)   \
          (new RPCFunction ## name ())
//        (std::make_shared< RPCFunction ## name >())

#define RPC_MAKE_FUNCTION_GROUP(name)   \
    nbl::rpc::RPCFunctionVector * the ## name ## Vector = nullptr;  \
    nbl::rpc::RPCFunctionVector * get ## name ## Vector() {         \
        if ( the ## name ## Vector != nullptr) return the ## name ## Vector ;   \
        else return ( the ## name ## Vector = new nbl::rpc::RPCFunctionVector{} ); }

#define RPC_MAKE_FUNCTION_GROUP_VISIBLE(name)   \
    extern nbl::rpc::RPCFunctionVector * get ## name ## Vector();

        struct static_rpc_adder {
            static_rpc_adder(RPCFunctionBase * ptr, std::vector<RPCFunctionBase *> * functions) {
                NBL_ASSERT_NE(ptr, NULL)
                NBL_ASSERT_NE(functions, NULL)

                functions->push_back(ptr);
            }
        };

#define RPC_UNIQUE_NAME1(prefix, name, line) RPC_UNIQUE_NAME2(prefix,name,line)
#define RPC_UNIQUE_NAME2(prefix, name, line) prefix ## name ## line

#define RPC_FUNCTION_STATIC_ADD(name, ptr, group)   \
    struct nbl::rpc::static_rpc_adder   \
        RPC_UNIQUE_NAME1(nbl_rpc_adder, name, __LINE__)  \
            {ptr, get ## group ## Vector()};

    }
}

#endif /* LogRPC_hpp */
