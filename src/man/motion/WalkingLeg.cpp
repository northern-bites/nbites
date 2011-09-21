
// This file is part of Man, a robotic perception, locomotion, and
// team strategy application created by the Northern Bites RoboCup
// team of Bowdoin College in Brunswick, Maine, for the Aldebaran
// Nao robot.
//
// Man is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Man is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser Public License for more details.
//
// You should have received a copy of the GNU General Public License
// and the GNU Lesser Public License along with Man.  If not, see
// <http://www.gnu.org/licenses/>.

#include <iomanip>

#include "WalkingLeg.h"

using namespace std;

using namespace Kinematics;
using namespace NBMath;

//#define DEBUG_WALKINGLEG
//#define USE_COM_CONTROL
//#define WALKING_LOCUS_LOGGING

WalkingLeg::WalkingLeg(boost::shared_ptr<Sensors> s,
                       const MetaGait * _gait,
                       const SensorAngles * _sensorAngles,
                       ChainID id)
    :sensors(s),
     state(SUPPORTING),
     frameCounter(0),
     cur_dest(EMPTY_STEP),swing_src(EMPTY_STEP),swing_dest(EMPTY_STEP),
     support_step(EMPTY_STEP),
     chainID(id), gait(_gait),
     goal(CoordFrame3D::vector3D(0.0f,0.0f,0.0f)),
     last_goal(CoordFrame3D::vector3D(0.0f,0.0f,0.0f)),
     odometry(new OdoFilter()),
     odoDiff(std::vector<float>()),
     lastRotation(0.0f),
     leg_sign(id == LLEG_CHAIN ? 1 : -1),
     leg_name(id == LLEG_CHAIN ? "left" : "right"),
     sensorAngles(_sensorAngles), sensorAngleX(0.0f), sensorAngleY(0.0f)
{
#ifdef DEBUG_WALKING_LOCUS_LOGGING
    char filepath[100];
    sprintf(filepath,"/tmp/%s_locus_log.xls",leg_name.c_str());
    locus_log  = fopen(filepath,"w");
    fprintf(locus_log,"time\tgoal_x\tgoal_y\tgoal_z\tstate\n");
#endif
#ifdef DEBUG_WALKING_DEST_LOGGING
    char filepath2[100];
    sprintf(filepath2,"/tmp/%s_dest_log.xls",leg_name.c_str());
    dest_log  = fopen(filepath2,"w");
    fprintf(dest_log,"time\tdest_x\tdest_y\tsrc_x\tsrc_y\tstate\n");
#endif
#ifdef DEBUG_WALKING_SENSOR_LOGGING
    char filepath3[120];
    sprintf(filepath3,"/tmp/%s_sensor_log.xls",leg_name.c_str());
    sensor_log  = fopen(filepath3,"w");
    fprintf(sensor_log,"time\tbodyAngleX\tbodyAngleY\t"
            "sensorAngleX\tsensorAngleY\t"
            "angleX\tangleY\tstate\n");
#endif
    for ( unsigned int i = 0 ; i< LEG_JOINTS; i++) lastJoints[i]=0.0f;
}


WalkingLeg::~WalkingLeg(){
#ifdef DEBUG_WALKING_LOCUS_LOGGING
    fclose(locus_log);
#endif
#ifdef DEBUG_WALKING_DEST_LOGGING
    fclose(dest_log);
#endif
}

void WalkingLeg::setSteps(boost::shared_ptr<Step> _swing_src,
                          boost::shared_ptr<Step> _swing_dest,
                          boost::shared_ptr<Step> _support_step){
    swing_src = _swing_src;
    swing_dest = _swing_dest;
    support_step = _support_step;
    assignStateTimes(support_step);
}

