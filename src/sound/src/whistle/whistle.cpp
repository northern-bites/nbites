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

const char * LAST_MODIFIED = "6/23 22:28";
const char * WHISTLE_LOG_PATH = "/home/nao/nbites/log/whistle";

using namespace nbl;
using namespace nbsound;

pthread_t capture_thread;
Capture * capture = nullptr;
whistle::SharedMemory shared;

bool useLogging = false;

#define SAVE_HEARD_WHISTLES

logptr logFromBuffer(SampleBuffer& buffer) {
    nbl::logptr newLog = nbl::Log::emptyLog();
    newLog->logClass = "DetectAmplitude";
    newLog->blocks.push_back(nbl::Block{buffer.toString(), "SoundAmplitude"});
    return newLog;
}

logptr logFromRingBuffer(SampleRingBuffer& buffer) {
    nbl::logptr newLog = nbl::Log::emptyLog();
    newLog->logClass = "DetectAmplitude";
    newLog->blocks.push_back(nbl::Block{buffer.toString(), "SoundAmplitude"});
    return newLog;
}

void simple_write_log(logptr log, size_t inter_heard) {
    NBL_INFO("writing whistle log in frame %zd...", inter_heard)

    std::string fileName = utilities::format("%s/whistle_log_%zd.nblog",
                                             SharedConstants::ROBOT_LOG_PATH_PREFIX().c_str(), inter_heard);
    std::string fileContents;
    log->serialize(fileContents);

    io::writeStringToFile(fileContents, fileName);
}

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

const int WINDOW_SIZE = 16384;
Config used_config{ 48000, WINDOW_SIZE };

#ifdef SAVE_HEARD_WHISTLES
SampleRingBuffer ringBuffer(4, used_config.num_channels, used_config.window_size);
#endif

size_t iteration = 0;
size_t listening = 0;

void the_callback(Handler& handler, Config& config, SampleBuffer& buffer) {

#ifdef SAVE_HEARD_WHISTLES
    ringBuffer.push(buffer);
#endif

    if (useLogging) {
        nbl::NBLog(logFromBuffer(buffer));
    }

    if (shared.isOpen()) {
        shared.whistle_heartbeat() = time(NULL);
    }

    if ( !shared.isOpen() || shared.whistle_listening() ) {
        ++listening;

        bool ignore_for_start = false;
        double buffer_fraction = config.window_size / (double) config.sample_rate;

        if ( (listening * buffer_fraction) < 1.5) {
            ignore_for_start = true;
        }

        bool heard = detect::detect(buffer);

        if (heard) {
            if (ignore_for_start) {
                NBL_WARN("WHISTLE HEARD but ignoring for first 1.5 seconds");
            } else {
                do_heard();
            }

#ifdef SAVE_HEARD_WHISTLES
            simple_write_log(logFromRingBuffer(ringBuffer), iteration);
#endif
        }

    } else {
        listening = 0;
        std::cout << "." << std::endl;
    }

    ++iteration;
}

int main(int argc, const char * argv[]) {

    signal(SIGINT, handler);
    signal(SIGTERM, handler);
    signal(SIGSEGV, handler);

    NBL_INFO("\twhistle ( %s )", LAST_MODIFIED);

#ifdef SAVE_HEARD_WHISTLES
    NBL_WARN("*** SAVING HEARD WHISTLES ***")
#endif
    
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


























