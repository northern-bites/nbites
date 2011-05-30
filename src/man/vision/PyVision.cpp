#include <Python.h>

#include <boost/shared_ptr.hpp>
using boost::shared_ptr;

#include <boost/python.hpp>
using namespace boost::python;

#include "PyVision.h"

shared_ptr<Vision> vision_pointer;

BOOST_PYTHON_MODULE(_vision)
{
  class_<VisualBall>("Ball", no_init)
    .def_readonly("width", &VisualBall::getWidth)
    .def_readonly("height", &VisualBall::getHeight)
    ;

  //noncopyable is required because vision has no public copy constructor
  class_<Vision, shared_ptr<Vision>, boost::noncopyable >("Vision", no_init)
    .add_property("ball", &Vision::ball)
    ;

  scope().attr("vision") = vision_pointer;
}

void c_init_vision() {
  if (!Py_IsInitialized())
    Py_Initialize();
  try{
    init_vision();
  } catch (error_already_set) {
    PyErr_Print();
  }
}

void set_vision_pointer (shared_ptr<Vision> visionptr) {
  vision_pointer = visionptr;
}