LegJointStiffTuple WalkingLeg::tick(boost::shared_ptr<Step> step,
				    boost::shared_ptr<Step> _swing_src,
				    boost::shared_ptr<Step> _swing_dest,
				    ufmatrix3 fc_Transform){
#ifdef DEBUG_WALKINGLEG
    cout << "WalkingLeg::tick() "<<leg_name <<" leg, state is "<<state<<endl;
#endif
    cur_dest = step;
    swing_src = _swing_src;
    swing_dest = _swing_dest;

    //ufvector3 dest_f = CoordFrame3D::vector3D(cur_dest->x,cur_dest->y);
    //ufvector3 dest_c = prod(fc_Transform,dest_f);
    //float dest_x = dest_c(0);
    //float dest_y = dest_c(1);
    LegJointStiffTuple result;
    switch(state){
    case SUPPORTING:
        result  = supporting(fc_Transform);
        break;
    case SWINGING:
        result  =  swinging(fc_Transform);
        break;
    case DOUBLE_SUPPORT:
        //In dbl sup, we have already got the final target after swinging in
        //mind, so we actually want to keep the target as the "source"
        cur_dest = swing_src;
        result  = supporting(fc_Transform);
        break;
    case PERSISTENT_DOUBLE_SUPPORT:
        result  = supporting(fc_Transform);
        break;
    default:
        cout << "Invalid SupportMode"<<endl;
        throw "Invalid SupportMode passed to WalkingLeg::tick";
    }

//    cout << "Step: " << step << endl;
    //  cout << "F->C Transform: " << fc_Transform << endl;

    computeOdoUpdate();
    debugProcessing(fc_Transform);

    last_goal = goal;
    lastRotation = getFootRotation_c();
    frameCounter++;
    //Decide if it's time to switch states

    //Decide if we need to switch states. Run twice in case state time of new
    //state is zero 
    for(unsigned int  i = 0; shouldSwitchStates() && i < 2; i++,switchToNextState());

    return result;
}

LegJointStiffTuple WalkingLeg::swinging(ufmatrix3 fc_Transform){
    ufvector3 dest_f = CoordFrame3D::vector3D(cur_dest->x,cur_dest->y);
    ufvector3 src_f = CoordFrame3D::vector3D(swing_src->x,swing_src->y);

    ufvector3 dest_c = prod(fc_Transform,dest_f);
    ufvector3 src_c = prod(fc_Transform,src_f);

    //float dest_x = dest_c(0);
    //float dest_y = dest_c(1);

    static float dist_to_cover_x = 0;
    static float dist_to_cover_y = 0;

    if(firstFrame()){
	dist_to_cover_x = cur_dest->x - swing_src->x;
	dist_to_cover_y = cur_dest->y - swing_src->y;
    }

    //There are two attirbutes to control - the height off the ground, and
    //the progress towards the goal.

    //HORIZONTAL PROGRESS:
    const float percent_complete =
        ( static_cast<float>(frameCounter) /
          static_cast<float>(singleSupportFrames));

    float theta = percent_complete*2.0f*M_PI_FLOAT;
    float percent_to_dest_horizontal = NBMath::cycloidx(theta)/(2.0f*M_PI_FLOAT);

    //Then we can express the destination as the proportionate distance to cover
    float dest_x = src_f(0) + percent_to_dest_horizontal*dist_to_cover_x;
    float dest_y = src_f(1) + percent_to_dest_horizontal*dist_to_cover_y;

    ufvector3 target_f = CoordFrame3D::vector3D(dest_x,dest_y);
    ufvector3 target_c = prod(fc_Transform, target_f);

    float target_c_x = target_c(0);
    float target_c_y = target_c(1);

    float radius =gait->step[WP::STEP_HEIGHT]/2;
    float heightOffGround = radius*NBMath::cycloidy(theta);

    goal(0) = target_c_x;
    goal(1) = target_c_y;
    goal(2) = -gait->stance[WP::BODY_HEIGHT] + heightOffGround;

/*
    if ( (goal(0) - last_goal(0) ) > 5) {
	cout << "broken goal x" << endl
	     << "fc_Transform: " << fc_Transform << endl
	     << "dest_f " << dest_f
	     << "src_f " << src_f
	     << "dest_c " << dest_c << endl
	     << "src_c " << src_c << endl
	     << "target_f " << target_f << endl
	     << "target_c " << target_c << endl
	     << "percent complete " << percent_complete << "theta " << theta
	     << " percent to horizontal " << percent_to_dest_horizontal << endl;
	throw "Found a bad value, dying";
    }
*/

    vector<float> joint_result = finalizeJoints(goal);

    vector<float> stiff_result = getStiffnesses();
    return LegJointStiffTuple(joint_result,stiff_result);
}

