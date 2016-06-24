#include "NoWifi.hpp"

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
    }
}