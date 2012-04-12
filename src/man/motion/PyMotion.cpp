
// This file is part of Man, a robotic perception, locomotion, and
// team strategy application created by the Northern Bites RoboCup
// team of Bowdoin College in Brunswick, Maine, for the Aldebaran
// Nao robot.
//
// Man is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Man is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser Public License for more details.
//
// You should have received a copy of the GNU General Public License
// and the GNU Lesser Public License along with Man.  If not, see
// <http://www.gnu.org/licenses/>.

/**
 * This file creates wraps the interface of our motion engine using
 * boost::python.
 *
 * @see PyMotionClasses.h
 * @see PyMotionCommand.h
 */

#define BOOST_PYTHON_MAX_ARITY 28

#include <Python.h>
#include <boost/python.hpp>
#include <boost/python/object.hpp>
#include <boost/python/module.hpp>
#include <boost/python/args.hpp>
#include <boost/shared_ptr.hpp>
using namespace std;
using namespace boost::python;

#include "PyMotionCommand.h"
#include "PyMotionClasses.h"

/**
 * If you want to expose an overloaded function to python, you have to create
 * pointers to each method which has a different signature and then add each
 * of these separately to the module.
 */
void (PyMotionInterface::*enq1)(const PyHeadJointCommand*) =
    &PyMotionInterface::enqueue;
void (PyMotionInterface::*enq2)(const PyBodyJointCommand*) =
    &PyMotionInterface::enqueue;

void (PyMotionInterface::*frz1)(const PyFreezeCommand*) =
    &PyMotionInterface::sendFreezeCommand;
void (PyMotionInterface::*frz2)(const PyUnfreezeCommand*) =
    &PyMotionInterface::sendFreezeCommand;


BOOST_PYTHON_MODULE(_motion)
{
    // generic interface available to all PyMotionCommands
    class_<PyMotionCommand, boost::noncopyable>("PyMotionCommand", no_init)
    .def("framesRemaining", &PyMotionCommand::framesRemaining)
    .def("isDone", &PyMotionCommand::isDone) // @TODO make just "done"; more Pythonic
    .def("timeRemaining", &PyMotionCommand::timeRemaining)
    ;

    class_<PyHeadJointCommand, bases<PyMotionCommand> >("HeadJointCommand",
                            init<float, tuple, tuple, int>(
                                "A container for a head joint command passed to the motion engine"))
        ;
    class_<PyGaitCommand, bases<PyMotionCommand> >("GaitCommand",
                           init<
                           tuple, tuple, tuple, tuple,
                           tuple, tuple, tuple, tuple>
                           (args("stance,step,zmp,joint_hack,sensor,"
                             "stiffness,odo,arms"),
                            "Parameterization of the"
                            " walk engine"))
        .def("getStepValue", &PyGaitCommand::getStepValue)
    ;

    class_<PyBodyJointCommand, bases<PyMotionCommand> >("BodyJointCommand",
                            init<float, tuple, tuple, tuple,
                            tuple, tuple, int>(
                                "A container for a body joint command passed to the motion engine"))
    .def(init<float, int, tuple, tuple, int>( // Single chain command
         args("time","chainID", "joints","body_stiffness","interpolation"),
         "A container for a body joint command passed to the motion engine"))
    ;
    class_<PySetHeadCommand, bases<PyMotionCommand> >("SetHeadCommand",
                              init<float, float>(args("yaw", "pitch"),
                                     "A container for a set head command. Holds yaw and pitch angles in degrees."))
    .def(init<float,float,float,float>(args("yaw","pitch",
                        "maxYawSpeed","maxPitchSpeed")))
        ;

    class_<PyCoordHeadCommand, bases<PyMotionCommand> >("CoordHeadCommand",
                            init<float, float, float>
                            (args("relX", "relY", "relZ"),
                             "A container for a coord head command."))
    .def(init<float, float, float, float, float> (
         args( "relX", "relY", "relZ", "maxYawSpeed", "maxPitchSpeed")))
    .def(init<float, float, float> (
         args( "relX", "relY", "relZ")))
    ;

    class_<PyWalkCommand, bases<PyMotionCommand> >("WalkCommand",
                           init<float, float, float>(args("x","y","theta"),
                                         "A container for a walk command. Holds an x, y and theta which represents a"
                                         " walk vector"))
        ;

    class_<PyDestinationCommand, bases<PyMotionCommand> >("DestinationCommand",
                              init<float, float, float, float>
                              (args("x","y","theta","gain"),
                               "A container for a destination command. Holds an x, y and theta which represent a"
                               " destination relative to the origin of the robot, and a speed (gain)"))
        ;

    class_<PyStepCommand, bases<PyMotionCommand> >("StepCommand",
                           init<float, float, float, int>(args("x","y","theta",
                                               "numSteps"),
                                          "A container for a step command. ""Holds an x, y and theta which represents a"
                                          " walk vector, in addition to the number of desired steps."))
        ;

    class_<PyFreezeCommand, bases<PyMotionCommand> >("FreezeCommand",
                             init<>("A container for a "
                                "command to freeze the robot"))
        ;

    class_<PyUnfreezeCommand, bases<PyMotionCommand> >("UnfreezeCommand",
                               init<float>(args("stiffness"),
                                   "A container for a command to "
                                   "UNfreeze the robot"))

        ;
    class_<PyMotionInterface>("MotionInterface")
        .def("enqueue", enq1)
        .def("enqueue", enq2)
        .def("setNextWalkCommand", &PyMotionInterface::setNextWalkCommand)
        .def("sendStepCommand", &PyMotionInterface::sendStepCommand)
        .def("sendDestCommand", &PyMotionInterface::sendDestCommand)
        .def("setGait", &PyMotionInterface::setGait)
        .def("setHead", &PyMotionInterface::setHead)
        .def("coordHead", &PyMotionInterface::coordHead)
        .def("sendFreezeCommand",frz1)
        .def("sendFreezeCommand",frz2)
        .def("isWalkActive", &PyMotionInterface::isWalkActive)
        .def("isHeadActive", &PyMotionInterface::isHeadActive)
        .def("isBodyActive", &PyMotionInterface::isBodyActive)
        .def("stopBodyMoves", &PyMotionInterface::stopBodyMoves)
        .def("stopHeadMoves", &PyMotionInterface::stopHeadMoves)
        .def("resetWalk", &PyMotionInterface::resetWalkProvider)
        .def("resetScripted", &PyMotionInterface::resetScriptedProvider)
        ;
}


void c_init_motion () {
    if (!Py_IsInitialized())
        Py_Initialize();

    try {
        init_motion();
    } catch (error_already_set) {
        PyErr_Print();
    }

}

void set_motion_interface (MotionInterface *_interface)
{
    interface_reference = _interface;
}

