#include "NoWifi.hpp"
#include "SimpleFSK.hpp"
#include "Correlation.hpp"

namespace nowifi {
    SendrBase::SendrBase() {
        running = false;
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
        running = false;
        recving = false;
    }

    void RecvrBase::finish() {
        NBL_ASSERT(start > 0)
        NBL_ASSERT(end > 0)

        callback(start, end, current);
        current.clear();
    }

    SendrBase * getSender() {
        switch (USING) {
            case Test: return new CorrSender{};
            case SimpleFSK: return nullptr;
            case MultiFSK: return nullptr;
        }
    }

    RecvrBase * getRecvr(Callback callback) {
        switch (USING) {
            case Test: return new CorrRecvr{callback};
            case SimpleFSK: return nullptr;
            case MultiFSK: return nullptr;
        }
    }
}