/**
 * EKF.h - Header file for the EKF class
 *
 * @author Tucker Hermans
 */

#ifndef _EKF_h_DEFINED
#define _EKF_h_DEFINED
using namespace std;

class EKF
{
public:
    // Constructors & Destructors
    EKF(float initX, float initY,
        float initVelX, float initVelY,
        float initXUncert,float initYUncert,
        float initVelXUncert, float initVelYUncert);
    ~EKF();

    // Core functions
    void updateOdometery(MotionModel u_t);
    void objectSighted(Measurement z_t);
    void incorporateTeammateSighting(Measurement z_t);
    void objectNotSighted();
}
#endif _EKF_h_DEFINED
