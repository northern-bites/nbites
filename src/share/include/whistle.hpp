#ifndef WHISTLE_SHARE_H
#define WHISTLE_SHARE_H

#include <iostream>
#include <cmath>
#include <signal.h>

#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/file.h>

#include <fcntl.h>
#include <errno.h>
#include <unistd.h>

#include "../logshare/utilities-pp.hpp"
#include "SharedData.h"
#include "TextToSpeech.h"

namespace whistle {

    class SharedMemory {
        int shared_fd;
        SharedData * shared;
    public:
        SharedMemory() {
            shared_fd = 0;
            shared = NULL;
        }

        bool open() {
            shared_fd = shm_open(NBITES_MEM, O_RDWR, 0600);
            if (shared_fd < 0) {
                NBL_ERROR("could not open shared memory file!")
                return false;
            }

            shared = (SharedData*) mmap(NULL, sizeof(SharedData),
                                                 PROT_READ | PROT_WRITE,
                                                 MAP_SHARED, shared_fd, 0);

            if (shared == MAP_FAILED) {
                NBL_ERROR("could not mmap shared memory file!")
                return false;
            }

            return true;
        }

        bool isOpen() {
            return (shared_fd > 0 && (shared != MAP_FAILED && shared));
        }

        void close() {
            if (shared != MAP_FAILED && shared)
                munmap((void *)shared, sizeof(SharedData));
            if (shared_fd > 0)
                ::close(shared_fd);
        }

        bool& whistle_listening() {
            return shared->whistle_listen;
        }

        bool& whistle_heard() {
            return shared->whistle_heard;
        }

        time_t& whistle_heartbeat() {
            return shared->whistle_heartbeat;
        }

        void whistle_alive() {
            shared->whistle_heartbeat = time(NULL);
        }

        bool gamestate_check_running() {
            static time_t last_complain = 0;

            double dt = difftime(time(NULL), whistle_heartbeat() );
            if (dt > 10) {
                NBL_WARN("gamestate hasn't heard from whistle for %lf seconds!", dt);

                if (difftime(time(NULL), last_complain) > 30) {
                    last_complain = time(NULL);
                    man::tts::say(IN_SCRIMMAGE, "whistle is not running!");

		    if (!fork()) {
			execl("/home/nao/whistle", "", NULL);

			for(;;) {
				printf("WHISTLE FAILED TO LOAD!!\n");
			        exit(-1);
				kill(getpid(), SIGKILL);
			}
		    }
                }

                return false;
            } else {
                return true;
            }
        }

        bool gamestate_do_start() {
            NBL_ASSERT(isOpen())
            if (!gamestate_check_running()) {
                return false;
            }

            whistle_heard() = false;
            whistle_listening() = true;
            return true;
        }

        bool gamestate_do_query() {
            NBL_ASSERT(isOpen())
            if (!gamestate_check_running()) {
                return false;
            }

            bool heard = whistle_heard();
            if (!heard) whistle_listening() = true;
            return whistle_heard();
        }

        bool gamestate_do_stop() {
            NBL_ASSERT(isOpen())
            if (!gamestate_check_running()) {
                return false;
            }

            whistle_heard() = false;
            whistle_listening() = false;
            return true;
        }
    };
}

#endif //WHISTLE_SHARE_H
