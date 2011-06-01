#include <Python.h>

#include <boost/shared_ptr.hpp>
using boost::shared_ptr;

#include <boost/python.hpp>
using namespace boost::python;
using namespace std;

#include "PyVision.h"

shared_ptr<Vision> vision_pointer;

BOOST_PYTHON_MODULE(vision)
{
  class_<VisualBall>("Ball", no_init)
    // Note that getters are in VisualDetection, parent to VisualBall
    .def_readonly("centerX", &VisualBall::getCenterX)
    .def_readonly("centerY", &VisualBall::getCenterY)
    .def_readonly("width", &VisualBall::getWidth)
    .def_readonly("height", &VisualBall::getHeight)
    .def_readonly("focDist", &VisualBall::getFocDist)
    .def_readonly("dist", &VisualBall::getDistance)
    .def_readonly("bearing", &VisualBall::getBearing)
    .def_readonly("angleX", &VisualBall::getAngleXDeg)
    .def_readonly("angleY", &VisualBall::getAngleYDeg)
    .def_readonly("elevation", &VisualBall::getElevationDeg)
    // From VisualBall itself
    .def_readonly("confidence", &VisualBall::getConfidence)
    .def_readonly("radius", &VisualBall::getRadius)
    ;

  class_<VisualFieldObject>("FieldObject", no_init)
    // From VisualDetection
    .def_readonly("centerX", &VisualBall::getCenterX)
    .def_readonly("centerY", &VisualBall::getCenterY)
    .def_readonly("width", &VisualBall::getWidth)
    .def_readonly("height", &VisualBall::getHeight)
    .def_readonly("focDist", &VisualBall::getFocDist)
    .def_readonly("dist", &VisualBall::getDistance)
    .def_readonly("bearing", &VisualBall::getBearing)
    .def_readonly("angleX", &VisualBall::getAngleXDeg)
    .def_readonly("angleY", &VisualBall::getAngleYDeg)
    ;

  //noncopyable is required because vision has no public copy constructor
  class_<Vision, shared_ptr<Vision>, boost::noncopyable >("Vision", no_init)
    //make_getter provides a getter for objects not pointers
    .add_property("ball", make_getter(&Vision::ball, return_value_policy<reference_existing_object>()))
    .add_property("yglp", make_getter(&Vision::yglp, return_value_policy<reference_existing_object>()))
    ;

  scope().attr("vision") = vision_pointer;
}

void c_init_vision() {
  if (!Py_IsInitialized())
    Py_Initialize();
  try{
    initvision();
  } catch (error_already_set) {
    PyErr_Print();
  }
}

void set_vision_pointer (shared_ptr<Vision> visionptr) {
  vision_pointer = visionptr;
}






