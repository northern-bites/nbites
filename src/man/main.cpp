/**
 * Specified in http://www.aldebaran-robotics.com/documentation/dev/cpp/tutos/create_module.html#how-to-create-a-local-module
 **/

#include "Man.h"

#include <sys/file.h>
#include <errno.h>

int lockFD = 0;
man::Man* instance;
const char * MAN_LOG_PATH = "/home/nao/nbites/log/manlog";

void handler(int signal)
{
    if (signal == SIGTERM)
    {
        // Give man a chance to clean up behind it
        // I.e. close camera driver gracefully
        instance->preClose();
        flock(lockFD, LOCK_UN);
        
        printf("man closing MAN_LOG_PATH...\n");
        fclose(stdout);
        
        delete instance;
        exit(0);
    }
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
    
    printf("\t\tman 7/6\n");
    printf("Man re-opening stdout...\n");
    freopen(MAN_LOG_PATH, "w", stdout);

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
