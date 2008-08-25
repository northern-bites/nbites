/**
 * BallEKF.h - Header file for the BallEKF class
 *
 * @author Tucker Hermans
 */

#ifndef BallEKF_h_DEFINED
#define BallEKF_h_DEFINED

#include "EKF.h"

class BallEKF : public EKF
{
public:
    BallEKF(float initX, float initY,
            float initVelX, float initVelY,
            float initXUncert,float initYUncert,
            float initVelXUncert, float initVelYUncert);

    virtual void timeUpdate(MotionModel u_t);
}
#endif // File
