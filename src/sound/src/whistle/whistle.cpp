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

#include "../sound/Detect.hpp"

#include "Logging.hpp"

const char * LAST_MODIFIED = "6/22 16:00";
const char * WHISTLE_LOG_PATH = "/home/nao/nbites/log/whistle";

using namespace nbl;
using namespace nbsound;

pthread_t capture_thread;
Capture * capture = nullptr;
whistle::SharedMemory shared;

bool useLogging = false;

void whistle_cleanup() {
    NBL_WARN("clearing up whistle process...");

    detect::cleanup();

    if (useLogging) {
        nbl::teardownLogging();
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

Config used_config{ 48000, 16384 };

size_t iteration = 0;

void the_callback(Handler& handler, Config& config, SampleBuffer& buffer) {

    if (useLogging) {
        int length = buffer.size_bytes();
        std::string newData( (const char *) buffer.buffer, length);

        printf("sending....\n");
        nbl::logptr newLog = nbl::Log::emptyLog();
        newLog->logClass = "DetectAmplitude";
        newLog->blocks.push_back(nbl::Block{newData, "SoundAmplitude"});
        nbl::NBLog(newLog);
    }

    if (shared.isOpen()) {
        shared.whistle_heartbeat() = time(NULL);
    }

    if ( !shared.isOpen() || shared.whistle_listening() ) {

        bool heard = detect::detect(buffer);

        if (heard) {
            do_heard();
        }

    } else {
        std::cout << "." << std::endl;
    }

    ++iteration;
}

int main(int argc, const char * argv[]) {

    signal(SIGINT, handler);
    signal(SIGTERM, handler);
    signal(SIGSEGV, handler);

    NBL_INFO("\twhistle ( %s )", LAST_MODIFIED);
    
    if (argc > 1) {
        NBL_WARN("-------------------------- whistle stand"
                 "alone mode --------------------------");

        if (std::string{argv[1]} == std::string{"log"}) {
            NBL_WARN("using logging (connect via nbtool)")
            useLogging = true;
            nbl::initiateLogging();
        }
    } else {
        NBL_WARN("whistle server mode");

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


























