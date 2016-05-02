//
//  Logging.cpp
//  tool8-separate
//
//  Created by Philip Koch on 3/21/16.
//  Copyright © 2016 pkoch. All rights reserved.
//

#include "Logging.hpp"

#include "utilities.hpp"
#include "nblogio.h"

#include "Network.hpp"
#include "Control.hpp"
#include "Filesystem.hpp"
#include "Log.hpp"

#include <string>
#include <algorithm>
#include <deque>

#include <unistd.h>

#define NBL_LOGGING_LEVEL NBL_INFO_LEVEL

namespace nblog {

#define LOGGING_Q_M(name) #name ,
    const char * Q_NAMES[] = {
        LOGGING_Q_M_SET
    };
#undef LOGGING_Q_M

    Center * center = NULL;

    using network::SocketMaster;
    using network::Streamer;
    using network::Controller;
    using filesystem::Filelogger;

    SocketMaster * socketMaster = NULL;
    pthread_t socketThread, streamThread, controlThread, fileThread;
    Streamer * streamer = NULL;
    Controller * controller = NULL;
    Filelogger * filelogger = NULL;

    control::ControlHandler * cntrlHandler = NULL;

    #define MakeMethodWrapper(CLASS, METHOD)                \
        void * METHOD ## WrapperFor ## CLASS(void * obj) {  \
        ( (CLASS *) obj)->METHOD(); return NULL;            }

    MakeMethodWrapper(SocketMaster, threadLoop)
    MakeMethodWrapper(Streamer, threadLoop)
    MakeMethodWrapper(Controller, threadLoop)
    MakeMethodWrapper(Filelogger, threadLoop)

    bool initiateLogging() {
        if (center) {
            return false;
        }

        center = new Center();
        cntrlHandler = new control::ControlHandler();
        cntrlHandler->printFunctions();

        socketMaster = new SocketMaster(CONSTANTS.ROBOT_PORT());

        pthread_create(&socketThread, NULL, threadLoopWrapperForSocketMaster, socketMaster);
        pthread_detach(socketThread);

        streamer = new Streamer(*socketMaster, *center);
        pthread_create(&streamThread, NULL, threadLoopWrapperForStreamer, streamer);
        pthread_detach(streamThread);

        controller = new Controller(*socketMaster, *cntrlHandler);
        pthread_create(&controlThread, NULL, threadLoopWrapperForController, controller);
        pthread_detach(controlThread);

        filelogger = new Filelogger(*center);
        pthread_create(&fileThread, NULL, threadLoopWrapperForFilelogger, filelogger);

        return true;
    }

    bool teardownLogging() {
        if (socketMaster) {
            socketMaster->forceClose();
            socketMaster->stop();
        }
        if (streamer)
            streamer->stop();
        if (controller)
            controller->stop();
        return true;
    }

    bool NBLog(logptr log, q_enum q) {
        if (center && q < nblog::NUM_QUEUES) {
            return center->addLog(log, q);
        }

        else return false;
    }

    bool Threadable::stop() {
        bool temp = running;
        running = false;
        return temp;
    }

    Center::Center() {
        pthread_mutex_init(&dequeMutex, NULL);
        pthread_cond_init(&logAvailableCond, NULL);
    }

    bool Center::addLog(logptr log, nblog::q_enum q) {
        NBL_ASSERT(IS_PTR_VALID(log));
        if (q >= NUM_QUEUES) return false;

        pthread_mutex_lock(&dequeMutex);
        queues[q].push_back(log);
        bool no_overflow = true;

        if (queues[q].size() > MAX_QUEUE_SIZE) {
            no_overflow = false;
            queues[q].pop_front();
        }

        pthread_cond_broadcast(&logAvailableCond);
        pthread_mutex_unlock(&dequeMutex);

        NBL_LOG_IF(NBL_INFO_LEVEL, !no_overflow, "Center::addLog() overflow on queue: %s", Q_NAMES[q]);
        return no_overflow;
    }

    logptr Center::blockForLog(qorder order, io::iotime_t max_wait) {
        NBL_ASSERT_GT(order.size(), 0);
        NBL_ASSERT_GT(max_wait, 0);

        pthread_mutex_lock(&dequeMutex);
        struct timespec until = io::io_get_abs_ts(max_wait);

        for (;;) {
            for (q_enum q : order) {
                if (!queues[q].empty()) {
                    logptr ret = queues[q].front();
                    queues[q].pop_front();

                    pthread_mutex_unlock(&dequeMutex);
                    return ret;
                }
            }

            int twr = pthread_cond_timedwait(&logAvailableCond, &dequeMutex, &until);
            NBL_ASSERT(twr == ETIMEDOUT || twr == 0)

            if (twr == ETIMEDOUT) {
                pthread_mutex_unlock(&dequeMutex);
                return NULL;
            }
        }
    }

}