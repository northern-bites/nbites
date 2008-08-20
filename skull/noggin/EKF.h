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
    EKF();
    virtual ~EKF();

    // Core functions
    virtual void timeUpdate(MotionModel u_t);
    virtual void correctionStep(Measurement z_t);
    virtual void noCorrectionStep();
    //void incorporateTeammateSighting(Measurement z_t);

//protected:

}
#endif _EKF_h_DEFINED
