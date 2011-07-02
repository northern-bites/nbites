#ifndef PyLoc_h_DEFINED
#define PyLoc_h_DEFINED
#include <boost/shared_ptr.hpp>

// #include "LocEKF.h"
#include "LocSystem.h"
#include "BallEKF.h"

using boost::shared_ptr;

static shared_ptr<LocSystem> loc_reference;
static shared_ptr<BallEKF> ballEKF_reference;

/**
 * Class to hold the localization data needed in Python
 *
 */
class PyLoc {
private:
    shared_ptr<BallEKF> ballEKF;
public:
    PyLoc() {
        loc = loc_reference;
        ballEKF = ballEKF_reference;
    }
    shared_ptr<LocSystem> loc;

    void reset() {
        loc->reset();
        ballEKF->reset();
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

void c_init_localization();

// C++ backend insertion (must be set before import)
//    can only be called once (subsequent calls ignored)
void set_loc_reference(boost::shared_ptr<LocSystem> _loc);
void set_ballEKF_reference(boost::shared_ptr<BallEKF> _ballEKF);

#endif // PyLoc_h_DEFINED
