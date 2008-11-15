#include "WalkingLeg.h"



WalkingLeg::WalkingLeg(ChainID id,
                       WalkingParameters * walkP)
  : supportMode(SUPPORTING),chainID(id), walkParams(walkP), goal(ufvector3(3))
{
    if (chainID == LLEG_CHAIN)
        leg_sign = 1;
    else
        leg_sign = -1;
    for ( unsigned int i = 0 ; i< LEG_JOINTS; i++) lastJoints[i]=0.0f;
}

vector <float> WalkingLeg::tick(float com_x, float com_y){
    if (supportMode == SUPPORTING){
        return supporting(com_x, com_y);
    }else if(supportMode == SWINGING){
        return swinging(com_x, com_y);
    }else{
        throw "Invalid SupportMode passed to WalkingLeg::tick";
    }
}


vector <float> WalkingLeg::swinging(float com_x, float com_y){
    return vector<float>();

}

vector <float> WalkingLeg::supporting(float com_x, float com_y){
    /**
       this method calculates the angles for this leg when it is on the ground
       (i.e. the leg on the ground in single support, or either leg in double
       support).
       We calculate the goal based on the comx,comy from the controller,
       and the given parameters using inverse kinematics.
     */
    float physicalHipOffY = 0;
    goal(0) = -com_x + walkParams->hipOffsetX; //targetX for this leg
    goal(1) = -com_y + leg_sign*HIP_OFFSET_Y;  //targetY
    goal(2) = -walkParams->bodyHeight;         //targetZ

    IKLegResult result = Kinematics::dls(chainID,goal,lastJoints);
    memcpy(lastJoints, result.angles, LEG_JOINTS*sizeof(float));
    return vector<float>(result.angles, &result.angles[LEG_JOINTS]);

    //return vector<float>(6);
};

