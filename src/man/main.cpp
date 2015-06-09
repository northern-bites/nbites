/**
 * Specified in http://www.aldebaran-robotics.com/documentation/dev/cpp/tutos/create_module.html#how-to-create-a-local-module
 **/

#include "Man.h"

man::Man* instance;

void handler(int signal)
{
    if (signal == SIGTERM)
    {
        // Give man a chance to clean up behind it
        delete instance;
        exit(0);
    }
}

int main() {

    signal(SIGTERM, handler);

    // Constructs an instance of man
    instance = new man::Man();

    while (1) {
        std::cout << "In main!" << std::endl;
        sleep(5);
    }
    return 1;
}
