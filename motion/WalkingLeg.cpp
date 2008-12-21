#include "WalkingLeg.h"


WalkingLeg::WalkingLeg(ChainID id,
                       const WalkingParameters *walkP)
    :state(SUPPORTING),lastState(SUPPORTING),lastDiffState(SUPPORTING),
     frameCounter(0), chainID(id), walkParams(walkP),
      goal(ufvector3(3)){
    if (chainID == LLEG_CHAIN)
        leg_sign = 1;
    else
        leg_sign = -1;
    for ( unsigned int i = 0 ; i< LEG_JOINTS; i++) lastJoints[i]=0.0f;
}


vector <float> WalkingLeg::tick(boost::shared_ptr<Step> step,
                                ublas::matrix<float> fc_Transform){
    //cout << "In leg" << chainID << " got target (" x
    //     << dest_x << "," <<dest_y << ")" <<endl;

    ublas::vector<float> dest_f = CoordFrame3D::vector3D(step->x,step->y);
    ublas::vector<float> dest_c = prod(fc_Transform,dest_f);
    float dest_x = dest_c(0);
    float dest_y = dest_c(1);
    //cout << "FC Transform" << fc_Transform <<endl;
    //cout <<"Dest_c: " << dest_f<<endl;
    vector<float> result(6);
    switch(state){
    case SUPPORTING:
        result  = supporting(dest_x, dest_y);
        break;
    case SWINGING:
        result  =  swinging(dest_x, dest_y);
        break;
    case DOUBLE_SUPPORT:
        result  = supporting(dest_x, dest_y);
        break;
    case PERSISTENT_DOUBLE_SUPPORT:
        result  = supporting(dest_x, dest_y);
        break;
    default:
        throw "Invalid SupportMode passed to WalkingLeg::tick";
    }

    frameCounter++;
    //Decide if it's time to switch states
    if ( shouldSwitchStates())
        switchToNextState();

    lastState=state;
    return result;
}


vector <float> WalkingLeg::swinging(float dest_x, float dest_y) {
    // the swinging leg will follow a trapezoid in 3-d. The trapezoid has
    // three stages: going up, a level stretch, going back down to the ground
    static int stage;
    if (firstFrame()) stage = 0;
    float x,y;
    float heightOffGround = 0.0f;

    if (stage == 0) { // we are rising
        // we want to raise the foot up for the first third of the step duration
        heightOffGround = walkParams->stepHeight*
            static_cast<float>(frameCounter) /
            ((walkParams->singleSupportFrames/3));
        if (frameCounter > walkParams->singleSupportFrames/3.)
            stage++;

    }
    else if (stage == 1) { // keep it level
        heightOffGround = walkParams->stepHeight;

        if (frameCounter > 2.* walkParams->singleSupportFrames/3)
            stage++;
    }
    else {// stage 2, set the foot back down on the ground
        heightOffGround = max(0.0f,
                              walkParams->stepHeight*
                              static_cast<float>(walkParams->singleSupportFrames
                                                 -frameCounter)/
                              (walkParams->singleSupportFrames/3));
    }

    goal(0) = dest_x;
    goal(1) = dest_y;
    goal(2) = -walkParams->bodyHeight + heightOffGround;

    IKLegResult result = Kinematics::dls(chainID,goal,lastJoints);
    memcpy(lastJoints, result.angles, LEG_JOINTS*sizeof(float));
    return vector<float>(result.angles, &result.angles[LEG_JOINTS]);
}

vector <float> WalkingLeg::supporting(float dest_x, float dest_y) {
    /**
       this method calculates the angles for this leg when it is on the ground
       (i.e. the leg on the ground in single support, or either leg in double
       support).
       We calculate the goal based on the comx,comy from the controller,
       and the given parameters using inverse kinematics.
     */
    float physicalHipOffY = 0;
    goal(0) = dest_x; //targetX for this leg
    goal(1) = dest_y;  //targetY
    goal(2) = -walkParams->bodyHeight;         //targetZ

    //calculate the new angles
    IKLegResult result = Kinematics::dls(chainID,goal,lastJoints);
    memcpy(lastJoints, result.angles, LEG_JOINTS*sizeof(float));
    return vector<float>(result.angles, &result.angles[LEG_JOINTS]);
}


void WalkingLeg::startLeft(){
    if(chainID == LLEG_CHAIN){
        //we will start walking first by swinging left leg (this leg), so
        //we want double, not persistent, support
        setState(DOUBLE_SUPPORT);
    }else{
        setState(PERSISTENT_DOUBLE_SUPPORT);
    }

}
void WalkingLeg::startRight(){
    if(chainID == LLEG_CHAIN){
        //we will start walking first by swinging right leg (not this leg), so
        //we want persistent double support
        setState(PERSISTENT_DOUBLE_SUPPORT);
    }else{
        setState(DOUBLE_SUPPORT);
    }

}


//transition function of the fsa
//returns the next logical state to enter
SupportMode WalkingLeg::nextState(){
    switch(state){
    case SUPPORTING:
        return DOUBLE_SUPPORT;
    case SWINGING:
        return PERSISTENT_DOUBLE_SUPPORT;
    case DOUBLE_SUPPORT:
        return SWINGING;
    case PERSISTENT_DOUBLE_SUPPORT:
        return SUPPORTING;
    default:
        throw "Non existent state";
        return PERSISTENT_DOUBLE_SUPPORT;
    }
}

//Decides if time is up for the current state
bool WalkingLeg::shouldSwitchStates(){
    switch(state){
    case SUPPORTING:
        return frameCounter >= walkParams->singleSupportFrames;
    case SWINGING:
        return frameCounter >= walkParams->singleSupportFrames;
    case DOUBLE_SUPPORT:
        return frameCounter >= walkParams->doubleSupportFrames;
    case PERSISTENT_DOUBLE_SUPPORT:
        return frameCounter >= walkParams->doubleSupportFrames;
    }

    throw "Non existent state";
    return false;
}

void WalkingLeg::switchToNextState(){
    setState(nextState());
}

void WalkingLeg::setState(SupportMode newState){
    state = newState;
    lastDiffState = state;
    frameCounter = 0;
}
