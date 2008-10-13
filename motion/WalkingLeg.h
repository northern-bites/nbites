
#ifndef _WalkingLeg_h_DEFINED
#define _WalkingLeg_h_DEFINED

#include <vector>
using namespace std;

#include "WalkingConstants.h"
#include "Kinematics.h"
using namespace Kinematics;

class WalkingLeg  {
public:
    WalkingLeg(ChainID id, WalkingParameters * walkP);
    ~WalkingLeg();

    vector <float> step(float com_x, float com_y);
    void switchSupportMode(SupportMode newMode){supportMode=newMode;}

private:
    vector <float> supporting(float com_x, float com_y);
    vector <float> swinging(float com_x, float com_y);

    SupportMode supportMode;
    //LegConstants * legConsts;
    ChainID chainID; //keep track of which leg this is
    WalkingParameters * walkParams;
    float lastJoints[LEG_JOINTS];
    ufvector3 goal;
    int leg_sign; //-1 for right leg, 1 for left leg
};

#endif
