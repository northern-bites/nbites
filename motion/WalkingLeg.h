
#ifndef _WalkingLeg_h_DEFINED
#define _WalkingLeg_h_DEFINED

#include <vector>
using namespace std;

#include "WalkingConstants.h"
#include "Kinematics.h"
using namespace Kinematics;

class WalkingLeg  {
public:
    WalkingLeg(ChainID id, const WalkingParameters * walkP);
    //~WalkingLeg() { };

    vector <float> tick(float com_x, float com_y);
    void switchSupportMode(SupportMode newMode){supportMode=newMode;}
    void switchSupportMode() {
        switch(supportMode) {
        case DOUBLE_SUPPORT:
            supportMode = SWINGING;
            break;
        case PERSISTENT_DOUBLE_SUPPORT:
            supportMode = SUPPORTING;
            break;
        case SUPPORTING:
            supportMode = SWINGING;
            break;
        case SWINGING:
            supportMode = SUPPORTING;
            stage = 0;
            frameCounter = 0;
            break;
        default:
            break;
        }
    }

private:
    vector <float> supporting(float com_x, float com_y);
    vector <float> swinging(float com_x, float com_y);

    SupportMode supportMode;
    //LegConstants * legConsts;
    ChainID chainID; //keep track of which leg this is
    const WalkingParameters *walkParams;
    float lastJoints[LEG_JOINTS];
    ufvector3 goal;
    int leg_sign; //-1 for right leg, 1 for left leg

    /***   The following are used for the swinging leg   ***/

    int stage; // of following the trapezoid shape
    // used to keep track how many frames have passed since start of swing
    int frameCounter;
};

#endif
