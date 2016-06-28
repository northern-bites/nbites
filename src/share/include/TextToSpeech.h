#ifndef TEXT_TO_SPEECH_H
#define TEXT_TO_SPEECH_H

#include <unistd.h>
#include <stdlib.h>
#include <signal.h>

#if !defined(NBL_STANDALONE) && !defined(NB_BOSS_COMPILATION)
#include "DebugConfig.h"
#endif

enum say_when {
    IN_DEBUG,
    IN_SCRIMMAGE,
    IN_GAME
};

namespace man {
    namespace tts {

        static const say_when CURRENT_LEVEL = IN_DEBUG;

#if (!defined(OFFLINE) && defined(USE_ROBOT_TTS)) || defined(NBL_STANDALONE)
        static inline void internal_say(const char * line) {
            if (!fork()) {
                execl("/usr/bin/say", "", line, NULL);
                exit(0);
                kill(getpid(), SIGKILL);
            }
        }

        static inline void say(say_when upTo, const char * line) {
            if (upTo >= CURRENT_LEVEL) {
                printf("say: %s\n", line);
                internal_say(line);
            } else {
                printf("say [blocked]: %s\n", line);
            }
        }
#else
        static inline void internal_say(const char * line){}
        static inline void say(say_when upTo, const char * line){}
#endif

    }
}


#endif // TEXT_TO_SPEECH_H