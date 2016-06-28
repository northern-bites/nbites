#include <iostream>
#include <cmath>
#include <signal.h>

#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/file.h>

#include <fcntl.h>
#include <errno.h>
#include <unistd.h>

#include "../sound/Playback.hpp"
#include "../nowifi/NoWifi.hpp"
#include "Output.hpp"

#include "utilities-test.hpp"

using namespace nbsound;
using namespace nbl;

const char * LAST_MODIFIED = "6/28 15:46";

const int WINDOW_SIZE = 16384;
Config used_config{ 48000, WINDOW_SIZE };

pthread_t capture_thread;
Playback * playback;
nowifi::SendrBase * sendr;

void cleanup() {
    if (playback) {
        if (!playback->stop()) {
            NBL_ERROR("could not stop playback")
        }
    }

    fflush(stdout);
    fclose(stdout);
    exit(0);
}

void handler(int signal) {
    NBL_WARN("sendr got signal (%d): %s\n", signal, strsignal(signal));

    psignal(signal, "the signal");
    cleanup();
    exit(-1);
}

size_t iteration = 0;
void the_callback(Handler& handler, Config& config, SampleBuffer& buffer) {
    ++iteration;
    printf("iteration %zd\n", iteration);
    sendr->fill(buffer, config);
}

int main(int argc, char ** argv) {
    printf("\t[ northern-bites data sendr ]\n");

    NBL_RUN_ALL_TESTS();

    printf("\t[LM: %s]\n", LAST_MODIFIED);

    signal(SIGINT, handler);
    signal(SIGTERM, handler);
    signal(SIGSEGV, handler);

    sendr = nowifi::getSender();

    playback = new Playback(the_callback, used_config);

    playback->init();
    playback->start_new_thread(capture_thread, NULL);

    while(true) sleep(5);

    return 0;
}











