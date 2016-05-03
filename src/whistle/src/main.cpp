#include <iostream>
#include <signal.h>

#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>

#include "Sound.h"
#include "Transform.h"
#include "nblogio.h"
#include "utilities.hpp"

#include "../../share/include/SharedData.h"

const int VERSION = 3;

using namespace nblog;

nbsound::Capture * capture = NULL;
nbsound::Transform * transform = NULL;

int shared_memory_fd = 0;
SharedData * shared_memory = NULL;
FILE * logFile;

void whistleExitEnd() {
    fflush(stdout);
    fclose(stdout);
    exit(0);
}

void whistleExit() {
    if (capture) {
        NBL_WARN("stopping capture...")
        if (!capture->stop()) {
        }
    }

    if (client) {
        NBL_WARN("close(client)...")
        close(client);
    }

    if (server) {
        NBL_WARN("close(server)...")
        close(server);
    }

    if (shared_memory) {
        NBL_WARN("munmap(shared_memory)...")
        munmap((void *)shared, sizeof(SharedData));
    }

    if (shared_memory_fd > 0) {
        NBL_WARN("close(shared_memory_fd)...")
        close(shared_memory_fd);
    }

    whistleExitEnd();
}

void handler(int signal) {
    printf("... handling signal ...\n");
    whistleExit();
}

long iteration = 0;

double sum(int start, int end) {
    NBL_ASSERT_GT(transform->get_freq_len(), end);
    double total = 0;
    for (int i = start; i < end; ++i) {
        total += transform->outputmag[i] * transform->outputmag[i];
    }

    return total;
}

const double WHISTLE_THRESHOLD = 2000000;

void callback(nbsound::Handler * cap, void * buffer, nbsound::parameter_t * params) {
//    printf("callback %ld\n", iteration);

    bool listening = (shared_memory && shared_memory->whistle_listen);

    if (listening && buffer && transform) {
        for (int i = 0; i < params->channels; ++i) {
//            printf("\ttransform %d\n", i);
            transform->transform(buffer, i);

            double summed = sum(1600, 1800);
//            NBL_PRINT("summed=\t%lf\n", summed);
            if (summed > WHISTLE_THRESHOLD) {
                NBL_WARN("WHISTLE: %lf\n", summed);
                if (shared_memory) {
                    shared_memory->whistle_heard = true;
                }
            }
        }
    }

    ++iteration;
}

int main(int argc, const char ** argv) {
	printf("\tversion=%d\n", VERSION);
    signal(SIGINT, handler);
    signal(SIGTERM, handler);

    printf("...whistle...\nfreopen()....\n");
    freopen(WHISTLE_LOG_PATH, "w", stdout);

    NBL_INFO("whistle::main() log file re-opened...");

    shared_memory_fd = shm_open(NBITES_MEM, O_RDWR, 0600);
    if (shared_memory_fd < 0) {
        std::cout << "sensorsModule couldn't open shared fd!" << std::endl;
        NBL_ERROR("whistle couldn't open shared memory file!");
        whistleExitEnd();
    }

    shared_memory = (volatile SharedData*) mmap(NULL, sizeof(SharedData),
                                         PROT_READ | PROT_WRITE,
                                         MAP_SHARED, shared_memory_fd, 0);

    if (shared_memory == MAP_FAILED) {
        NBL_ERROR("whistle couldn't map shared memory file!");
        close(shared_memory_fd);
        whistleExitEnd();
    }

    NBL_WHATIS(shared_memory);

    nbsound::parameter_t params = {nbsound::NBS_S16_LE, 2, 32768, 48000};
    transform = new nbsound::Transform(params);
    capture = new nbsound::Capture(callback, params);

    printf("main: period is %lf seconds\n", nbsound::PERIOD(params));
    printf("main: sample freq is %lf seconds\n", nbsound::FREQUENCY(params));

    capture->init();
    std::cout << capture->print() << std::endl;
    pthread_t capture_thread;

    printf("main: capture created...\n");
    capture->start_new_thread(capture_thread, NULL);

    while(capture->is_active()) {
        std::string unused;
        std::getline(std::cin, unused);
        if (shared_memory) {
            NBL_WARN("FAKE WHISTLE! (newline on stdin)\n");
            shared_memory->whistle_heard = true;
        }
    }

    whistleExit();
    return 0;
}