/**
   this method calculates the angles for this leg when it is on the
   ground (i.e. the leg on the ground in single support, or either
   leg in double support).  We calculate the goal based on the
   comx,comy from the controller, and the given parameters using
   inverse kinematics.
*/
LegJointStiffTuple WalkingLeg::supporting(ufmatrix3 fc_Transform){//float dest_x, float dest_y) {
    ufvector3 dest_f = CoordFrame3D::vector3D(cur_dest->x,cur_dest->y);
    ufvector3 dest_c = prod(fc_Transform,dest_f);
    float dest_x = dest_c(0);
    float dest_y = dest_c(1);

    //HORIZONTAL PROGRESS:
    const float percent_complete =
        ( static_cast<float>(frameCounter) /
          static_cast<float>(singleSupportFrames));

    float com_height_adjustment = 0.0f;

    // modulate our CoM height based on where we are in the step phase
    // check bounds, so we don't do stupid things when the robot is stationary
    // disabled for US Open 2011 until I can do more testing --Nathan @ 4/21/11
    if (false && percent_complete >= 0.0f && percent_complete <= 1.0f) {
	const float com_height_adjustment_max = -1.5f;
	// sin maps [0, 1.0f] -> [0, 1]
	com_height_adjustment = sin(percent_complete*M_PI_FLOAT)
	    * com_height_adjustment_max;

	//cout << "percent complete: " << percent_complete << endl;
//		cout << "height adjustment " << com_height_adjustment << endl;
    }

    goal(0) = dest_x; //targetX for this leg
    goal(1) = dest_y;  //targetY
    goal(2) = -gait->stance[WP::BODY_HEIGHT] - com_height_adjustment; //targetZ

    //cout << "goal X: " << dest_x << " Y: " << dest_y << " Z: " << goal(2) << endl;

    vector<float> joint_result = finalizeJoints(goal);
    vector<float> stiff_result = getStiffnesses();
    return LegJointStiffTuple(joint_result,stiff_result);
}


const vector<float> WalkingLeg::finalizeJoints(const ufvector3& footGoal){
    const float startStopSensorScale = getEndStepSensorScale();

    const float COM_SCALE = startStopSensorScale;
    const ufvector4 com_c = CoordFrame4D::vector4D(0,0,0);

    //HACK -- the startStopSensor gives us a nice in/out scaling from motion
    //we should really rename that function
    const float COM_Z_OFF = 69.9f;
    ufvector3 comFootGoal = footGoal;
    comFootGoal(2) += COM_Z_OFF*COM_SCALE;

    const boost::tuple <const float, const float > sensorCompensation =
        sensorAngles->getAngles(startStopSensorScale);

    const float bodyAngleX = sensorAngleX =
        sensorCompensation.get<SensorAngles::X>();
    const float bodyAngleY = sensorAngleY = gait->stance[WP::BODY_ROT_Y] +
        sensorCompensation.get<SensorAngles::Y>();

    //Hack
    const boost::tuple <const float, const float > ankleAngleCompensation =
	getAnkleAngles();

    const float footAngleX = ankleAngleCompensation.get<0>();
    const float footAngleY = ankleAngleCompensation.get<1>();
    const float footAngleZ = getFootRotation_c()
        + static_cast<float>(leg_sign) * gait->stance[WP::LEG_ROT_Z] * 0.5f;

    const ufvector3 bodyOrientation = CoordFrame3D::vector3D(bodyAngleX,
							     bodyAngleY, 0.0f);
    const ufvector3 footOrientation = CoordFrame3D::vector3D(footAngleX,
                                                             footAngleY,
                                                             footAngleZ);

    const ufvector3 bodyGoal = COM_SCALE * CoordFrame3D::vector3D( -com_c(0),
								   -com_c(1),
								   COM_Z_OFF);
    IKLegResult result =
        Kinematics::legIK(chainID,comFootGoal,footOrientation,
                          bodyGoal,bodyOrientation);

    applyHipHacks(result.angles);

    memcpy(lastJoints, result.angles, LEG_JOINTS*sizeof(float));
    return vector<float>(result.angles, &result.angles[LEG_JOINTS]);
}

