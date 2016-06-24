/**
 * Specified in http://doc.aldebaran.com/1-14/dev/cpp/tutos/create_module.html
 **/

#include <alcommon/almodule.h>
#include <alcommon/albroker.h>
#include <alcommon/albrokermanager.h>

#include <stdio.h>

#include "Boss.h"

extern "C"
{
    int _createModule(boost::shared_ptr<AL::ALBroker> broker)
    {
        // init broker with the main broker instance
        // from the parent executable
        AL::ALBrokerManager::setInstance(broker->fBrokerManager.lock());
        AL::ALBrokerManager::getInstance()->addBroker(broker);
        // create module instances
        printf("Creating module\n");
        AL::ALModule::createModule<boss::Boss>(broker, "nbites-boss");
        printf("Successfully created module!\n");
        return 0;
    }

    int _closeModule()
    {
        printf("CLOSING MODULE\n");
        return 0;
    }
}
