/**
 * Specified in http://www.aldebaran-robotics.com/documentation/dev/cpp/tutos/create_module.html#how-to-create-a-local-module
 **/

//#include "Woman.h"
#include <alcommon/almodule.h>
#include <alcommon/albroker.h>
#include <alcommon/albrokermanager.h>

#include <stdio.h>

extern "C"
{
    int _createModule(boost::shared_ptr<AL::ALBroker> broker)
    {
        // init broker with the main broker instance
        // from the parent executable
        AL::ALBrokerManager::setInstance(broker->fBrokerManager.lock());
        AL::ALBrokerManager::getInstance()->addBroker(broker);
        // create module instances
        //AL::ALModule::createModule<man::Man>(broker, "nbitesman");
        printf("NEW NAOQI MODULE!\n");
        return 0;
    }

    int _closeModule()
    {
        return 0;
    }
}

// int main() {
//     printf("IM BEING EXECUTED ON A ROBOT!!!!!!!!!\n");
//     return 1;
// }
