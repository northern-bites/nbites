#include <Python.h>

#include <boost/shared_ptr.hpp>
using boost::shared_ptr;

#include <boost/python.hpp>
#include <boost/python/suite/indexing/vector_indexing_suite.hpp>
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
    .def_readonly("dist", &VisualBall::getDistance)
    .def_readonly("bearing", &VisualBall::getBearingDeg)
    .def_readonly("angleX", &VisualBall::getAngleXDeg)
    .def_readonly("angleY", &VisualBall::getAngleYDeg)
    .def_readonly("elevation", &VisualBall::getElevationDeg)
    // From VisualBall itself
    .def_readonly("confidence", &VisualBall::getConfidence)
    .def_readonly("radius", &VisualBall::getRadius)
    ;

  class_<VisualFieldObject>("FieldObject", no_init)
    // From VisualDetection
    .def_readonly("centerX", &VisualFieldObject::getCenterX)
    .def_readonly("centerY", &VisualFieldObject::getCenterY)
    .def_readonly("width", &VisualFieldObject::getWidth)
    .def_readonly("height", &VisualFieldObject::getHeight)
    .def_readonly("dist", &VisualFieldObject::getDistance)
    .def_readonly("bearing", &VisualFieldObject::getBearingDeg)
    .def_readonly("angleX", &VisualFieldObject::getAngleXDeg)
    .def_readonly("angleY", &VisualFieldObject::getAngleYDeg)
    // From VisualLandmark
    .def_readonly("certainty", &VisualFieldObject::getIDCertaintyInt)
    .def_readonly("distCertainty", &VisualFieldObject::getDistanceCertaintyInt)
    ;

  // Currently unused, but fully avaliable to python if uncommented
  class_<VisualCrossbar>("Crossbar", no_init)
    // From VisualDetection
    .def_readonly("centerX", &VisualCrossbar::getCenterX)
    .def_readonly("centerY", &VisualCrossbar::getCenterY)
    .def_readonly("width", &VisualCrossbar::getWidth)
    .def_readonly("height", &VisualCrossbar::getHeight)
    .def_readonly("dist", &VisualCrossbar::getDistance)
    .def_readonly("bearing", &VisualCrossbar::getBearingDeg)
    .def_readonly("angleX", &VisualCrossbar::getAngleXDeg)
    .def_readonly("angleY", &VisualCrossbar::getAngleYDeg)
    .def_readonly("x", &VisualCrossbar::getX)
    .def_readonly("y", &VisualCrossbar::getY)
    .def_readonly("elevation", &VisualCrossbar::getElevationDeg)
    // From VisualCrossbar
    .def_readonly("shoot", &VisualCrossbar::shotAvailable)
    .def_readonly("leftOpening", &VisualCrossbar::getLeftOpening)
    .def_readonly("rightOpening", &VisualCrossbar::getRightOpening)
    ;

  //FieldLines: holds corner and line information
  class_<FieldLines, boost::shared_ptr<FieldLines> >("FieldLines", no_init)
    .def_readonly("numCorners", &FieldLines::getNumCorners)
    .def_readonly("numLines", &FieldLines::getNumLines)
    .add_property("linesList", &FieldLines::getActualLines)
    .add_property("cornersList", &FieldLines::getActualCorners)
    ;

  //FieldLines helper classes:/

  // FieldLines holds a list of shared_ptrs to VisualLines (linesList) 
  class_<std::vector<shared_ptr<VisualLine> > >("LineVec")
    // True is for NoProxy, since shared_ptrs don't need one
    .def(vector_indexing_suite<std::vector<shared_ptr<VisualLine> >, true>())
    ;

  class_<VisualLine, boost::shared_ptr<VisualLine> >("VisualLine", no_init)
    .def_readonly("angle", &VisualLine::getAngle)
    .def_readonly("avgWidth", &VisualLine::getAvgWidth)
    .def_readonly("bearing", &VisualLine::getBearing)
    .def_readonly("dist", &VisualLine::getDistance)
    .def_readonly("length", &VisualLine::getLength)
    .def_readonly("slope", &VisualLine::getSlope)
    .def_readonly("yInt", &VisualLine::getYIntercept)
    ;

  // FieldLines holds a list of VisualCorners (not pointers) (cornersList) 
  class_<std::list<VisualCorner> >("CornerList")
    .def("__iter__", boost::python::iterator<std::list<VisualCorner> >())
    ;

  class_<VisualCorner>("VisualCorner", no_init)
    // From VisualDetection
    .def_readonly("centerX", &VisualCorner::getCenterX)
    .def_readonly("centerY", &VisualCorner::getCenterY)
    .def_readonly("width", &VisualCorner::getWidth)
    .def_readonly("height", &VisualCorner::getHeight)
    .def_readonly("dist", &VisualCorner::getDistance)
    .def_readonly("bearing", &VisualCorner::getBearingDeg)
    .def_readonly("angleX", &VisualCorner::getAngleXDeg)
    .def_readonly("angleY", &VisualCorner::getAngleYDeg)
    .def_readonly("x", &VisualCorner::getX)
    .def_readonly("y", &VisualCorner::getY)
    .def_readonly("elevation", &VisualCorner::getElevationDeg)
    ;

  ///////MAIN VISION CLASS/////////  
  //noncopyable is required because vision has no public copy constructor
  class_<Vision, shared_ptr<Vision>, boost::noncopyable >("Vision", no_init)
    //make_getter provides a getter for objects not pointers
    .add_property("ball", make_getter(&Vision::ball, return_value_policy
				      <reference_existing_object>()))
    .add_property("yglp", make_getter(&Vision::yglp, return_value_policy
				      <reference_existing_object>()))
    .add_property("ygrp", make_getter(&Vision::ygrp, return_value_policy
				      <reference_existing_object>()))
    .add_property("bglp", make_getter(&Vision::bglp, return_value_policy
				      <reference_existing_object>()))
    .add_property("bgrp", make_getter(&Vision::bgrp, return_value_policy
				      <reference_existing_object>()))
    .add_property("fieldLines", make_getter(&Vision::fieldLines, 
					    return_value_policy
					    <return_by_value>()))

    /* Crossbars: not used right now, uncomment here and Brain.py to use
       in python
    .add_property("ygCrossbar", make_getter(&Vision::ygCrossbar, 
    return_value_policy<reference_existing_object>()))
    .add_property("bgCrossbar", make_getter(&Vision::bgCrossbar, 
    return_value_policy<reference_existing_object>()))
    */
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






