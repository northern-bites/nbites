//
//  nbcross.cpp
//  tool8-separate
//
//  Created by Philip Koch on 4/16/16.
//

#include "nbcross.hpp"
#include "nblogio.h"
#include <getopt.h>

#define NBL_LOGGING_LEVEL NBL_WARN_LEVEL

RPC_MAKE_FUNCTION_GROUP(NBCross)

namespace nbl {
    namespace nbcross {
        std::string NBCROSS_INSTANCE_NAME = "default";
        std::string SERVER_NAME = io::LOCAL_HOST_ADDR;
        bool NBCROSS_PRINTOUTS_ON = true;

        const std::string breakString(50, '-');
        void print_break(int newlines = 0) {
            std::string nl(newlines, '\n');
            std::cout << breakString << nl << std::endl;
        }

        int nbcross_main(int argc, char ** argv) {

            char flag;
            while ( (flag = getopt(argc, argv, "qn:s:")) != -1) {
                switch (flag) {
                    case 'q':
                        NBL_PRINT("nbcross quiet mode.");
                        NBCROSS_PRINTOUTS_ON = false;
                        break;

                    case 'n':
                        NBL_PRINT("nbcross instance name = %s",
                                  optarg);
                        NBCROSS_INSTANCE_NAME = optarg;
                        break;

                    case 's':
                        NBL_PRINT("nbcross server address = %s",
                                  optarg);
                        SERVER_NAME = optarg;
                        break;

                    default:
                        NBL_ERROR("FATAL: invalid argument: %c\n\tusage: nbcross -q -n <inst_name> -s <server_addr>", optopt);
                        exit(-1);
                }
            }

            print_break();

            rpc::RPCFunctionMap map;
            rpc::mapfromFunctions(map, getNBCrossVector() );

            for (auto func : map) {
                NBL_PRINT("\t%s", func.first.c_str());
            }

            print_break();

            io::client_socket_t sock;
            io::ioret conret = io::connect_to(sock, SharedConstants::CROSS_PORT(), SERVER_NAME.c_str());
            logptr heartbeat = Log::emptyLog();
            heartbeat->logClass = SharedConstants::LogClass_Null();

            if (conret) {
                NBL_ERROR("FATAL: nbcross connection failed.");
                exit(1);
            }

            NBL_PRINT("connection to '%s' established.\n",
                      SERVER_NAME.c_str());

            print_break(2);

            for (;;) {

                NBL_INFO("loop...");

                logptr call = Log::recv(sock);

                if (!IS_PTR_VALID(call)) {
                    NBL_ERROR("FATAL: could not read rpc function call from socket.");
                    exit(1);
                }

                if (call->logClass == SharedConstants::LogClass_Null()) {
                    NBL_INFO("heartbeat...")

                    io::ioret failure = heartbeat->send(sock);
                    if (failure) {
                        NBL_ERROR("FATAL: could not send heartbeat to socket.");
                        exit(1);
                    }

                    continue;
                }

                NBL_ASSERT_EQ(call->logClass, SharedConstants::LogClass_RPC_Call());

                rpc::RPCFunctionBase * fptr = functionFromMap(map, call);
                if (!IS_PTR_VALID(fptr)) {
                    NBL_ERROR("FATAL: nbcross could not find function.");
                    exit(1);
                }

                print_break();
                NBL_PRINT("calling: %s", fptr->getName().c_str());
                rpc::rpc_return ret = fptr->call(call);
                print_break(2);

                if (!ret.argumentsValid) {
                    NBL_ERROR("FATAL: nbcross given invalid arguments for: %s",
                              fptr->getName().c_str());
                    exit(1);
                }
                
                conret = ret.returns->send(sock);
                if (conret) {
                    NBL_ERROR("FATAL: could not send rpc return to socket.");
                    exit(1);
                }
            }
            
            return 0;
        }
    }
}

#include "utilities-test.hpp"

int main(int argc, char ** argv) { NBL_RUN_ALL_TESTS(); return nbl::nbcross::nbcross_main(argc, argv); }




