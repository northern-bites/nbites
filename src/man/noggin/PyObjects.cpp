/*
 * Wrapper for CombinationObjects.*. See header for more information. Import
 * objects in python to use these classes. 
 */

#include "PyObjects.h"

using namespace boost::python;
using namespace noggin;

BOOST_PYTHON_MODULE(objects)
{
    class_<Location>("Location", init<float, float>())
        .add_property("x", &Location::getX, &Location::setX)
        .add_property("y", &Location::getY, &Location::setY)
        .def(self == self)
        .def(self != self)
        .def(self - self)
        .def("relativeLocationOf", &Location::getRelLocationOf)
        .def("toTupleXY", &Location::toTupleXY)
        .def("distTo", &Location::distTo)
        .def("headingTo", &Location::headingToInDeg)
        .def("inOppGoalBox", &Location::inOppGoalBox)
        .def("inMyGoalBox", &Location::inMyGoalBox)
        .def("inTopOfField", &Location::inTopOfField)
        .def("inBottomOfField", &Location::inBottomOfField)
        .def("inCenterOfField", &Location::inCenterOfField)
        .def("__hasattr__", &Location::hasattr)
        .def("__str__", &Location::toString)
        ;

    class_<RobotLocation, bases<Location> >("RobotLocation",
                                            init<float, float, float>())
        .add_property("h", &RobotLocation::getH, &RobotLocation::setH)
        .def(self == self)
        .def(self != self)
        .def(self - self)
        .def(self + other<RelRobotLocation>())
        .def("relativeRobotLocationOf", &RobotLocation::getRelLocationOf)
        .def("getRelativeBearing", &RobotLocation::getRelativeBearing)
        .def("__str__", &RobotLocation::toString)
        ;

    class_<RelLocation>("RelLocation", init<float, float>())
        .add_property("relX", &RelLocation::getRelX, &RelLocation::setRelX)
        .add_property("relY", &RelLocation::getRelY, &RelLocation::setRelY)
        .add_property("bearing", &RelLocation::getBearing)
        .add_property("dist", &RelLocation::getDist)
        .def("__str__", &RelLocation::toString)
        ;

    class_<RelRobotLocation, bases<RelLocation> >("RelRobotLocation",
                                                  init<float, float, float>())
        .add_property("relH", &RelRobotLocation::getRelH, &RelRobotLocation::setRelH)
        .def("__str__", &RelRobotLocation::toString)
        ;

    class_<LocObject, bases<Location> >("LocObject", init<PyLoc&>())
        .add_property("trackingFitness", &LocObject::getTrackingFitness,
                      &LocObject::setTrackingFitness)
        .def(self < self)
        .def(self > self)
        .add_property("dist", &LocObject::getDist)
        .add_property("bearing", &LocObject::getBearing)
        .add_property("relX", &LocObject::getRelX)
        .add_property("relY", &LocObject::getRelY)
        ;

    class_<FieldObject, boost::noncopyable>
        ("FieldObject", init<VisualFieldObject&, py_constants::vis_landmark,
         PyLoc&>())
        .add_property("vis", make_getter(&FieldObject::vis, return_value_policy
                                         <reference_existing_object>()))
        .add_property("loc", make_getter(&FieldObject::loc, return_value_policy
                                         <reference_existing_object>()))
        .add_property("localId", &FieldObject::getLocalID)
        .add_property("visId", &FieldObject::getVisID)
        .add_property("dist", &FieldObject::getDist)
        .add_property("bearing", &FieldObject::getBearing)
        .def("setBest", &FieldObject::setBest)
        .def("associateWithRelativeLandmark",
             &FieldObject::associateWithRelativeLandmark)
        .def("__hasattr__", &FieldObject::hasattr)
        ;

    class_<MyInfo, bases<RobotLocation>,
           boost::noncopyable>("MyInfo", init<PyLoc&>())
        .def("update", &MyInfo::update)
        .add_property("teamColor", &MyInfo::getTeamColor, &MyInfo::setTeamColor)
        .add_property("locScoreTheta", &MyInfo::getLocScoreTheta)
        .add_property("locScoreXY", &MyInfo::getLocScoreXY)
        .add_property("locScore", &MyInfo::getLocScore)
        .add_property("playerNumber", &MyInfo::getPlayerNumber,
                      &MyInfo::setPlayerNumber)
        .add_property("uncertX", &MyInfo::getXUncert)
        .add_property("uncertY", &MyInfo::getYUncert)
        .add_property("uncertH", &MyInfo::getHUncert)
        .def("spinDirToPoint", &RobotLocation::spinDirToPoint)
        ;

    class_<LocBall, bases<Location, RelLocation> >("LocBall", init<PyLoc&, MyInfo&>())
        .add_property("uncertX", &LocBall::getXUncert)
        .add_property("uncertY", &LocBall::getYUncert)
        .add_property("sd", &LocBall::getSD)
        .add_property("relX", &LocBall::getRelX)
        .add_property("relY", &LocBall::getRelY)
        .add_property("velX", &LocBall::getVelX)
        .add_property("velY", &LocBall::getVelY)
        .add_property("uncertVelX", &LocBall::getVelXUncert)
        .add_property("uncertVelY", &LocBall::getVelYUncert)
        .add_property("heading", &LocBall::getHeading)
        .add_property("relVelX", &LocBall::getRelVelX)
        .add_property("relVelY", &LocBall::getRelVelY)
        .add_property("accX", &LocBall::getAccX)
        .add_property("accY", &LocBall::getAccY)
        .add_property("dx", &LocBall::dX)
        .add_property("dy", &LocBall::dY)
        .add_property("endY", &LocBall::getEndY)
        .add_property("uncertAccX", &LocBall::getAccXUncert)
        .add_property("uncertAccY", &LocBall::getAccYUncert)
        .add_property("relAccX", &LocBall::getRelAccX)
        .add_property("relAccY", &LocBall::getRelAccY)
        .add_property("dist", &LocBall::getDist)
        .add_property("bearing", &LocBall::getBearing)
        ;

    class_<Ball, boost::noncopyable>("Ball", init<VisualBall&,
                                     PyLoc&, MyInfo&>())
        .add_property("vis", make_getter(&Ball::vis, return_value_policy
                                         <reference_existing_object>()))
        .add_property("loc", make_getter(&Ball::loc, return_value_policy
                                         <reference_existing_object>()))
        .add_property("dist", &Ball::getDist)
        .add_property("bearing", &Ball::getBearing)
        .def("update", &Ball::update)
        .def("__hasattr__", &Ball::hasattr)
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


