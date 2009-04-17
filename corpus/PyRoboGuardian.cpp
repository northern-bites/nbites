
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


// PyRoboGuardian.cpp : Defines the exported functions for the python
//                      interpreter.
//

#include "PyRoboGuardian.h"

#include <Python.h>
#include <boost/python.hpp>
#include <boost/python/object.hpp>
#include <boost/python/module.hpp>
#include <boost/python/args.hpp>
using namespace std;
using namespace boost::python;

#include <boost/shared_ptr.hpp>
using boost::shared_ptr;

#include "RoboGuardian.h"

shared_ptr<RoboGuardian> roboguardian_pointer;

// Note the use of "class_<RoboGuardian, shared_ptr<RoboGuardian> >".
// It means that Python will be happy to treat shared pointers to RoboGuardin
// as real objects rather than pointers and it will not encroach on the shared
// pointer's built-in reference counter.
BOOST_PYTHON_MODULE(_roboguardian)
{
    class_<RoboGuardian, shared_ptr<RoboGuardian> >("RoboGuardian", no_init)
        .def("executeShutdownAction", &RoboGuardian::executeShutdownAction)
        .def("executeStartupAction", &RoboGuardian::executeStartupAction)
        .def("speakIPAddress", &RoboGuardian::speakIPAddress)
        .def("isRobotFalling", &RoboGuardian::isRobotFalling)
        .def("isRobotFallen", &RoboGuardian::isRobotFallen)
        .def("enableFallProtection", &RoboGuardian::enableFallProtection)
        ;

    scope().attr("roboguardian") = roboguardian_pointer;
}


void c_init_roboguardian () {
    if (!Py_IsInitialized())
        Py_Initialize();

    try {
        init_roboguardian();
    } catch (error_already_set) {
        PyErr_Print();
    }

}

void set_guardian_pointer (shared_ptr<RoboGuardian> guardian_ptr) {
    roboguardian_pointer = guardian_ptr;
}
