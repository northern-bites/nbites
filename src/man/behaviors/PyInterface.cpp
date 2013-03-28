/*
 This file contains the boost interface for accessing messages
 from python.
*/

#include "PyInterface.h"
#include <boost/python.hpp>

using namespace boost::python;
namespace man {
namespace behaviors {

BOOST_PYTHON_MODULE(interface)
{
	class_<PyInterface, boost::noncopyable>("Interface", no_init)
		.add_property("gameState", make_getter(&PyInterface::gameState_ptr,
											   return_value_policy
											   <reference_existing_object>()))
		;
}
}
}
