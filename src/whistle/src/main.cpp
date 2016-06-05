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

#define WHISTLE_COMPILE
#include "../../share/include/SharedData.h"

const int VERSION = 3;
const char * WHISTLE_LOG_PATH = "/home/nao/nbites/log/manlog";

using namespace nblog;

nbsound::Capture * capture = NULL;
nbsound::Transform * transform = NULL;

int shared_memory_fd = 0;
volatile SharedData * shared_memory = NULL;
FILE * logFile;

void whistleExitEnd() {
    fflush(stdout);
    fclose(stdout);
    exit(0);
}

const double WHISTLE_THRESHOLD = 2000000;

/*
void callback(nbsound::Handler * cap, void * buffer, nbsound::parameter_t * params) {
    //    printf("callback %ld\n", iteration);

    bool listening = (!shared_memory || shared_memory->whistle_listen);

    if (listening && buffer && transform) {
        for (int i = 0; i < params->channels; ++i) {
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
} */

void whistleExit() {
    if (capture) {
        NBL_WARN("stopping capture...")
        if (!capture->stop()) {
        }
    }

    if (shared_memory) {
        NBL_WARN("munmap(shared_memory)...")
        munmap((void *)shared_memory, sizeof(SharedData));
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

std::pair<double, int> peak1() {
    double mv = 0;
    int mi = 0;
    for (int i = 0; i < transform->get_freq_len(); ++i) {
        double iv = std::abs(transform->outputmag[i]);
        if (iv > mv) {
            mv = iv;
            mi = i;
        }
    }

    return {mv, mi};
}

std::pair<double, int> peak2( int astart, int aend ) {
    double mv = 0;
    int mi = 0;

    for (int i = 0; i < astart; ++i) {
        double iv = std::abs(transform->outputmag[i]);
        if (iv > mv) {
            mv = iv;
            mi = i;
        }
    }

    for (int i = aend + 1; i < transform->get_freq_len(); ++i) {
        double iv = std::abs(transform->outputmag[i]);
        if (iv > mv) {
            mv = iv;
            mi = i;
        }
    }

    return {mv, mi};
}

double sum(int start, int end) {
    NBL_ASSERT_GT(transform->get_freq_len(), end);
    double total = 0;
    for (int i = start; i < end; ++i) {
        total += transform->outputmag[i] * transform->outputmag[i];
    }

    return total;
}
//
//const int whistleWindow = 50;
//const std::pair<int,int> whistleRange = {1000,2000};
//const double whistleThreshold = 600.0;
//const double whistleSumThreshold = 1000000;
//const double secondPeakMult = 4.0;

const int whistleWindow = 100;
const std::pair<int,int> whistleRange = {1000,2000};
const double whistleThreshold = 200.0;
const double whistleSumThreshold = 5000000;
const double secondPeakMult = 2.0;

void callback(nbsound::Handler * cap, void * buffer, nbsound::parameter_t * params) {
    bool listening = (!shared_memory || shared_memory->whistle_listen);

    if (listening && buffer && transform) {
        for (int i = 0; i < params->channels; ++i) {
            transform->transform(buffer, i);

            std::pair<double, int> p1 = peak1();
            printf("%d: %lf\n", p1.second, p1.first);

            if (p1.first < whistleThreshold) {
                printf("too quiet!\n");
                continue;
            }

            if (p1.second >= whistleRange.first && p1.second <= whistleRange.second) {
                printf("in range!\n");

                int wstart = std::max(0, p1.second - whistleWindow);
                int wend = std::min(transform->get_freq_len(), p1.second + whistleWindow);

                std::pair<double, int> p2 = peak2(wstart, wend);

                printf("outside {%d, %d}: %d: %lf\n",
                       wstart, wend, p2.second, p2.first);

                double sum1 = sum(wstart, wend);

                if ( (p2.first * secondPeakMult) < (p1.first) &&
                    sum1 > whistleSumThreshold ) {
                    NBL_WARN("WHISTLE HEARD! {%lf}\n", sum1);
                }
            }

        }
    }

    ++iteration;
}

int main(int argc, const char ** argv) {
    printf("\t9:21 AM\n");
	printf("\tversion=%d\n", VERSION);
    signal(SIGINT, handler);
    signal(SIGTERM, handler);

    if (argc == 1) {

        printf("...whistle...\nfreopen()....\n");
        freopen(WHISTLE_LOG_PATH, "wa", stdout);

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
    } else {
        printf("main: standalone!\n");
        shared_memory_fd = 0;
        shared_memory = nullptr;
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
	  sleep(10);
//        std::string unused;
//        std::getline(std::cin, unused);
//        if (shared_memory) {
//            NBL_WARN("FAKE WHISTLE! (newline on stdin)\n");
//            shared_memory->whistle_heard = true;
//        }
    }

    whistleExit();
    return 0;
}
