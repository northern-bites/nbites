#ifndef TEXT_TO_SPEECH_H
#define TEXT_TO_SPEECH_H

#include <unistd.h>
#include <stdlib.h>
#include <signal.h>

enum say_when {
    IN_DEBUG,
    IN_SCRIMMAGE,
    IN_GAME
};

namespace man {
    namespace tts {

        static const say_when CURRENT_LEVEL = IN_DEBUG;

        static inline void internal_say(const char * line) {
            if (!fork()) {
                static const int line_size = 500;
                char buffer[line_size];
                snprintf(buffer, line_size, "say \"%s\"", line);
                system( (const char *) buffer );
                for (;;) {                  //forever
                    ::exit(0);              //in some cases, this does not seem to be enough (weird naoqi process?)

                    kill(getpid(), SIGKILL);//so send SIGKILL too
                }
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
    }
}


#endif // TEXT_TO_SPEECH_H