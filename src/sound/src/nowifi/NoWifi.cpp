#include "NoWifi.hpp"
#include "SimpleFSK.hpp"
#include "Correlation.hpp"

namespace nowifi {
    SendrBase::SendrBase() {
        running = true;
        sending = false;
        pthread_mutex_init(&mutex, NULL);

        current_offset = 0;
    }

    void SendrBase::send(std::string data) {
        if (!running) {
            NBL_ERROR("Sender not running.")
            return;
        }

        pthread_mutex_lock(&mutex);
        if (sending) {
            pthread_mutex_unlock(&mutex);
            NBL_ERROR("already sending!")
            return;
        }

        sending = true;
        current = data;
        current_offset = 0;

        pthread_mutex_unlock(&mutex);
    }

    RecvrBase::RecvrBase(Callback cb) {
        callback = cb;
        running = true;
        recving = false;
    }

    void RecvrBase::finish(std::string data) {
        NBL_ASSERT(start > 0)
        NBL_ASSERT(end > 0)

        callback(start, end, data);
    }

    SendrBase * getSender() {
        switch (USING) {
            case Test: return new CorrSender{};
            case SimpleFSK: return new SimpleFSKSendr{};
            case MultiFSK: return nullptr;
        }
    }

    RecvrBase * getRecvr(Callback callback) {
        switch (USING) {
            case Test: return new CorrRecvr{callback};
            case SimpleFSK: return new SimpleFSKRecvr{callback};
            case MultiFSK: return nullptr;
        }
    }
}