const boost::tuple <const float, const float >
WalkingLeg::getAnkleAngles(){
    if(state != SWINGING){
	return boost::tuple<const float, const float>(0.0f,0.0f);
    }

    const float angle = static_cast<float>(frameCounter)/
	static_cast<float>(singleSupportFrames)*M_PI_FLOAT;

    const float scale = std::sin(angle);

    const float ANKLE_LIFT_ANGLE = swing_dest->stepConfig[WP::FOOT_LIFT_ANGLE]*scale;

    return boost::tuple<const float, const float>(0.0f,ANKLE_LIFT_ANGLE);
}


/*
 * When we are starting and stopping, we want to gradually turn on or off sensor
 * feedback.  We do this by checking if the support step is an END step,
 * and to see if the next step is also an end step. If so, and the next one
 * is an end step as well, then we are stopping
 * if not, then we are starting
 */
const float WalkingLeg::getEndStepSensorScale(){

    //do nothing for regular steps
    if(support_step->type == REGULAR_STEP)
        return 1.0f;

    if ( swing_src->type==END_STEP)
        //We've already stopped, so don't scale down again!
        return 0.0f;

    float startScale, endScale;
    if(swing_dest->type == REGULAR_STEP){
        //Starting from stopped
        startScale = 0.0f;
        endScale = 1.0f;
    }else{
        //Stopping
        startScale = 1.0f;
        endScale = 0.0f;
    }
    //WARNING: Assume in an END step, all frames of cycle are double support
    float percent_complete = static_cast<float>(frameCounter) /
        static_cast<float>(doubleSupportFrames);

    const float theta = percent_complete*2.0f*M_PI_FLOAT;//TODO: move to common
    const float percent_to_dest = NBMath::cycloidx(theta)/(2.0f*M_PI_FLOAT);

    return startScale + (endScale-startScale)*percent_to_dest;
}

/*  Returns the rotation for this motion frame which we expect
 *  the swinging foot to have relative to the support foot (in the f frame)
 */
const float WalkingLeg::getFootRotation(){
    if(state != SUPPORTING && state != SWINGING)
        return swing_src->theta;

    const float percent_complete =
        static_cast<float>(frameCounter) /
	static_cast<float>(singleSupportFrames);

    const float theta = percent_complete*2.0f*M_PI_FLOAT;
    const float percent_to_dest = NBMath::cycloidx(theta)/(2.0f*M_PI_FLOAT);

    const float end = swing_dest->theta;
    const float start = swing_src->theta;

    const float value = start + (end-start)*percent_to_dest;
    return value;
}

/* Returns the foot rotation for this foot relative to the C frame*/
const float WalkingLeg::getFootRotation_c(){
    const float abs_rot =  std::abs(getFootRotation());

    const float rot_rel_c = abs_rot * 0.5f * static_cast<float>(leg_sign);

    return rot_rel_c;


}

/* We assume!!! that the rotation of the hip yaw pitch joint should be 1/2*/
const float WalkingLeg::getHipYawPitch(){
    return -fabs(getFootRotation()*0.5f);
}

void WalkingLeg::applyHipHacks(float angles[]){
    const float footAngleZ = getFootRotation_c();
    boost::tuple <const float, const float > hipHacks  = getHipHack(footAngleZ);
    angles[1] += hipHacks.get<1>(); //HipRoll
    angles[2] += hipHacks.get<0>(); //HipPitch
}

/**
 * Function returns the angle to add to the hip roll joint depending on
 * how far along we are in the process of a state (namely swinging,
 * and supporting)
 *
 * In certain circumstances, this function returns 0.0f, since we don't
 * want to be hacking the hio when we are starting and stopping.
 */
