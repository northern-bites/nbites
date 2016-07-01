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

    static const CommType USING = SimpleFSK;

    typedef void (*Callback)(time_t start, time_t end, std::string data);

    class SendrBase {
        NBL_DISALLOW_COPY(SendrBase)

    protected:
        pthread_mutex_t mutex;
        bool running;
        bool sending;

        std::string current;
        size_t current_offset;

    public:
        SendrBase();
        virtual ~SendrBase(){}
        bool isRunning() const { return running; }
        bool isSending() const { return sending; }

        virtual void send(std::string data);
        virtual void fill(nbsound::SampleBuffer& buffer, nbsound::Config& conf) = 0;

    };

    class RecvrBase {
        NBL_DISALLOW_COPY(RecvrBase)

        Callback callback;
        bool running;
        bool recving;

        time_t start;
        std::string current;
        time_t end;
        virtual void finish();

    public:
        RecvrBase(Callback cb);
        virtual ~RecvrBase(){}

        bool isRunning() const { return running; }
        bool isRecving() const { return recving; }

        virtual void parse(nbsound::SampleBuffer& buffer, nbsound::Config& conf) = 0;

    };

    SendrBase * getSender();
    RecvrBase * getRecvr(Callback callback);
}

//#define NOWIFI_CHECK(val) if (val) { NBL_ERROR(

#endif /* NoWifi_h */
