#include <Python.h>
#include <boost/python.hpp>
#include <boost/python/object.hpp>
#include <boost/python/module.hpp>
#include <boost/python/args.hpp>
#include <boost/shared_ptr.hpp>
using namespace std;
using namespace boost::python;
using namespace boost;

#include "MCL.h"
#include "BallEKF.h"

static shared_ptr<MCL> mcl_reference;
static shared_ptr<BallEKF> ballEKF_reference;

/**
 * Class to hold the localization data needed in Python
 *
 */
class PyLoc {
private:
    shared_ptr<MCL> mcl;
    shared_ptr<BallEKF> ballEKF;
public:
    PyLoc() {
        mcl = mcl_reference;
        ballEKF = ballEKF_reference;
    }

    /* Getters */
    // We use degreees in python, and radians in C++
    // Self localization
    const float getXEst() const { return mcl->getXEst(); }
    const float getYEst() const { return mcl->getYEst(); }
    const float getHEst() const { return mcl->getHEstDeg(); }
    // Self Uncertainty
    const float getXUncert() const { return mcl->getXUncert(); }
    const float getYUncert() const { return mcl->getYUncert(); }
    const float getHUncert() const { return mcl->getHUncertDeg(); }

    // Ball localization
    // Global Coordinates
    const float getBallXEst() const { return ballEKF->getXEst(); }
    const float getBallYEst() const { return ballEKF->getYEst(); }
    const float getXVelocityEst() const { return ballEKF->getXVelocityEst(); }
    const float getYVelocityEst() const { return ballEKF->getYVelocityEst(); }

    // Ball Uncertainty
    // Global Coordinates
    const float getBallXUncert() const { return ballEKF->getXUncert(); }
    const float getBallYUncert() const { return ballEKF->getYUncert(); }
    const float getXVelocityUncert() const {
        return ballEKF->getXVelocityUncert();
    }
    const float getYVelocityUncert() const {
        return ballEKF->getYVelocityUncert();
    }
};

BOOST_PYTHON_MODULE(_localization)
{
    class_<PyLoc>("Loc")
        .add_property("x", &PyLoc::getXEst)
        .add_property("y", &PyLoc::getYEst)
        .add_property("h", &PyLoc::getHEst)
        .add_property("ballX", &PyLoc::getBallXEst)
        .add_property("ballY", &PyLoc::getBallYEst)
        .add_property("ballVelX", &PyLoc::getXVelocityEst)
        .add_property("ballVelY", &PyLoc::getYVelocityEst)
        // Uncertainty
        .add_property("xUncert", &PyLoc::getXUncert)
        .add_property("yUncert", &PyLoc::getYUncert)
        .add_property("hUncert", &PyLoc::getHUncert)
        .add_property("ballXUncert", &PyLoc::getBallXUncert)
        .add_property("ballYUncert", &PyLoc::getBallYUncert)
        .add_property("ballVelXUncert", &PyLoc::getXVelocityUncert)
        .add_property("ballVelYUncert", &PyLoc::getYVelocityUncert)
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

void set_mcl_reference(shared_ptr<MCL> _mcl)
{
    mcl_reference = _mcl;
}

void set_ballEKF_reference(shared_ptr<BallEKF> _ballEKF)
{
    ballEKF_reference = _ballEKF;
}
