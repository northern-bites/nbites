
#include <Python.h>
#include <boost/python.hpp>
using namespace boost::python;

#include <string>

#include <boost/shared_ptr.hpp>
using boost::shared_ptr;

#include "PySpeech.h"
#include "Speech.h"

shared_ptr<Speech> speech_pointer;

BOOST_PYTHON_MODULE(_speech)
{
    class_<Speech, shared_ptr<Speech> >("Speech", no_init)
        .def("say", &Speech::say)
        .add_property("volume", &Speech::getVolume, &Speech::setVolume)
        .def("enable", &Speech::enable)
        .def("disable", &Speech::disable)
        ;

    scope().attr("speech") = speech_pointer;
}

void c_init_speech () {
    if (!Py_IsInitialized())
        Py_Initialize();

    try {
        init_speech();
    } catch (error_already_set) {
        PyErr_Print();
    }

}

void set_speech_pointer (shared_ptr<Speech> _speech_ptr) {
    speech_pointer = _speech_ptr;
}

