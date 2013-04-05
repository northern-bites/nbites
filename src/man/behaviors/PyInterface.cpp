/*
  This file contains the boost interface for accessing messages
  from python.
*/

#include "PyInterface.h"
#include <boost/python.hpp>

#include <iostream>

using namespace boost::python;
namespace man {
namespace behaviors {

boost::shared_ptr<PyInterface> interface_ptr;


void set_interface_ptr(boost::shared_ptr<PyInterface> ptr)
{
    interface_ptr = ptr;
}

BOOST_PYTHON_MODULE(interface)
{
    class_<PyInterface, boost::shared_ptr<PyInterface>, boost::noncopyable>("Interface", no_init)
        .add_property("gameState",
                      make_getter(&PyInterface::gameState_ptr,
                                  return_value_policy
                                  <reference_existing_object>()))
        .add_property("visionBall",
                      make_getter(&PyInterface::visionBall_ptr,
                                  return_value_policy
                                  <reference_existing_object>()))
        .add_property("visionRobot",
                      make_getter(&PyInterface::visionRobot_ptr,
                                  return_value_policy
                                  <reference_existing_object>()))
        .add_property("visionField",
                      make_getter(&PyInterface::visionField_ptr,
                                  return_value_policy
                                  <reference_existing_object>()))
        .add_property("filteredBall",
                      make_getter(&PyInterface::filteredBall_ptr,
                                  return_value_policy
                                  <reference_existing_object>()))
        .add_property("ledCommand",
                      make_getter(&PyInterface::ledCommand_ptr,
                                  return_value_policy
                                  <reference_existing_object>()))
        .def("worldModelList", &PyInterface::getWorldModelList)
        .add_property("motionCommand",
                      make_getter(&PyInterface::motionCommand_ptr,
                                  return_value_policy
                                  <reference_existing_object>()))
        .add_property("motionStatus",
                      make_getter(&PyInterface::motionStatus_ptr,
                                  return_value_policy
                                  <reference_existing_object>()))
        .add_property("sonarState",
                      make_getter(&PyInterface::sonarState_ptr,
                                  return_value_policy
                                  <reference_existing_object>()))
        .add_property("footBumperState",
                      make_getter(&PyInterface::footBumperState_ptr,
                                  return_value_policy
                                  <reference_existing_object>()))
		;
	scope().attr("interface") = interface_ptr;
}
}
}
