/**
 * BallEKF.h - Header file for the BallEKF class
 *
 * @author Tucker Hermans
 */

#ifndef _BallEKF_h_DEFINED
#define _BallEKF_h_DEFINED
using namespace std;

class BallEKF : public EKF
{
public:
    BallEKF(float initX, float initY,
            float initVelX, float initVelY,
            float initXUncert,float initYUncert,
            float initVelXUncert, float initVelYUncert);
}
#endif // File
