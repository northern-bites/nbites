/**
 * Naoqi module to create a static instance of man
 * Useful when loading man as a dynamic library
 *
 * @author The Northern Bites - Bowdoin College, Brunswick, ME
 *
**/

#pragma once

#include <boost/shared_ptr.hpp>
#include <alcommon/albroker.h>
#include <alcommon/almodule.h>

#include "TMan.h"

#include "include/ExportDefines.h"

START_FUNCTION_EXPORT

//This is what Aldebaran will call when it loads this module
//Note: this is the point of entry for our code
int _createModule(boost::shared_ptr<AL::ALBroker> pBroker);

//Aldebaran apparently never calls this - Octavian
int _closeModule();

END_FUNCTION_EXPORT

class NaoManLoader : public AL::ALModule {

public:
	NaoManLoader(boost::shared_ptr<AL::ALBroker> pBroker,
                const std::string& pName);
	virtual ~NaoManLoader();

public:
	boost::shared_ptr<TMan> man;

private:
    boost::shared_ptr<AL::ALBroker> broker;
};