//
//  Network.hpp
//  tool8-separate
//
//  Created by Philip Koch on 4/6/16.
//  Copyright © 2016 pkoch. All rights reserved.
//

#ifndef Network_hpp
#define Network_hpp

#include <stdio.h>
#include "Log.hpp"
#include "nblogio.h"

#include "Logging.hpp"

namespace nblog {
    namespace network {
        class SocketMaster : public Threadable {
            io::server_socket_t server;
            io::client_socket_t client;
            size_t clientIndex;
            bool connected;

            pthread_mutex_t connectionMutex;
            pthread_cond_t connectionAvailableCond;
            pthread_cond_t connectionDiedCond;

        public:
            SocketMaster(int port);

            /*For external threads: */

            //block until client connects
            void blockForClient(io::client_socket_t& sock, size_t& index);
            //check if this client index still connected
            bool checkClient(size_t clientIndex);
            //flag this client index as disconnected
            void flagClient(size_t clientIndex);

            void threadLoop();

            //NOT thread safe, NOT guaranteed intended to close important sockets and prod SocketMaster thread
            //into ending soon.
            void forceClose();
        };

        class SocketUser : public Threadable {
        protected:
            SocketMaster& master;
            io::client_socket_t socket;
            size_t clientIndex;

            SocketUser(SocketMaster& m) :
                master(m), socket(-1), clientIndex(0) {}
        public:
            virtual void threadLoop() = 0;
        };

        class Streamer : public SocketUser {
            LogProvider& provider;
            size_t index;
            logptr ping;
        public:
            Streamer(SocketMaster& master, LogProvider& prov) :
                SocketUser(master), provider(prov), index(0),
                ping(Log::explicitLog({}, json::Object(), CONSTANTS.LogClass_Null(), 0))
            {}

            void threadLoop();
        };

        class Controller : public SocketUser {
            LogConsumer& consumer;
        public:
            Controller(SocketMaster& master, LogConsumer& cons) :
                SocketUser(master), consumer(cons) {}

            void threadLoop();
        };
    }
}


#endif /* Network_hpp */
