
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

#include <Python.h>
#include <boost/python.hpp>
using namespace boost::python;

#include <string>

#include <boost/shared_ptr.hpp>
using boost::shared_ptr;

#include "PyLights.h"


shared_ptr<Lights> lights_pointer;

void (Lights::*setRGB1)(const std::string led_id, const int rgbHex) =
     &Lights::setRGB;
void (Lights::*setRGB2)(const unsigned int led_id, const int rgbHex) =
    &Lights::setRGB;

BOOST_PYTHON_MODULE(_lights)
{
    class_<Lights, shared_ptr<Lights> >("Lights", no_init)
         .def("setRGB", setRGB1)
        .def("setRGB", setRGB2)
        ;

    scope().attr("lights") = lights_pointer;
}

