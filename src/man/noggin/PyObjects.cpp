/*
 * Wrapper for Location.*. Provides a class for location computations done for
 * python.
 */

#include "PyObjects.h"

using namespace boost::python;
using namespace noggin;

//Overloaded function from MyInfo and its base class Location. Provides
// with proper pointers to these functions, for use in MyInfo
const float (MyInfo::*distTo1)(FieldObject&, bool) = &MyInfo::distTo;
const float (MyInfo::*distTo2)(const Location&) = &Location::distTo;

BOOST_PYTHON_MODULE(objects)
{
    class_<Location>("Location", init<float, float>())
        .add_property("x", &Location::getX, &Location::setX)
        .add_property("y", &Location::getY, &Location::setY)
        .def(self == self)
        .def(self != self)
        .def("toTupleXY", &Location::toTupleXY)
        .def("distTo", &Location::distTo)
        .def("headingTo", &Location::headingToInDeg)
        .def("inOppGoalBox", &Location::inOppGoalBox)
        .def("inMyGoalBox", &Location::inMyGoalBox)
        .def("inTopOfField", &Location::inTopOfField)
        .def("inBottomOfField", &Location::inBottomOfField)
        .def("inCenterOfField", &Location::inCenterOfField)
        ;

    class_<RobotLocation, bases<Location> >("RobotLocation",
                                            init<float, float, float>())
        .add_property("h", &RobotLocation::getH, &RobotLocation::setH)
        .def(self == self)
        .def(self != self)
        .def("getRelativeBearing", &RobotLocation::getRelativeBearing)
        .def("spinDirToPoint", &RobotLocation::spinDirToPoint)
        ;

    class_<RelLocation, bases<RobotLocation> >("RelLocation",
                                               init<RobotLocation&,
                                               float, float, float>())
        .add_property("relX", &RelLocation::getRelX, &RelLocation::setRelX)
        .add_property("relY", &RelLocation::getRelY, &RelLocation::setRelY)
        .add_property("relH", &RelLocation::getRelH, &RelLocation::setRelH)
        ;

    class_<LocObject, bases<Location> >("LocObject")
        .add_property("trackingFitness", &LocObject::getTrackingFitness,
                      &LocObject::setTrackingFitness)
        .def(self < self)
        .def(self > self)
        ;

    class_<FieldObject, bases<LocObject>, boost::noncopyable>
        ("FieldObject", init<VisualFieldObject&, py_constants::vis_landmark,
         MyInfo&>())
        .add_property("vis", make_getter(&FieldObject::vis, return_value_policy
                                         <reference_existing_object>()))
        .add_property("localId", &FieldObject::getLocalID)
        .add_property("visId", &FieldObject::getVisID)
        .add_property("relX", &FieldObject::getRelX)
        .add_property("relY", &FieldObject::getRelY)
        .add_property("dist", &FieldObject::getDist)
        .add_property("bearing", &FieldObject::getBearing)
        .add_property("locDist", &FieldObject::getLocDist)
        .add_property("locBearing", &FieldObject::getLocBearing)
        .def("setBest", &FieldObject::setBest)
        .def("associateWithRelativeLandmark",
             &FieldObject::associateWithRelativeLandmark)
        ;

    class_<MyInfo, bases<RobotLocation>,
           boost::noncopyable>("MyInfo", init<PyLoc&>())
        .def("update", &MyInfo::update)
        .def("distTo", distTo1)
        .def("distTo", distTo2)
        .add_property("teamColor", &MyInfo::getTeamColor, &MyInfo::setTeamColor)
        .add_property("locScoreTheta", &MyInfo::getLocScoreTheta)
        .add_property("locScoreXY", &MyInfo::getLocScoreXY)
        .add_property("locScore", &MyInfo::getLocScore)
        .add_property("playerNumber", &MyInfo::getPlayerNumber,
                      &MyInfo::setPlayerNumber)
        .add_property("uncertX", &MyInfo::getXUncert)
        .add_property("uncertY", &MyInfo::getYUncert)
        .add_property("uncertH", &MyInfo::getHUncert)
        ;
}

void c_init_objects() {
    if (!Py_IsInitialized())
        Py_Initialize();
    try{
        initobjects();
    } catch (error_already_set) {
        PyErr_Print();
    }
}


