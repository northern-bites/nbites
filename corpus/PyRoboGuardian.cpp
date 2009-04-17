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

void set_guardian_pointer (shared_ptr<RoboGuardian> _interface) {
    roboguardian_pointer = _interface;
}
