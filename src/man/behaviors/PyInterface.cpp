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
        .add_property("loc",
                      make_getter(&PyInterface::loc_ptr,
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
        .add_property("bodyMotionCommand",
                      make_getter(&PyInterface::bodyMotionCommand_ptr,
                                  return_value_policy
                                  <reference_existing_object>()))
        .add_property("headMotionCommand",
                      make_getter(&PyInterface::headMotionCommand_ptr,
                                  return_value_policy
                                  <reference_existing_object>()))
        .add_property("motionStatus",
                      make_getter(&PyInterface::motionStatus_ptr,
                                  return_value_policy
                                  <reference_existing_object>()))
        .add_property("motionRequest",
                      make_getter(&PyInterface::motionRequest_ptr,
                                  return_value_policy
                                  <reference_existing_object>()))
        .add_property("odometry",
                      make_getter(&PyInterface::odometry_ptr,
                                  return_value_policy
                                  <reference_existing_object>()))
        .add_property("joints",
                      make_getter(&PyInterface::joints_ptr,
                                  return_value_policy
                                  <reference_existing_object>()))
        .add_property("fallStatus",
                      make_getter(&PyInterface::fallStatus_ptr,
                                  return_value_policy
                                  <reference_existing_object>()))
        .add_property("resetLocRequest",
                      make_getter(&PyInterface::resetLocRequest_ptr,
                                  return_value_policy
                                  <reference_existing_object>()))
        .add_property("myWorldModel",
                      make_getter(&PyInterface::myWorldModel_ptr,
                                  return_value_policy
                                  <reference_existing_object>()))
        .add_property("stiffStatus",
                      make_getter(&PyInterface::stiffStatus_ptr,
                                  return_value_policy
                                  <reference_existing_object>()))
        .add_property("obstacle",
                      make_getter(&PyInterface::obstacle_ptr,
                                  return_value_policy
                                  <reference_existing_object>()))
        .add_property("visionObstacle",
                      make_getter(&PyInterface::visionObstacle_ptr,
                                  return_value_policy
                                  <reference_existing_object>()))

        .add_property("sharedBall",
                      make_getter(&PyInterface::sharedBall_ptr,
                                  return_value_policy
                                  <reference_existing_object>()))


        ;
    scope().attr("interface") = interface_ptr;
}
}
}
