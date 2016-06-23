//
//  NoWifi.hpp
//  nowifi
//
//  Created by pkoch on 6/23/16.
//

#ifndef NoWifi_h
#define NoWifi_h

#include "utilities-pp.hpp"
#include "../sound/Sound.hpp"

namespace nowifi {

    NBL_MAKE_ENUM_FULL(CommType, Test, SimpleFSK, MultiFSK)

    static const CommType USING = Test;

    typedef void (*Callback)(time_t start, time_t end, std::string data);

    class SendrBase {
        pthread_mutex_t mutex;
        bool running;
        bool sending;

        std::string current;
        size_t current_offset;

    public:
        SendrBase();
        bool isRunning() const { return running; }
        bool isSending() const { return sending; }

        virtual void send(std::string data);
        virtual void fill(nbsound::SampleBuffer& buffer, nbsound::Config& conf) = 0;

        NBL_DISALLOW_COPY(SendrBase)
    };

    class RecvrBase {
        Callback callback;
        bool running;
        bool sending;

        std::string current;

    public:
        RecvrBase(Callback cb);
        bool isRunning() const { return running; }
        bool isSending() const { return sending; }

        virtual void parse(nbsound::SampleBuffer& buffer, nbsound::Config& conf) = 0;

        NBL_DISALLOW_COPY(RecvrBase)
    };

    SendrBase * getSender();
    RecvrBase * getRecvr();
}

#include "SimpleFSK.hpp"

#define NOWIFI_CHECK(val) if (val) { NBL_ERROR(

#endif /* NoWifi_h */
