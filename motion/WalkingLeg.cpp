#include "WalkingLeg.h"


WalkingLeg::WalkingLeg(ChainID id,
                       const WalkingParameters *walkP)
    : supportMode(SUPPORTING), chainID(id), walkParams(walkP),
      goal(ufvector3(3)), stage(0), frameCounter(0) {
    if (chainID == LLEG_CHAIN)
        leg_sign = 1;
    else
        leg_sign = -1;
    for ( unsigned int i = 0 ; i< LEG_JOINTS; i++) lastJoints[i]=0.0f;
}

vector <float> WalkingLeg::tick(float com_x, float com_y) {
    if (supportMode == SUPPORTING) {
        return supporting(com_x, com_y);
    }
    else if(supportMode == SWINGING) {
        return swinging(com_x, com_y);
    }
    else if(supportMode == DOUBLE_SUPPORT) {
        return supporting(com_x, com_y);
    }
    else if(supportMode == PERSISTENT_DOUBLE_SUPPORT) {
        return supporting(com_x, com_y);
    }
    else{
        throw "Invalid SupportMode passed to WalkingLeg::tick";
    }
}


vector <float> WalkingLeg::swinging(float com_x, float com_y) {
    // the swinging leg will follow a trapezoid in 3-d. The trapezoid has
    // three stages: going up, a level stretch, going back down to the ground

    float x,y;
    float heightOffGround = 0.0f;

    int numChops = static_cast<int>(walkParams->stepDuration /
                                    walkParams->motion_frame_length_s);

    int numFramesInDoubleSupport = numChops/4;

    if (stage == 0) { // we are rising
        if (frameCounter > numFramesInDoubleSupport)
        // we want to raise the foot up for the first third of the step duration
            heightOffGround = walkParams->stepHeight*
                static_cast<float>(frameCounter-numFramesInDoubleSupport) /
                ((numChops/3 - numFramesInDoubleSupport));
        if (frameCounter > numChops/3.)
            stage++;

    }
    else if (stage == 1) { // keep it level
        heightOffGround = walkParams->stepHeight;

        if (frameCounter > 2.*numChops/3)
            stage++;
    }
    else {// stage 2, set the foot back down on the ground
        heightOffGround = max(0.0f,walkParams->stepHeight*
                              static_cast<float>(numChops-frameCounter)/((numChops/3)));
    }

    frameCounter++;

    goal(0) = -com_x + walkParams->hipOffsetX;
    goal(1) = -com_y + leg_sign*HIP_OFFSET_Y;
    goal(2) = -walkParams->bodyHeight + heightOffGround;

    IKLegResult result = Kinematics::dls(chainID,goal,lastJoints);
    memcpy(lastJoints, result.angles, LEG_JOINTS*sizeof(float));
    return vector<float>(result.angles, &result.angles[LEG_JOINTS]);

}

vector <float> WalkingLeg::supporting(float com_x, float com_y) {
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