const boost::tuple<const float, const float>
WalkingLeg::getHipHack(const float footAngleZ){

    ChainID hack_chain;
    if(state == SUPPORTING){
        hack_chain = chainID;
    }else if(state == SWINGING){
        hack_chain = getOtherLegChainID();
    }else{
        // This step is double support, returning 0 hip hack
        return 0.0f;
    }
    const float support_sign = (state !=SWINGING? 1.0f : -1.0f);
    const float absFootAngle = std::abs(footAngleZ);


    //Calculate the compensation to the HIPROLL
    float MAX_HIP_ANGLE_OFFSET = (hack_chain == LLEG_CHAIN ?
                                  gait->hack[WP::L_HIP_AMP]:
                                  gait->hack[WP::R_HIP_AMP]);

    // the swinging leg will follow a trapezoid in 3-d. The trapezoid has
    // three stages: going up, a level stretch, going back down to the ground
    static int stage;
    if (firstFrame()) stage = 0;

    float hr_offset = 0.0f;

    if (stage == 0) { // we are rising
        // we want to raise the foot up for the first third of the step duration
        hr_offset = MAX_HIP_ANGLE_OFFSET*
            static_cast<float>(frameCounter) /
            (static_cast<float>(singleSupportFrames)/3.0f);
        if (frameCounter >= (static_cast<float>(singleSupportFrames)
			     / 3.0f) )
            stage++;

    }
    else if (stage == 1) { // keep it level
        hr_offset  = MAX_HIP_ANGLE_OFFSET;

        if (frameCounter >= 2.* static_cast<float>(singleSupportFrames)/3)
            stage++;
    }
    else {// stage 2, set the foot back down on the ground
        hr_offset = max(0.0f,
                        MAX_HIP_ANGLE_OFFSET*
                        static_cast<float>(singleSupportFrames
                                           -frameCounter)/
                        ( static_cast<float>(singleSupportFrames)/
			  3.0f) );
    }

    //we've calcuated the correct magnitude, but need to adjust for specific
    //hip motor angle direction in this leg
    //AND we also need to rotate some of the correction to the hip pitch motor
    // (This is kind of a HACK until we move the step lifting to be taken
    // directly into account when we determine x,y 3d targets for each leg)
    const float hipPitchAdjustment = -hr_offset * std::sin(footAngleZ);
    const float hipRollAdjustment = support_sign*(hr_offset *
						  static_cast<float>(leg_sign)*
						  std::cos(footAngleZ) );

    return boost::tuple<const float, const float> (hipPitchAdjustment,
                                                   hipRollAdjustment);
    //return leg_sign*hr_offset;
}

/**
 * Determine the stiffness for all the joints in the leg at the current point
 * in the gait cycle. Currently, the stiffnesses are static throughout the gait
 * cycle
 */
const vector<float> WalkingLeg::getStiffnesses(){

    //get shorter names for all the constants
    const float maxS = gait->stiffness[WP::HIP];
    const float anklePitchS = gait->stiffness[WP::AP];
    const float ankleRollS = gait->stiffness[WP::AR];
    const float kneeS = gait->stiffness[WP::KP];

    float stiffnesses[LEG_JOINTS] = {maxS, maxS, maxS,
                                     kneeS,anklePitchS,ankleRollS};
    vector<float> stiff_result = vector<float>(stiffnesses,
                                               &stiffnesses[LEG_JOINTS]);
    return stiff_result;

}


inline ChainID WalkingLeg::getOtherLegChainID(){
    return (chainID==LLEG_CHAIN ? 
            RLEG_CHAIN : LLEG_CHAIN);
}


void WalkingLeg::computeOdoUpdate() {
    const ufvector3 diff = goal-last_goal;
    const float xCOMMovement = -diff(0);
    const float yCOMMovement = -diff(1);
    const float thetaCOMMovement = 0; // set elsewhere

    odometry->update(    xCOMMovement * gait->odo[WP::X_SCALE],
		         yCOMMovement * gait->odo[WP::Y_SCALE],
		     thetaCOMMovement * gait->odo[WP::THETA_SCALE]);
}

/**
 *  STATIC!! method to get angles from a goal, and the components of walking params
 */
vector<float>
WalkingLeg::getAnglesFromGoal(const ChainID chainID,
                              const ufvector3 & goal,
                              const float stance[WP::LEN_STANCE_CONFIG]){

    const float sign = (chainID == LLEG_CHAIN ? 1.0f : -1.0f);

    const ufvector3 body_orientation =
	CoordFrame3D::vector3D(0.0f,
			       stance[WP::BODY_ROT_Y],
			       0.0f);

    const ufvector3 foot_orientation =
	CoordFrame3D::vector3D(0.0f,
			       0.0f,
			       static_cast<float>(sign) *
			       stance[WP::LEG_ROT_Z]*0.5f);
    const ufvector3 body_goal =
	CoordFrame3D::vector3D(0.0f,0.0f,0.0f);


    IKLegResult result = Kinematics::legIK(chainID,
					   goal,
					   foot_orientation,
					   body_goal,
					   body_orientation);
    return  vector<float>(result.angles,&result.angles[LEG_JOINTS]);

}


