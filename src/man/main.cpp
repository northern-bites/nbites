
#include "Man.h"
#include "SharedData.h"
#include "whistle.hpp"

#include <sys/file.h>
#include <sys/types.h>
#include <sys/wait.h>

#include <errno.h>
#include <unistd.h>
#include <sys/wait.h>

int lockFD = 0;
man::Man* instance;

pid_t whistlePID = 0;
const char * MAN_LOG_PATH = "/home/nao/nbites/log/manlog";

void cleanup() {

    printf("::::::::::::::::::: MAN cleanup code executing! :::::::::::::::::::\n");

    if (whistlePID > 0) {
        kill(whistlePID, SIGTERM);
    }

    // Give man a chance to clean up behind it
    // I.e. close camera driver gracefully
    instance->preClose();
    flock(lockFD, LOCK_UN);

    printf("Man closing output streams...\n");
    fflush(stderr);
    fflush(stdout);
    fclose(stdout);

    if (instance) delete instance;
    printf("::::::::::::::::::: MAN cleanup code finished! :::::::::::::::::::\n");
}

void handler(int signal)
{
    cleanup();
    exit(0);
}

void error_signal_handler(int signal) {
    char buffer[1000];

    char * sigstr = strsignal(signal);
    snprintf(buffer, 1000, "error_signal_handler() SIGNALLED: %s\n", sigstr);
    fprintf(stdout, "%s", buffer);
    fprintf(stderr, "%s", buffer);
    fflush(stdout);
    fflush(stderr);

    // while(1) {
    // 	//man::tts::say(IN_GAME, "g d b me");
    // 	sleep(10);
    // }

    // cleanup();

    abort();
}

// Deal with lock file. To ensure that we only have ONE instance of man
void establishLock()
{
    lockFD = open("/home/nao/nbites/nbites.lock", O_CREAT | O_RDWR, 0666);
    if (lockFD < 0) {
        int err = errno;
        std::cout << "Could not open lockfile" << std::endl;
        std::cout << "Errno is: " << err << std::endl;
        exit(0);
    }

    int result = flock(lockFD, LOCK_EX | LOCK_NB);
    if (result == -1) {
        std::cout << "Could not establish lock on lock file. Is man already running?" << std::endl;
        exit(0);
    }
}

int main() {
    signal(SIGTERM, handler);
    signal(SIGINT, handler);

    establishLock();

    signal(SIGSEGV, error_signal_handler);

    printf("forking for whistle...\n");
//    whistlePID = whistle::start_whistle_process();

    printf("\t\tCOMPILED WITH BOSS VERSION == %d\n", BOSS_VERSION);
    
    //it is somewhat important that we write to the old file descriptors before reopening.
    //this copies some stdout buffer behavior to the new file description.
    printf("Man re-opening stdout...\n");
    fprintf(stderr, "Man re-opening stderr...\n");

    //Make stdout's fd point to a file description for the manlog file (MAN_LOG_PATH)
    freopen(MAN_LOG_PATH, "w", stdout);

    //Send stderr to whatever stdout's fd describes
    dup2(STDOUT_FILENO, STDERR_FILENO);
    
    //fprintf(stderr, "THIS IS A TEST OF STDERR\n");
    //fprintf(stdout, "THIS IS A TEST OF STDOUT\n");
    
    // Constructs an instance of man. If we get here we have a lock
    instance = new man::Man();

    while (1) {
        // Hack so that I don't have to modify DiagramThread
        // (Diagram threads are daemon threads, and man will exit if they're the
        // only ones left)
        sleep(10);

        int status;
        // clear zombie mans
        while ((waitpid(-1, &status, WNOHANG|WUNTRACED)) > 0);
    }

    return 1;
}
