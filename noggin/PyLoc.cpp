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
    float getXEst() { return mcl->getXEst(); }
    float getYEst() { return mcl->getYEst(); }
    float getHEst() { return mcl->getHEstDeg(); }
    // Self Uncertainty
    float getXUncert() { return mcl->getXUncert(); }
    float getYUncert() { return mcl->getYUncert(); }
    float getHUncert() { return mcl->getHUncertDeg(); }

    // Ball localization
    float getBallXEst() { return ballEKF->getXEst(); }
    float getBallYEst() { return ballEKF->getYEst(); }
    float getXVelocityEst() { return ballEKF->getXVelocityEst(); }
    float getYVelocityEst() { return ballEKF->getYVelocityEst(); }
    // Ball Uncertainty
    float getBallXUncert() { return ballEKF->getXUncert(); }
    float getBallYUncert() { return ballEKF->getYUncert(); }
    float getXVelocityUncert() { return ballEKF->getXVelocityUncert(); }
    float getYVelocityUncert() { return ballEKF->getYVelocityUncert(); }
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
