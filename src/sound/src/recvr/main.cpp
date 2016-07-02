#include <iostream>
#include <cmath>
#include <signal.h>

#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/file.h>

#include <fcntl.h>
#include <errno.h>
#include <unistd.h>

#include "../nowifi/NoWifi.hpp"
#include "Input.hpp"
#include "../sound/Capture.hpp"

#include "../nowifi/SimpleFSK.hpp"

using namespace nbsound;
using namespace nbl;

const char * LAST_MODIFIED = "7/2 07:30";

const int WINDOW_SIZE = nowifi::SIMPLE_FSK_WINDOW_SIZE;
Config used_config{ 48000, WINDOW_SIZE, 2 };

pthread_t capture_thread;
Capture * capture;
nowifi::RecvrBase * recvr;

void cleanup() {
    if (capture) {
        if (!capture->stop()) {
            NBL_ERROR("could not stop capture")
        }
    }

    fflush(stdout);
    fclose(stdout);
    exit(0);
}

void handler(int signal) {
    NBL_WARN("recvr got signal (%d): %s\n", signal, strsignal(signal));

    psignal(signal, "the signal");
    cleanup();
    exit(-1);
}

size_t iteration = 0;
void the_callback(Handler& handler, Config& config, SampleBuffer& buffer) {
    ++iteration;
    printf("iteration %zd\n", iteration);
    recvr->parse(buffer, config);
}

void recvr_callback(time_t start, time_t end, std::string data) {
    printf("recvr callback!\n");

    for (int i = 0; i < data.size(); ++i) {
        printf("\t0x%X\n", data[i]);
    }
}

int main(int argc, char ** argv) {
    printf("\t[ northern-bites data recvr ]\n");
    printf("\t[LM: %s]\n", LAST_MODIFIED);

    signal(SIGINT, handler);
    signal(SIGTERM, handler);
    signal(SIGSEGV, handler);

    recvr = nowifi::getRecvr(recvr_callback);

    capture = new Capture(the_callback, used_config);

    capture->init();
    capture->start_new_thread(capture_thread, NULL);

    while(true) sleep(5);

    return 0;
}