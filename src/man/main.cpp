/**
 * Specified in http://www.aldebaran-robotics.com/documentation/dev/cpp/tutos/create_module.html#how-to-create-a-local-module
 **/

#include "Man.h"

// #include <alcommon/albrokermanager.h>

// extern "C"
// {
//     int _createModule(boost::shared_ptr<AL::ALBroker> broker)
//     {
//         // init broker with the main broker instance
//         // from the parent executable
//         AL::ALBrokerManager::setInstance(broker->fBrokerManager.lock());
//         AL::ALBrokerManager::getInstance()->addBroker(broker);
//         // create module instances
//         AL::ALModule::createModule<man::Man>(broker, "nbitesman");
//         return 0;
//     }

//     int _closeModule()
//     {
//         return 0;
//     }
// }

int main() {
    // Constructs an instance of man
    man::Man man; // lol man man man
//    while (1) pause();
    while (1) {
        std::cout << "In main!" << std::endl;
        sleep(5);
    }
    return 1;
}
