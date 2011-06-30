#include <Python.h>
#include <boost/python.hpp>
#include <boost/python/object.hpp>
#include <boost/python/module.hpp>
#include <boost/python/args.hpp>
#include <boost/shared_ptr.hpp>
using namespace std;
using namespace boost::python;
using namespace boost;

// #include "LocEKF.h"
#include "LocSystem.h"
#include "BallEKF.h"

static shared_ptr<LocSystem> loc_reference;
static shared_ptr<BallEKF> ballEKF_reference;

/**
 * Class to hold the localization data needed in Python
 *
 */
class PyLoc {
private:
    shared_ptr<LocSystem> loc;
    shared_ptr<BallEKF> ballEKF;
public:
    PyLoc() {
        loc = loc_reference;
        ballEKF = ballEKF_reference;
    }

    void reset() {
        loc->reset();
    }
    void resetBall() {
        ballEKF->reset();
    }
    void blueGoalieReset() {
        loc->blueGoalieReset();
    }
    void redGoalieReset() {
        loc->redGoalieReset();
    }

	void resetLocTo(float x, float y, float h){
		loc->resetLocTo(x, y, h * TO_RAD);
		ballEKF->reset();
	}

    /* Getters */
    // We use degreees in python, and radians in C++
    // Self localization
    const float getXEst() const { return loc->getXEst(); }
    const float getYEst() const { return loc->getYEst(); }
    const float getHEst() const { return loc->getHEstDeg(); }
    const float getRadHEst() const { return loc->getHEst(); }
    // Self Uncertainty
    const float getXUncert() const { return loc->getXUncert(); }
    const float getYUncert() const { return loc->getYUncert(); }
    const float getHUncert() const { return loc->getHUncertDeg(); }
    const float getRadHUncert() const { return loc->getHUncert(); }

    // Ball localization
    // Global Coordinates
    const float getBallXEst() const { return ballEKF->getGlobalX(); }
    const float getBallYEst() const { return ballEKF->getGlobalY(); }
    const float getBallXVelocityEst() const {
        return ballEKF->getGlobalXVelocity();
    }
    const float getBallYVelocityEst() const {
        return ballEKF->getGlobalYVelocity();
    }
    const float getBallXAccelerationEst() const {
        return ballEKF->getGlobalXAcceleration();
    }
    const float getBallYAccelerationEst() const {
        return ballEKF->getGlobalYAcceleration();
    }

    // Relative coordinates
    const float getBallRelXEst() const { return ballEKF->getRelativeX(); }
    const float getBallRelYEst() const { return ballEKF->getRelativeY(); }
    const float getRelXVelocityEst() const {
        return ballEKF->getRelativeXVelocity();
    }
    const float getRelYVelocityEst() const {
        return ballEKF->getRelativeYVelocity();
    }
    const float getRelXAccelerationEst() const {
        return ballEKF->getRelativeXAcceleration();
    }
    const float getRelYAccelerationEst() const {
        return ballEKF->getRelativeYAcceleration();
    }

    // Ball Uncertainty
    // Global Coordinates
    const float getBallXUncert() const { return ballEKF->getGlobalXUncert(); }
    const float getBallYUncert() const { return ballEKF->getGlobalYUncert(); }
    const float getXVelocityUncert() const {
        return ballEKF->getGlobalXVelocityUncert();
    }
    const float getYVelocityUncert() const {
        return ballEKF->getGlobalYVelocityUncert();
    }
    const float getXAccelerationUncert() const {
        return ballEKF->getGlobalXAccelerationUncert();
    }
    const float getYAccelerationUncert() const {
        return ballEKF->getGlobalYAccelerationUncert();
    }

    // Relative coordinates
    const float getBallRelXUncert() const {
        return ballEKF->getRelativeXUncert();
    }
    const float getBallRelYUncert() const {
        return ballEKF->getRelativeYUncert();
    }
    const float getRelXVelocityUncert() const {
        return ballEKF->getRelativeXVelocityUncert();
    }
    const float getRelYVelocityUncert() const {
        return ballEKF->getRelativeYVelocityUncert();
    }
    const float getRelXAccelerationUncert() const {
        return ballEKF->getRelativeXAccelerationUncert();
    }
    const float getRelYAccelerationUncert() const {
        return ballEKF->getRelativeYAccelerationUncert();
    }

    float getBallDistance() const {
        return ballEKF->getDistance();
    }

    float getBallBearing() const {
        return ballEKF->getBearing();
    }

    float getBallBearingDeg() const {
        return ballEKF->getBearingDeg();
    }

    // Odometry
    const float getOdoF() const { return loc->getLastOdo().deltaF; }
    const float getOdoL() const { return loc->getLastOdo().deltaL; }
    const float getOdoR() const { return loc->getLastOdo().deltaR; }

};

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
        .add_property("lastOdoF", &PyLoc::getOdoF)
        .add_property("lastOdoL", &PyLoc::getOdoL)
        .add_property("lastOdoR", &PyLoc::getOdoR)
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
