/**
 * Specified in http://www.aldebaran-robotics.com/documentation/dev/cpp/tutos/create_module.html#how-to-create-a-local-module
 **/

#include "Man.h"
#include "SharedData.h"


#include <sys/file.h>
#include <errno.h>
#include <unistd.h>

int lockFD = 0;
pid_t whistlePID = 0;
man::Man* instance;

//const char * MAN_LOG_PATH = "/home/nao/nbites/log/manlog";
const char * MAN_LOG_PATH = "/home/nao/nbites/log/manlog";

void cleanup() {
    instance->preClose();
    flock(lockFD, LOCK_UN);

    if (whistlePID > 0) {
        kill(whistlePID, SIGTERM);
    }

    printf("Man closing output streams...\n");
    fflush(stderr);
    fflush(stdout);
    fclose(stdout);

    if (instance) delete instance;
}

void handler(int signal)
{
    if (signal == SIGTERM)
    {

        // Give man a chance to clean up behind it
        // I.e. close camera driver gracefully
        cleanup();
        exit(0);
    }
}

void error_signal_handler(int signal) {
    char buffer[1000];

    char * sigstr = strsignal(signal);
    snprintf(buffer, 1000, "error_signal_handler() SIGNALLED: %s\n", sigstr);
    fprintf(stdout, "%s", buffer);
    fprintf(stderr, "%s", buffer);
    fflush(stdout);
    fflush(stderr);

    cleanup();

    printf("error_signal_handler() done.\n");
    exit(-1);
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

    establishLock();

    signal(SIGSEGV, error_signal_handler);

    printf("forking for whistle...\n");
    whistlePID = fork();
    if (whistlePID == 0) {
         execl("/home/nao/whistle", "", NULL);
    }

    printf("\t\tCOMPILED WITH BOSS VERSION == %d\n", BOSS_VERSION);
    
    //it is somewhat important that we write to the old file descriptors before reopening.
    //this copies some stdout buffer behavior to the new file description.
    printf("Man re-opening stdout...\n");
    fprintf(stderr, "Man re-opening stderr...\n");

    //Make stdout's fd point to a file description for the manlog file (MAN_LOG_PATH)
//    freopen(MAN_LOG_PATH, "w", stdout);
     freopen(MAN_LOG_PATH, "wa", stdout);
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
    }
    return 1;
}
