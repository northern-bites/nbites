
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


// PySensors.cpp : Defines the exported functions for the python
//                 interpreter.
//

#include <Python.h>
#include <vector>

#include <boost/python.hpp>
#include <boost/python/suite/indexing/vector_indexing_suite.hpp>
using namespace boost::python;

#include <boost/shared_ptr.hpp>
using boost::shared_ptr;

#include "Sensors.h"
#include "PySensors.h"

shared_ptr<Sensors> sensors_pointer;

// We want to be able to save frames, resetsaveframes
BOOST_PYTHON_MODULE(sensors)
{
    class_<FSR>("FSR", no_init)
        .def_readonly("frontLeft", &FSR::frontLeft)
        .def_readonly("frontRight", &FSR::frontRight)
        .def_readonly("rearLeft", &FSR::rearLeft)
        .def_readonly("rearRight", &FSR::rearRight)
        ;

    class_<FootBumper>("FootBumper", no_init)
        .def_readonly("left", &FootBumper::left)
        .def_readonly("right", &FootBumper::right)
        ;

    class_<Inertial>("Inertial", no_init)
        .def_readonly("accX", &Inertial::accX)
        .def_readonly("accY", &Inertial::accY)
        .def_readonly("accZ", &Inertial::accZ)
        .def_readonly("gyrX", &Inertial::gyrX)
        .def_readonly("gyrY", &Inertial::gyrY)
        .def_readonly("angleX", &Inertial::angleX)
        .def_readonly("angleY", &Inertial::angleY)
        ;

    // Wrapper for std::vector which will allow us to access angle vectors
    // from Sensors.
    class_< std::vector<float> >("AngleVector")
        .def(vector_indexing_suite< std::vector<float> >())
        ;

    class_<Sensors, shared_ptr<Sensors> >("Sensors", no_init)
        //All the properties lack a setter. The values should be read-only
        //NOTE: all sensor values in Python should be in degrees and centimeters
        .add_property("angles", &Sensors::getBodyAngles_degs)
        .add_property("motionAngles",&Sensors::getMotionBodyAngles_degs)
        .add_property("leftFootFSR", &Sensors::getLeftFootFSR)
        .add_property("rightFootFSR", &Sensors::getRightFootFSR)
        .add_property("leftFootBumper", &Sensors::getLeftFootBumper)
        .add_property("rightFootBumper", &Sensors::getRightFootBumper)
        .add_property("inertial", &Sensors::getInertial_degs)
        .add_property("ultraSoundDistanceLeft", &Sensors::getUltraSoundLeft_cm)
        .add_property("ultraSoundDistanceRight", &Sensors::getUltraSoundRight_cm)
        .add_property("chestButton", &Sensors::getChestButton)
        .add_property("batteryCharge", &Sensors::getBatteryCharge)
        .add_property("batteryCurrent", &Sensors::getBatteryCurrent)

        .def("saveFrame", &Sensors::saveFrame)
        .def("resetSaveFrame", &Sensors::resetSaveFrame)
        .def("startSavingFrames", &Sensors::startSavingFrames)
        .def("stopSavingFrames", &Sensors::stopSavingFrames)
        .def("writeVarianceData", &Sensors::writeVarianceData)
        ;

    scope().attr("sensors") = sensors_pointer;
}

void c_init_sensors () {
    if (!Py_IsInitialized())
        Py_Initialize();

    try {
        initsensors();
    } catch (error_already_set) {
        PyErr_Print();
    }

}

void set_sensors_pointer (shared_ptr<Sensors> sensors_ptr) {
    sensors_pointer = sensors_ptr;
}
