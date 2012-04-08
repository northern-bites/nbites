#include <Python.h>
#include <boost/python.hpp>
#include <boost/python/object.hpp>
#include <boost/python/module.hpp>
#include <boost/python/args.hpp>
#include <boost/shared_ptr.hpp>
using namespace std;
using namespace boost::python;
using namespace boost;

#include "PyLoc.h"

BOOST_PYTHON_MODULE(_localization)
{
    class_<PyLoc>("Loc")
        .add_property("x", &PyLoc::getXEst)
        .add_property("y", &PyLoc::getYEst)
        .add_property("h", &PyLoc::getHEst)
        .add_property("radH", &PyLoc::getRadHEst)

        .add_property("ballDistance", &PyLoc::getBallDistance)
        .add_property("ballBearing", &PyLoc::getBallBearingDeg)

        // Relative coordinates
        .add_property("ballRelX", &PyLoc::getBallRelXEst)
        .add_property("ballRelY", &PyLoc::getBallRelYEst)
        .add_property("ballRelVelX", &PyLoc::getRelXVelocityEst)
        .add_property("ballRelVelY", &PyLoc::getRelYVelocityEst)
        .add_property("ballRelAccX", &PyLoc::getRelXAccelerationEst)
        .add_property("ballRelAccY", &PyLoc::getRelYAccelerationEst)
        .add_property("ballRelXUncert", &PyLoc::getBallRelXUncert)
        .add_property("ballRelYUncert", &PyLoc::getBallRelYUncert)
        .add_property("ballRelVelXUncert", &PyLoc::getRelXVelocityUncert)
        .add_property("ballRelVelYUncert", &PyLoc::getRelYVelocityUncert)
        .add_property("ballRelAccXUncert", &PyLoc::getRelXAccelerationUncert)
        .add_property("ballRelAccYUncert", &PyLoc::getRelYAccelerationUncert)

        // Global coordinates
        .add_property("ballX", &PyLoc::getBallXEst)
        .add_property("ballY", &PyLoc::getBallYEst)
        .add_property("ballVelX", &PyLoc::getBallXVelocityEst)
        .add_property("ballVelY", &PyLoc::getBallYVelocityEst)
        .add_property("ballAccX", &PyLoc::getBallXAccelerationEst)
        .add_property("ballAccY", &PyLoc::getBallYAccelerationEst)
        .add_property("ballXUncert", &PyLoc::getBallXUncert)
        .add_property("ballYUncert", &PyLoc::getBallYUncert)
        .add_property("ballVelXUncert", &PyLoc::getXVelocityUncert)
        .add_property("ballVelYUncert", &PyLoc::getYVelocityUncert)
        .add_property("ballAccXUncert", &PyLoc::getXAccelerationUncert)
        .add_property("ballAccYUncert", &PyLoc::getYAccelerationUncert)

        // Uncertainty
        .add_property("xUncert", &PyLoc::getXUncert)
        .add_property("yUncert", &PyLoc::getYUncert)
        .add_property("hUncert", &PyLoc::getHUncert)
        .add_property("radHUncert", &PyLoc::getRadHUncert)
        // Odometry
        .add_property("lastOdoX", &PyLoc::getOdoX)
        .add_property("lastOdoY", &PyLoc::getOdoY)
        .add_property("lastOdoTheta", &PyLoc::getOdoTheta)
        // functional
        .def("reset", &PyLoc::reset, "reset the localization system")
        .def("resetBall", &PyLoc::resetBall)
        .def("blueGoalieReset", &PyLoc::blueGoalieReset,
             "reset the localization system")
        .def("redGoalieReset", &PyLoc::redGoalieReset,
             "reset the localization system")
		.def("resetLocTo", &PyLoc::resetLocTo,
			 "reset the localiation system to a specific location")
        ;
}

void c_init_localization() {
    if (!Py_IsInitialized())
        Py_Initialize();
    try {
        init_localization();
    } catch (error_already_set) {
        PyErr_Print();
    }
}

void set_loc_reference(shared_ptr<LocSystem> _loc)
{
    loc_reference = _loc;
}

void set_ballEKF_reference(shared_ptr<BallEKF> _ballEKF)
{
    ballEKF_reference = _ballEKF;
}
