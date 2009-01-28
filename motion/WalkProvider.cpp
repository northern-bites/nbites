#include "WalkProvider.h"
using Kinematics::LLEG_CHAIN;
using Kinematics::RLEG_CHAIN;

WalkProvider::WalkProvider()
    : MotionProvider(),
      walkParameters(.02f, // motion frame length
                     310.0f, // COM height
                     0.0f, // hipOffsetX
                     1.00f, //stepDuration
                     0.25f, //fraction in double support mode
                     17.0f), // steHeight
      stepGenerator(&walkParameters)
{

    //Make up something arbitrary for the arms
    const float larm[ARM_JOINTS] = {M_PI/2,M_PI/10,-M_PI/2,-M_PI/2};
    const float rarm[ARM_JOINTS] = {M_PI/2,-M_PI/10,M_PI/2,M_PI/2};
    larm_angles = vector<float>(larm,larm+ARM_JOINTS);
    rarm_angles = vector<float>(rarm,rarm+ARM_JOINTS);

    setActive();
}

WalkProvider::~WalkProvider() {

}

void WalkProvider::requestStop() {

}

void WalkProvider::calculateNextJoints() {
    //ask the step Generator to update ZMP values, com targets
    stepGenerator.tick_controller();

    // Now ask the step generator to get the leg angles
    WalkLegsTuple legs_result = stepGenerator.tick_legs();

    //Get the joints for each Leg
    vector<float> lleg_results = legs_result.get<LEFT_FOOT>();
    vector<float> rleg_results = legs_result.get<RIGHT_FOOT>();

    // vector<float> rarm_results = vector<float>(ARM_JOINTS, 0.0f);
//     vector<float> larm_results = vector<float>(ARM_JOINTS, 0.0f);

//     cout << "2rarm size "<< rarm_results.size()
//          << "larm size "<< larm_results.size() <<endl;


    //Return the joints for the legs
    setNextChainJoints(LARM_CHAIN,larm_angles);
    setNextChainJoints(LLEG_CHAIN,lleg_results);
    setNextChainJoints(RLEG_CHAIN,rleg_results);
    setNextChainJoints(RARM_CHAIN,rarm_angles);

    setActive();
}

void WalkProvider::setMotion(const float x, const float y, const float theta) {

}


//Returns the 20 body joints
vector<float> WalkProvider::getWalkStance(){
    //cout << "getWalkStance" <<endl;
    //calculate the walking stance of the robot
    const float z = walkParameters.bodyHeight;
    const float x = walkParameters.hipOffsetX;
    const float ly = HIP_OFFSET_Y;
    const float ry = -HIP_OFFSET_Y;


    //just assume we start at zero
    float zeroJoints[LEG_JOINTS] = {0.1f,0.1f,0.1f,
                                    0.1f,0.1f,0.1f};
    //Use inverse kinematics to find the left leg angles
    ufvector3 lgoal = ufvector3(3);
    lgoal(0)=x; lgoal(1) = ly; lgoal(2) = -z;
    IKLegResult lresult = Kinematics::dls(LLEG_CHAIN,lgoal,zeroJoints);
    vector<float> lleg_angles(lresult.angles, lresult.angles + LEG_JOINTS);

    //Use inverse kinematics to find the right leg angles
    ufvector3 rgoal = ufvector3(3);
    rgoal(0)=x; rgoal(1) = ry; rgoal(2) = -z;
    IKLegResult rresult = Kinematics::dls(RLEG_CHAIN,rgoal,zeroJoints);
    vector<float> rleg_angles(rresult.angles, rresult.angles + LEG_JOINTS);

    vector<float> allJoints;

    //now combine all the vectors together
    allJoints.insert(allJoints.end(),larm_angles.begin(),larm_angles.end());
    allJoints.insert(allJoints.end(),lleg_angles.begin(),lleg_angles.end());
    allJoints.insert(allJoints.end(),rleg_angles.begin(),rleg_angles.end());
    allJoints.insert(allJoints.end(),rarm_angles.begin(),rarm_angles.end());
    return allJoints;
}

void WalkProvider::setActive(){
    //check to see if the walk engine is active
    if(stepGenerator.isDone()){
        inactive();
    }else{
        active();
    }
}