/**
 * Assuming this is the support foot, then we can return how far we have moved
 */
vector<float> WalkingLeg::getOdoUpdate(){
    std::vector<float> odo = odometry->getOdometry();

#ifdef POOPIES
    static int counter;
    cout << counter++ << " , " << odo[0] << " , " << odo[1] << " , "
	 << odo[2] << endl;
#endif

    return odo;
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
        return frameCounter >= singleSupportFrames;
    case SWINGING:
        return frameCounter >= singleSupportFrames;
    case DOUBLE_SUPPORT:
        return frameCounter >= doubleSupportFrames;
    case PERSISTENT_DOUBLE_SUPPORT:
        return frameCounter >= doubleSupportFrames;
    }

    throw "Non existent state";
    return false;
}

void WalkingLeg::switchToNextState(){
    setState(nextState());
}

void WalkingLeg::setState(SupportMode newState){
    state = newState;
    frameCounter = 0;
    if(state == PERSISTENT_DOUBLE_SUPPORT ||
       state == DOUBLE_SUPPORT)
        lastRotation = -lastRotation;
}

void WalkingLeg::assignStateTimes(boost::shared_ptr<Step> step){
    doubleSupportFrames = step->doubleSupportFrames;
    singleSupportFrames = step->singleSupportFrames;
    cycleFrames = step->stepDurationFrames;
}

void WalkingLeg::debugProcessing(ufmatrix3 fc_Transform){
#ifdef DEBUG_WALKING_STATE_TRANSITIONS
    if (firstFrame()){
        if(chainID == LLEG_CHAIN){
            cout<<"Left leg "
		}else{
            cout<<"Right leg "
		}
	if(state == SUPPORTING)
	    cout <<"switched into single support"<<endl;
	else if(state== DOUBLE_SUPPORT || state == PERSISTENT_DOUBLE_SUPPORT)
	    cout <<"switched into double support"<<endl;
	else if(state == SWINGING)
	    cout << "switched into swinging."<<endl;
    }
#endif

//#define DEBUG_WALKING_GOAL_CONTINUITY
#ifdef DEBUG_WALKING_GOAL_CONTINUITY
    ufvector3 diff = goal - last_goal;
#define GTHRSH 3

    static int counter;
    ++counter;

    if(diff(0) > GTHRSH || diff(1) > GTHRSH ){
        if(chainID == LLEG_CHAIN){
            cout << "Left leg ";
        }else
            cout << "Right leg ";

        cout << "noticed a big jump from last frame" << endl
	     << " difference: " << diff
	     << "  from: "<< last_goal << endl
	     << "  to: "<< goal << endl
	     << " F->C Transform: " << endl << fc_Transform << endl;
    }
#endif

//DANGER/HACK these static timers probably get incremented twice per frame
//once in th left leg, and once in the right leg

#ifdef DEBUG_WALKING_LOCUS_LOGGING
    static float ttime= 0.0f;
    fprintf(locus_log,"%f\t%f\t%f\t%f\t%d\n",ttime,goal(0),goal(1),goal(2),state);
    ttime += MOTION_FRAME_LENGTH_S;
#endif
#ifdef DEBUG_WALKING_DEST_LOGGING
    static float stime= 0.0f;
    fprintf(dest_log,"%f\t%f\t%f\t%f\t%f\t%d\n",stime,
            cur_dest->x,cur_dest->y,
            swing_src->x,swing_src->y,state);
    stime += MOTION_FRAME_LENGTH_S;
#endif
#ifdef DEBUG_WALKING_SENSOR_LOGGING
    static float sentime= 0.0f;
    Inertial inertial = sensors->getInertial();
    fprintf(sensor_log,
            "%f\t%f\t"
            "%f\t%f\t"
            "%f\t%f\t"
            "%f\t%d\n",sentime,
            0.0f,gait->stance[WP::BODY_ROT_Y],
            sensorAngleX,sensorAngleY,
            inertial.angleX,inertial.angleY,
            state);
    sentime += MOTION_FRAME_LENGTH_S;
#endif
}
