/**
 * Specified in http://www.aldebaran-robotics.com/documentation/dev/cpp/tutos/create_module.html#how-to-create-a-local-module
 **/

#include "NewMan.h"

#include <boost/shared_ptr.hpp>
#include <alcommon/albroker.h>
#include <alcommon/albrokermanager.h>

extern "C"
{
    int _createModule(boost::shared_ptr<AL::ALBroker> broker)
    {
        // init broker with the main broker instance
        // from the parent executable
        AL::ALBrokerManager::setInstance(broker->fBrokerManager.lock());
        AL::ALBrokerManager::getInstance()->addBroker(broker);
        // create module instances
        AL::ALModule::createModule<Man>(broker, "ManModule");
        return 0;
    }

    int _closeModule()
    {
        return 0;
    }
}
