#include "LocalizationModule.h"

namespace man
{
    namespace localization
    {
	LocalizationModule::LocalizationModule()
	    : Module(), output(base())
	{

	}

	LocalizationModule::~LocalizationModule()
	{

	}

	void LocalizationModule::run_()
	{
	    // @todo pull information from (1) MotionModule 
	    //       and (2) VisionModule. 
	    updateLocalization(/* @todo */);
	}
    }
}
