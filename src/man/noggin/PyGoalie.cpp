/*
 * PyGoalie interface
 */

#include "PyGoalie.h"

#include <boost/python.hpp>

using namespace boost::python;
using namespace noggin;

BOOST_PYTHON_MODULE(goalie)
{
    class_<GoalieSystem>("GoalieSystem")
        .def("pushRightPostObservation",
             &GoalieSystem::pushRightPostObservation)
        .def("pushLeftPostObservation",
             &GoalieSystem::pushLeftPostObservation)
        .def("pushCrossObservation", &GoalieSystem::pushCrossObservation)
        .def("resetPosts", &GoalieSystem::resetPosts)
        .def("leftPostBearing", &GoalieSystem::leftPostBearing)
        .def("rightPostBearing", &GoalieSystem::rightPostBearing)
        .def("leftPostDistance", &GoalieSystem::leftPostDistance)
        .def("rightPostDistance", &GoalieSystem::rightPostDistance)
        .def("crossBearing", &GoalieSystem::crossBearing)
        .def("leftPostRelX", &GoalieSystem::leftPostRelX)
        .def("rightPostRelX", &GoalieSystem::rightPostRelX)
        .def("leftPostRelY", &GoalieSystem::leftPostRelY)
        .def("rightPostRelY", &GoalieSystem::rightPostRelY)
        .def("centerGoalDistance", &GoalieSystem::centerGoalDistanceAvg)
        .def("centerGoalBearing", &GoalieSystem::centerGoalBearingAvg)
        .def("centerGoalRelX", &GoalieSystem::centerGoalRelX)
        .def("centerGoalRelY", &GoalieSystem::centerGoalRelY)
        .add_property("home", &GoalieSystem::home)
        ;

    scope().attr("RIGHT_SIDE_ANGLE") = RIGHT_SIDE_ANGLE;
    scope().attr("LEFT_SIDE_ANGLE") = LEFT_SIDE_ANGLE;
    scope().attr("RIGHT_SIDE_RP_ANGLE") = RIGHT_SIDE_RP_ANGLE;
    scope().attr("RIGHT_SIDE_LP_ANGLE") = RIGHT_SIDE_LP_ANGLE;
    scope().attr("LEFT_SIDE_LP_ANGLE") = LEFT_SIDE_LP_ANGLE;
    scope().attr("LEFT_SIDE_RP_ANGLE") = LEFT_SIDE_RP_ANGLE;
    scope().attr("RIGHT_SIDE_RP_DISTANCE") = RIGHT_SIDE_RP_DISTANCE;
    scope().attr("RIGHT_SIDE_LP_DISTANCE") = RIGHT_SIDE_LP_DISTANCE;
    scope().attr("LEFT_SIDE_LP_DISTANCE") = LEFT_SIDE_LP_DISTANCE;
    scope().attr("LEFT_SIDE_RP_DISTANCE") = LEFT_SIDE_RP_DISTANCE;

}

void c_init_goalie() {
    if (!Py_IsInitialized())
        Py_Initialize();
    try{
        initgoalie();
    } catch (error_already_set) {
        PyErr_Print();
    }
}




