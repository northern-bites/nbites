#include <iostream>
#include <cmath>
#include <signal.h>

#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/file.h>

#include <fcntl.h>
#include <errno.h>
#include <unistd.h>

#include "../sound/Sound.hpp"
#include "../sound/Capture.hpp"
#include "../sound/Transform.hpp"

#include "whistle.hpp"

#include "../sound/Whistle.hpp"

const char * LAST_MODIFIED = "6/12 19:53";
const char * WHISTLE_LOG_PATH = "/home/nao/nbites/log/whistle";

using namespace nbl;
using namespace nbsound;

pthread_t capture_thread;
Capture * capture = nullptr;
whistle::SharedMemory shared;

int whistleSingleFD = 0;

void whistle_cleanup() {
    NBL_WARN("clearing up whistle process...");

    detect::cleanup();

    if (whistleSingleFD > 0) {
        flock(whistleSingleFD, LOCK_UN);
        close(whistleSingleFD);
    }

    if (shared.isOpen()) {
        NBL_WARN("closing SharedMemory")
        shared.close();
    }

    printf("\tstopping capture...\n");
    if (capture) {
        if (!capture->stop()) {
            NBL_WARN("could not stop capture!")
        }
    }

    fflush(stdout);
    fclose(stdout);
    exit(-2);
}

void handler(int signal) {
    NBL_WARN("whistle got signal (%d): %s\n", signal, strsignal(signal));

    psignal(signal, "the signal");
    whistle_cleanup();
    exit(-1);
}

/* procesing code... */

void do_heard() {
    NBL_WARN(":::::::::::::::::::: WHISTLE HEARD ::::::::::::::::::::");
    if (shared.isOpen()) {
        shared.whistle_heard() = true;
        shared.whistle_listening() = false;
    }
}

Config used_config{48000, 16384};

size_t iteration = 0;



void the_callback(Handler& handler, Config& config, SampleBuffer& buffer) {

    if (shared.isOpen()) {
        shared.whistle_heartbeat() = time(NULL);
    }

    if ( !shared.isOpen() || shared.whistle_listening() ) {

        bool heard = detect::detect(buffer);

        if (heard) {
            do_heard();
        }

    } else {
        printf(".");
    }

    ++iteration;
}

void establishLock() {
    whistleSingleFD = open("/home/nao/nbites/whistle.lock", O_CREAT | O_RDWR, 0666);
    if (whistleSingleFD < 0) {
        int err = errno;
        nbl::utilities::safe_perror(err);
        NBL_ERROR("could not open lock file.")
        exit(0);
    }

    int result = flock(whistleSingleFD, LOCK_EX | LOCK_NB);
    if (result == -1) {
        NBL_ERROR("could not lock file – is whistle already running?")
        exit(0);
    }
}

int main(int argc, const char * argv[]) {

    signal(SIGINT, handler);
    signal(SIGTERM, handler);
    signal(SIGSEGV, handler);

    NBL_INFO("\twhistle ( %s )", LAST_MODIFIED);
    if (argc > 1) {
        NBL_WARN("--------------------------\nwhistle stand"
                 "alone mode\n--------------------------");
    } else {
        NBL_WARN("whistle server mode");

        establishLock();

        NBL_WARN("( whistle reopening to %s now )",
                 WHISTLE_LOG_PATH);
        freopen(WHISTLE_LOG_PATH, "wa", stdout);
        NBL_WARN("whistle separate logging start:\n\n")

        NBL_ASSERT(shared.open());
    }

    detect::init();
    capture = new Capture(the_callback, used_config);

    capture->init();
    capture->start_new_thread(capture_thread, NULL);

    while(true) sleep(5);

    return 1;
}


























