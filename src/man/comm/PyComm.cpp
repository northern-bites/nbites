/*
 * Wrapper for all comm infomation needed in Python.
 *
 * @author: Wils Dawson   @date 9/17/12
 */

#include <boost/shared_ptr.hpp>
using boost::shared_ptr;

#include <boost/python.hpp>
#include <boost/python/suite/indexing/vector_indexing_suite.hpp>
using namespace boost::python;
using namespace std;

#include "PyComm.h"

shared_ptr<Comm> comm_pointer;

BOOST_PYTHON_MODULE(comm)
{
    class_<GameData>("GameData", no_init)
        .def_readonly("currentState", &GameData::currentState)
        .def_readonly("firstHalf", &GameData::firstHalf)
        .def_readonly("myTeamColor", &GameData::myTeamColor)
        .def_readonly("ourKickoff", &GameData::ourKickoff)
        .def_readonly("secondaryState", &GameData::secondState)
        .def_readonly("dropInTeam", &GameData::dropInTeam)
        .def_readonly("dropInTime", &GameData::dropInTime)
        .def_readonly("timeRemaining", &GameData::timeRemaining)
        .def_readonly("ourScore", &GameData::ourScore)
        .def_readonly("theirScore", &GameData::theirScore)
        .def_readonly("goalDifferential", &GameData::goalDifferential)
        .def_readonly("numOurPenalizedRobots", &GameData::numOurPenalizedRobots)
        .def_readonly("numTheirPenalizedRobots", &GameData::numTheirPenalizedRobots)
        .def_readonly("timeUntilOurNextUnpened", &GameData::timeUntilOurNextUnpened)
        .def_readonly("timeUntilTheirNextUnpened", &GameData::timeUntilTheirNextUnpened)
        .def_readonly("timeUntilWeFullStrength", &GameData::timeUntilWeFullStrength)
        .def_readonly("timeUntilTheyFullStrength", &GameData::timeUntilTheyFullStrength)
        .def("timeUntilOurPlayerUnpened", &GameData::timeUntilOurPlayerUnpened)
        .def("timeUntilTheirPlayerUnpened", &GameData::timeUntilTheirPlayerUnpened)
        ;

    class_<TeamMember>("TeamMember", no_init)
        .def_readonly("number", &TeamMember::playerNumber)
        .def_readonly("x", &TeamMember::myX)
        .def_readonly("y", &TeamMember::myY)
        .def_readonly("h", &TeamMember::myH)
        .def_readonly("xU", &TeamMember::myXUncert)
        .def_readonly("yU", &TeamMember::myYUncert)
        .def_readonly("hU", &TeamMember::myHUncert)
        .def_readonly("ballDist", &TeamMember::ballDist)
        .def_readonly("ballBearing", &TeamMember::ballBearing)
        .def_readonly("ballDistU", &TeamMember::ballDistUncert)
        .def_readonly("ballBearing", &TeamMember::ballBearingUncert)
        .def_readonly("chaseTime", &TeamMember::chaseTime)
        .def_readonly("role", &TeamMember::role)
        .def_readonly("subRole", &TeamMember::subRole)
        .def_readonly("active", &TeamMember::active)
        ;

    class_<Comm, shared_ptr<Comm>, boost::noncopyable >("Comm", no_init)
        .def_readonly("gd", &Comm::getGameData)
        .def("teammate", &Comm::getTeammate)
        .def("setData", &Comm::setBehaviorData)
        .def("setPlayer", &Comm::setMyPlayerNumber)
        .def_readonly("player", &Comm::myPlayerNumber)
        .def("setTeam", &Comm::setTeamNumber)
        ;

    scope().attr("comm") = comm_pointer;
}

void c_init_comm()
{
    if (!Py_IsInitialized())
        Py_Initialize();
    try
    {
        initcomm();
    }
    catch (error_already_set)
    {
        PyErr_Print();
    }
}

void set_comm_pointer (shared_ptr<Comm> commptr)
{
    comm_pointer = commptr;
}
