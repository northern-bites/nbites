//
//  Logging.hpp
//  tool8-separate
//
//  Created by Philip Koch on 3/21/16.
//  Copyright © 2016 pkoch. All rights reserved.
//

#ifndef Logging_hpp
#define Logging_hpp

#include <stdio.h>

#include <memory>
#include <deque>

#include "Log.hpp"
#include "nblogio.h"

namespace nbl {

    /* interface for man */

#define LOGGING_Q_M_SET         \
    LOGGING_Q_M(Q_CONTROL)      \
    LOGGING_Q_M(Q_STREAM)       \
    LOGGING_Q_M(Q_FILESYSTEM)   \
    LOGGING_Q_M(NUM_QUEUES)     \
    LOGGING_Q_M(NONE)           \


#define LOGGING_Q_M(name) name ,

    /*
    Q_CONTROL,   //it's from a control call!
    Q_STREAM,    //it's just a streamed log!
    Q_FILESYSTEM,//write me to the filesystem (if enabled)!
    NUM_QUEUES,
    NONE
     */

    //where the log should end up
    enum q_enum {
        LOGGING_Q_M_SET
    };

#undef LOGGING_Q_M

    extern const char * Q_NAMES[];

    typedef std::initializer_list<q_enum> qorder;

    bool contains(qorder order, q_enum q);

    bool initiateLogging();
    bool teardownLogging();

    /**
     *  external access to logging system.
     *  q defines what queue the log is to be put on.
     *      for multiple queues, call NBLog multiple times.
     *      the logptr will ensure the log is not copied.
     *  returns ( logging_on && (queue did not overflow) )
     */
    bool NBLog(logptr log, q_enum q = Q_STREAM);


    //-----------------------------------------------------------
    /* interface for parts of logging system */

    class LogProvider {
    public:
        virtual logptr blockForLog(qorder order, io::iotime_t max_wait) = 0;
    };

    class LogConsumer {
    public:
        virtual void consumeLog(logptr ptrTo) = 0;
    };

    class Center : public LogProvider {

        static const size_t MAX_QUEUE_SIZE = 10;
        std::deque<logptr> queues[NUM_QUEUES];

        pthread_mutex_t dequeMutex;
        pthread_cond_t logAvailableCond;

    public:
        Center();
        //returns (success && (queue did not overflow))
        bool addLog(logptr log, q_enum q);
        logptr blockForLog(qorder order, io::iotime_t max_wait);
    };

    class Threadable {
    protected:
        bool running = true;
    public:
        bool stop();
        virtual void threadLoop() = 0;
    };
}


#endif /* Logging_hpp */
