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

#include <iostream>
#include <iomanip>
using namespace std;

#include <math.h>
#include <boost/shared_ptr.hpp>
#include <boost/assign/std/vector.hpp>
using namespace boost::assign;
using boost::shared_ptr;

#include "StepGenerator.h"
#include "NBMath.h"
#include "Observer.h"
#include "BasicWorldConstants.h"
#include "COMKinematics.h"
#include "JointMassConstants.h"

using namespace boost::numeric;
using namespace Kinematics;
using namespace NBMath;

//#define DEBUG_STEPGENERATOR
//#define DEBUG_ZMP
//#define DEBUG_ZMP_REF
//#define DEBUG_COM_TRANSFORMS
//#define DEBUG_DESTINATION

StepGenerator::StepGenerator(boost::shared_ptr<Sensors> s,
                             boost::shared_ptr<NaoPose> p,
                             const MetaGait * _gait)
    : x(0.0f), y(0.0f), theta(0.0f),
      done(true),
      hasDestination(false),
      brokenSensorWarning(false),
      sensorAngles(s,_gait),
      com_i(CoordFrame3D::vector3D(0.0f,0.0f)),
      joints_com_i(CoordFrame3D::vector3D(0.0f,0.0f)),
      com_f(CoordFrame3D::vector3D(0.0f,0.0f)),
      est_zmp_i(CoordFrame3D::vector3D(0.0f,0.0f)),
      zmp_ref_x(list<float>()),zmp_ref_y(list<float>()),
      futureSteps(),
      currentZMPDSteps(),
      si_Transform(CoordFrame3D::identity3D()),
      last_zmp_end_s(CoordFrame3D::vector3D(0.0f,0.0f)),
      if_Transform(CoordFrame3D::identity3D()),
      fi_Transform(CoordFrame3D::identity3D()),
      fc_Transform(CoordFrame3D::identity3D()),
      cf_Transform(CoordFrame3D::identity3D()),
      cc_Transform(CoordFrame3D::identity3D()),
      lastRotation(0), avgStepRotation(0), dThetaPerMotionFrame(0),
      xOdoFilter(Boxcar(1)),
      sensors(s), pose(p), gait(_gait), nextStepIsLeft(true),
      leftLeg(s,gait,&sensorAngles, LLEG_CHAIN),
      rightLeg(s,gait,&sensorAngles, RLEG_CHAIN),
      leftArm(gait,LARM_CHAIN), rightArm(gait,RARM_CHAIN),
      supportFoot(LEFT_SUPPORT),
      controller_x(new Observer()),
      controller_y(new Observer()),
      zmp_filter(),
      acc_filter(),
      accInWorldFrame(CoordFrame4D::vector4D(0.0f,0.0f,0.0f))
{
    //COM logging
#ifdef DEBUG_CONTROLLER_COM
    com_log = fopen("/tmp/com_log.xls","w");
    fprintf(com_log,"time\tcom_x\tcom_y\tpre_x\tpre_y\tzmp_x\tzmp_y\t"
            "sensor_zmp_x\tsensor_zmp_y\tekf_zmp_x\tekf_zmp_y\t"
            "real_com_x\treal_com_y\tjoints_com_x\tjoints_com_y\t"
            "angleX\tangleY\taccX\taccY\taccZ\t"
            "lfl\tlfr\tlrl\tlrr\trfl\trfr\trrl\trrr\t"
            "state\n");
#endif
#ifdef DEBUG_SENSOR_ZMP
    zmp_log = fopen("/tmp/zmp_log.xls","w");
    fprintf(zmp_log,"time\tpre_x\tpre_y\tcom_x\tcom_y\tcom_px\tcom_py"
            "\taccX\taccY\taccZ\t"
            "ekf_zmp_x\tekf_zmp_y\t"
            "angleX\tangleY\n");
#endif
#ifdef DEBUG_ZMP_REF
    zmp_ref_log = fopen("/tmp/zmp_ref_log.xls","w");
#endif

}

StepGenerator::~StepGenerator()
{
#ifdef DEBUG_CONTROLLER_COM
    fclose(com_log);
#endif
#ifdef DEBUG_SENSOR_ZMP
    fclose(zmp_log);
#endif
#ifdef DEBUG_ZMP_REF
    fclose(zmp_ref_log);
#endif
    delete controller_x; delete controller_y;
}

void StepGenerator::resetHard(){
    //When we fall, or other bad things happen, we just want to stop asap
    //Clear all zmp, clear footsteps
    resetQueues();
    done  = true;
    //I don't think there is much else we need to do....
}

/**
 * Central method to get the previewed zmp_refernce values
 * In the process of getting these values, this method handles the following:
 *
 *  * Handles transfer from futureSteps list to the currentZMPDsteps list.
 *    When the Future ZMP values we want run out, we pop the next future step
 *    add generated ZMP from it, and put it into the ZMPDsteps List
 *
 *  * Ensures that there are NUM_PREVIEW_FRAMES + 1 frames in the zmp lists.
 *    the oldest value will be popped off before the list is sent to the
 *    controller.
 *
 */
zmp_xy_tuple StepGenerator::generate_zmp_ref() {
    //Generate enough ZMPs so a) the controller can run
    //and                     b) there are enough steps
    while (zmp_ref_y.size() <= Observer::NUM_PREVIEW_FRAMES ||
           // VERY IMPORTANT: make sure we have enough ZMPed steps
           currentZMPDSteps.size() < MIN_NUM_ENQUEUED_STEPS) {
        if (futureSteps.size() == 0) {
	    if (hasDestination) {
		x = y = theta = 0; // stop the robot
		hasDestination = false;
	    }

	    // replenish with the current walk vector, when we don't have a destination
            generateStep(x, y, theta);
        }
        else {
            Step::ptr nextStep = futureSteps.front();
            futureSteps.pop_front();

            fillZMP(nextStep);
            //transfer the nextStep element from future to current list
            currentZMPDSteps.push_back(nextStep);
        }
#ifdef DEBUG_ZMP
        cout << "generate_zmp_ref()\n";
        list<float>::iterator it;
        cout << "zmp_ref_x: " << zmp_ref_x.size();
        for (it=zmp_ref_x.begin(); it!=zmp_ref_x.end(); ++it)
            cout << " " << *it;
        cout << "\n";

        cout << " zmp_ref_y: " << zmp_ref_y.size();
        for (it=zmp_ref_y.begin(); it!=zmp_ref_y.end(); ++it)
            cout << " " << *it;
        cout << "\n";
#endif
    }

    return zmp_xy_tuple(&zmp_ref_x, &zmp_ref_y);
}

/**
 * This method calculates the sensor ZMP. We build a body to world
 * transform using Aldebaran's filtered angleX/angleY. We then use
 * this to rotate the EKF-filtered accX/Y/Z from the
 * accelerometers. The transformed values are fed into an exponential
 * filter (acc_filter, to reduce jitter from the rotation), and the
 * filtered values are used in an EKF that maintains our sensor ZMP
 * (zmp_filter). The ZMP EKF also takes in the CoM as calculated by
 * the joint angles of the robot (see JointMassConstants.h and
 * COKKinematics.cpp for implementation details of that)
 */
void StepGenerator::findSensorZMP(){
    const Inertial inertial = sensors->getInertial();

    //The robot may or may not be tilted with respect to vertical,
    //so, since walking is conducted from a bird's eye perspective
    //we would like to rotate the sensor measurements appropriately.
    //We will use angleX, and angleY:
    const ufmatrix4 bodyToWorldTransform =
        prod(CoordFrame4D::rotation4D(CoordFrame4D::X_AXIS, -inertial.angleX),
             CoordFrame4D::rotation4D(CoordFrame4D::Y_AXIS, -inertial.angleY));

    // update the IIR filter
    acc_filter.update(inertial.accX,
                      inertial.accY,
                      inertial.accZ);

    const ufvector4 accInBodyFrame = CoordFrame4D::vector4D(acc_filter.getX(),
                                                            acc_filter.getY(),
                                                            acc_filter.getZ());
    // and rotate the filtered acceleration
    accInWorldFrame = prod(bodyToWorldTransform,
                           accInBodyFrame);

    //cout << endl<< "########################"<<endl;
    //cout << "Accel in body  frame: "<< accInBodyFrame <<endl;
    //cout << "Accel in world frame: "<< accInWorldFrame <<endl;
    //cout << "Angle X is "<< inertial.angleX << " Y is" <<inertial.angleY<<endl;

    //Rotate from the local C to the global I frame
    const ufvector3 accel_c = CoordFrame3D::vector3D(accInWorldFrame(0),
                                                     accInWorldFrame(1));
    const float angle_fc = safe_asin(fc_Transform(1,0));
    const float angle_if = safe_asin(if_Transform(1,0));
    const float tot_angle = -(angle_fc+angle_if);
    const ufvector3 accel_i = prod(CoordFrame3D::rotation3D(CoordFrame3D::Z_AXIS,
                                                            tot_angle),
                                   accel_c);
    // translate com_c (from joint angles) to I frame
    const ufvector4 com_c_xyz = getCOMc(sensors->getMotionBodyAngles());
    const ufvector3 joints_com_c = CoordFrame3D::vector3D(com_c_xyz(0), com_c_xyz(1));
    const ufvector3 joints_com_f = prod(cf_Transform, joints_com_c);
    joints_com_i = prod(fi_Transform, joints_com_f);
    const float joint_com_i_x = joints_com_i(0);
    const float joint_com_i_y = joints_com_i(1);

    ZmpTimeUpdate tUp = {controller_x->getZMP(), controller_y->getZMP()};
    ZmpMeasurement pMeasure =
    //{joint_com_i_x, (joint_com_i_y + COM_I_Y_OFFSET),
	{controller_x->getPosition(), controller_y->getPosition(),
	 accel_i(0), accel_i(1)};
    zmp_filter.update(tUp,pMeasure);

#ifdef DEBUG_COM_TRANSFORMS
    cout << "raw " << com_c_xyz
	 << " controller_x com: " << controller_x->getPosition()
	 << " com_c -> com_i (x): " << joint_com_i_x
	 << " controller_y com: " << controller_y->getPosition()
	 << " com_c -> com_i (y): " << joint_com_i_y + COM_I_Y_OFFSET
	 << endl;
#endif
}

float StepGenerator::scaleSensors(const float sensorZMP,
                                  const float perfectZMP) {
    // TODO: find a better value for this!
    float sensorWeight = 0.4f; //gait->sensor[WP::OBSERVER_SCALE];

    // If our motion sensors are broken, we don't want to use the observer
    if (brokenSensorWarning || sensors->angleXYBroken()) {
	sensorWeight = 0.0f;

	// TODO: signal Python, so the robot can fall back to a slower gait

	if (!brokenSensorWarning) {
	    brokenSensorWarning = true;
	    cout << "*********** WARNING WARNING **********************" << endl
		 << "Too many motion sensors are broken, disabling the observer" << endl
		 << "*********** WARNING WARNING **********************" << endl;
	}
    }

    return sensorZMP*sensorWeight + (1.0f - sensorWeight)*perfectZMP;
}

/**
 *  This method gets called by the walk provider to generate the output
 *  of the ZMP controller. It generates the com location in the I frame
 */
void StepGenerator::tick_controller(){
#ifdef DEBUG_STEPGENERATOR
    cout << "StepGenerator::tick_controller" << endl;
#endif

    // update the acc/gyro based ZMP (in (ZmpEKF)zmp_filter)
    findSensorZMP();

    zmp_xy_tuple zmp_ref = generate_zmp_ref();

#ifdef DEBUG_ZMP
    cout << "StepGenerator::generate_zmp_ref() finished\n";
#endif

    //The observer needs to know the current reference zmp
    const float cur_zmp_ref_x =  zmp_ref_x.front();
    const float cur_zmp_ref_y = zmp_ref_y.front();
    //clear the oldest (i.e. current) value from the preview list
    zmp_ref_x.pop_front();
    zmp_ref_y.pop_front();

    //Scale the sensor feedback according to the gait parameters
    est_zmp_i(0) = scaleSensors(zmp_filter.get_zmp_x(), cur_zmp_ref_x);
    est_zmp_i(1) = scaleSensors(zmp_filter.get_zmp_y(), cur_zmp_ref_y);

    //Tick the controller (input: ZMPref, sensors -- out: CoM x, y)

    const float com_x = controller_x->tick(zmp_ref.get<0>(),cur_zmp_ref_x,
                                           est_zmp_i(0));

    const float com_y = controller_y->tick(zmp_ref.get<1>(),cur_zmp_ref_y,
                                           est_zmp_i(1));
    com_i = CoordFrame3D::vector3D(com_x,com_y);
}

/** Central method for moving the walking legs. It handles important stuff like:
 *
 *  * Switching support feet
 *  * Updating the coordinate transformations from i to f and f to c
 *  * Keep running track of the f locations of the following 'footholds':
 *    - the support foot (supp_pos_f), which depends on the first step in the
 *      ZMPDSteps queue, but is always at the origin in f frame, by definition
 *    - the source of the swinging foot (swing_src_f), which is the location
 *      where the now swinging foot was once the supporting foot
 *    - the destination of the swinging foot (swing_pos_f), which depends on
 *      the second step in the ZMPDSteps queue
 *  * Handles poping from the ZMPDStep list when we switch support feet
 */


WalkLegsTuple StepGenerator::tick_legs(){
#ifdef DEBUG_STEPGENERATOR
    cout << "StepGenerator::tick_legs" << endl;
#endif
    sensorAngles.tick_sensors();
    //Decide if this is the first frame into any double support phase
    //which is the critical point when we must swap coord frames, etc
    if(leftLeg.isSwitchingSupportMode() && leftLeg.stateIsDoubleSupport()){
        swapSupportLegs();
    }

    //cout << "Support step is " << *supportStep_f <<endl;
    //hack-ish for now to do hyp pitch crap
    leftLeg.setSteps(swingingStepSource_f, swingingStep_f,supportStep_f);
    rightLeg.setSteps(swingingStepSource_f, swingingStep_f,supportStep_f);

    //Each frame, we must recalculate the location of the center of mass
    //relative to the support leg (f coord frame), based on the output
    //of the controller (in tick_controller() )
    com_f = prod(if_Transform,com_i);

    //We want to get the incremental rotation of the center of mass
    //we need to ask one of the walking legs to give it:
    const float body_rot_angle_fc = leftLeg.getFootRotation()/2; //relative to f

    //Using the location of the com in the f coord frame, we can calculate
    //a transformation matrix to go from f to c
    fc_Transform = prod(CoordFrame3D::rotation3D(CoordFrame3D::Z_AXIS,
                                                 body_rot_angle_fc),
                        CoordFrame3D::translation3D(-com_f(0),-com_f(1)));
    // and oppositely, a transform from c to f
    cf_Transform = prod(CoordFrame3D::translation3D(com_f(0),com_f(1)),
                        CoordFrame3D::rotation3D(CoordFrame3D::Z_AXIS,
                                                 -body_rot_angle_fc));

    //Now we need to determine which leg to send the coorect footholds/Steps to
    Step::ptr leftStep_f,rightStep_f;
    //First, the support leg.
    if (supportStep_f->foot == LEFT_FOOT){
        leftStep_f = supportStep_f;
        rightStep_f = swingingStep_f;
    }
    else{
        rightStep_f = supportStep_f;
        leftStep_f = swingingStep_f;
    }

    //Since we'd like to ignore the state information of the WalkinLeg as much
    //as possible, we send in the source of the swinging leg to both, regardless
    LegJointStiffTuple left  = leftLeg.tick(leftStep_f,swingingStepSource_f,
					    swingingStep_f,fc_Transform);
    LegJointStiffTuple right = rightLeg.tick(rightStep_f,swingingStepSource_f,
					     swingingStep_f,fc_Transform);

    updateOdometry();

    //HACK check to see if we are done - still too soon, but works! (see graphs)
    if(supportStep_s->type == END_STEP && swingingStep_s->type == END_STEP
       && lastStep_s->type == END_STEP &&
       // Check that the walk vector is currently all 0s
       x == 0.0f && y == 0.0f && theta == 0.0f) {
        //cout << "step generator done = true" << endl;
        done = true;
    }

    debugLogging();
#ifdef DEBUG_STEPGENERATOR
    cout << "StepGenerator::tick_legs DONE" << endl;
#endif

    return WalkLegsTuple(left,right);
}

/**
 * This method handles updating all the necessary coordinate frames and steps
 * when the support feet change
 */
void StepGenerator::swapSupportLegs(){
    if (currentZMPDSteps.size() +  futureSteps.size() <
	MIN_NUM_ENQUEUED_STEPS)
	throw "Insufficient steps";

    //there are at least three elements in the list, pop the obsolete one
    //(currently use last step to determine when to stop, hackish-ish)
    //and the first step is the support one now, the second the swing
    lastStep_s = *currentZMPDSteps.begin();
    currentZMPDSteps.pop_front();
    swingingStep_s  = *(++currentZMPDSteps.begin());
    supportStep_s   =  *currentZMPDSteps.begin();

    supportFoot = (supportStep_s->foot == LEFT_FOOT ?
		   LEFT_SUPPORT : RIGHT_SUPPORT);

    //update the translation matrix between i and f coord. frames
    ufmatrix3 stepTransform = get_fprime_f(supportStep_s);
    if_Transform = prod(stepTransform,if_Transform);
    update_FtoI_transform();

    //Express the  destination  and source for the supporting foot and
    //swinging foots locations in f coord. Since the supporting foot doesn't
    //move, we ignore its source.

    //First, do the support foot, which is always at the origin
    const ufvector3 origin = CoordFrame3D::vector3D(0,0);
    const ufvector3 supp_pos_f = origin;

    //Second, do the source of the swinging leg, which can be calculated
    //using the stepTransform matrix from above
    ufvector3 swing_src_f = prod(stepTransform,origin);


    //Third, do the dest. of the swinging leg, which is more complicated
    //We get the translation matrix that takes points in next f-type
    //coordinate frame, namely the one that will be centered at the swinging
    //foot's destination, and puts them into the current f coord. frame
    const ufmatrix3 swing_reverse_trans =
	get_f_fprime(swingingStep_s);
    //This gives us the position of the swinging foot's destination
    //in the current f frame
    const ufvector3 swing_pos_f = prod(swing_reverse_trans,
				       origin);

    //finally, we need to know how much turning there will be. Turns out,
    //we can simply read this out of the aforementioned translation matr.
    //this only works because its a 3D homog. coord matr - 4D would break
    float swing_dest_angle = -safe_asin(swing_reverse_trans(1,0));

    /* save the rotation of this step, so we can use it to update odometry
     * this is kind of a HACK but it gives us a dTheta per motion frame
     * that is proportional to our actual dTheta, and in the correct direction.
     *
     * To make it better, look at the odometry methods inside WalkingLeg and use
     * the matrices there possibly?
     *
     * NOTE: The sign of the rotation is switched to get our movement from the
     * angle, since it comes from the reverse transformation.
     */
    const float rotationThisStep = -1*swing_dest_angle;
    //cout << "step rotation " << rotationThisStep;

    avgStepRotation = (lastRotation + rotationThisStep)*0.5f;
    lastRotation = rotationThisStep;

    dThetaPerMotionFrame = avgStepRotation /
	static_cast<float>(lastQueuedStep->stepDurationFrames);

    //cout << " rotation per motion frame " << dThetaPerMotionFrame << endl;

    //we use the swinging source to calc. a path for the swinging foot
    //it is not clear now if we will need to angle offset or what
    float swing_src_angle = -safe_asin(stepTransform(1,0));

    //in the F coordinate frames, we express Steps representing
    // the three footholds from above
    supportStep_f =
	Step::ptr(new Step(supp_pos_f(0),supp_pos_f(1),
			   0.0f,*supportStep_s));
    swingingStep_f =
	Step::ptr(new Step(swing_pos_f(0),swing_pos_f(1),
			   swing_dest_angle,*swingingStep_s));
    swingingStepSource_f  =
	Step::ptr(new Step(swing_src_f(0),swing_src_f(1),
			   swing_src_angle,*lastStep_s));

}

/**
 *  This method fills the ZMP queue with extra zmp based on a step
 *  Note the bug that currently exists with this process (in the timing)
 *  See the header and the README.tex/pdf
 *
 *  There are two types of ZMP patterns for when a step is supporting
 *    - Regular, where there is another step coming after
 *    - End, where the ZMP should move directly under the robot (origin of S)
 */
void StepGenerator::fillZMP(const Step::ptr newSupportStep ){
    switch(newSupportStep->type){
    case REGULAR_STEP:
        fillZMPRegular(newSupportStep);
        break;
    case END_STEP: //END and NULL might be the same thing....?
        fillZMPEnd(newSupportStep);
        break;
    default:
        throw "Unsupported Step type";
    }

    newSupportStep->zmpd = true;
}

/**
 * Generates the ZMP reference pattern for a normal step
 */
void StepGenerator::fillZMPRegular(const Step::ptr newSupportStep ){
    //update the lastZMPD Step
    const float sign = (newSupportStep->foot == LEFT_FOOT ? 1.0f : -1.0f);
    const float last_sign = -sign;

    //The intent of this constant is to be approximately the length of the foot
    //and corresponds to the distance we would like the ZMP to move along the
    //single-support-foot (say 80mm or so?). Might not want it to be linearly
    //interpolated either - maybe stay at a point a bit and then  move in a line
    //HACK/ HOWEVER - the best value for this constant is about -20 right now
    //This could have two+ reasons:
    //1) The controller is inaccurate in getting ref and actual zmp to line up
    //   (In fact, we know this is the case, from the debug graphs. but, what we
    //    dont know is if this is the definite cause of instability)
    //2) The approximations made in the simple PreviewController are finally
    //   hurting us. This could be fixed with an observer
    // in anycase, we'll leave this at -20 for now. (The effect is that
    // the com path 'pauses' over the support foot, which is quite nice)
    float X_ZMP_FOOT_LENGTH = 0.0f;// HACK/TODO make this center foot gait->footLengthX;

    // An additional HACK:
    // When we are turning, we have this problem that the direction in which
    // we turn, the opening step is well balanced but the step which brings the
    // foot back is bad. We need to swing more toward the opening step in
    // order to not fall inward.
    const float HACK_AMOUNT_PER_PI_OF_TURN =
        newSupportStep->zmpConfig[WP::TURN_ZMP_OFF];
    const float HACK_AMOUNT_PER_1_OF_LATERAL =
        newSupportStep->zmpConfig[WP::STRAFE_ZMP_OFF];

    float adjustment = ((newSupportStep->theta / M_PI_FLOAT)
                        * HACK_AMOUNT_PER_PI_OF_TURN);
    adjustment += (newSupportStep->y - (sign*HIP_OFFSET_Y))
        * HACK_AMOUNT_PER_1_OF_LATERAL;

    //Another HACK (ie. zmp is not perfect)
    //This moves the zmp reference to the outside of the foot
    float Y_ZMP_OFFSET = (newSupportStep->foot == LEFT_FOOT ?
                          newSupportStep->zmpConfig[WP::L_ZMP_OFF_Y]:
                          newSupportStep->zmpConfig[WP::R_ZMP_OFF_Y]);

    Y_ZMP_OFFSET += adjustment;

    // When we turn, the ZMP offset needs to be corrected for the rotation of
    // newSupportStep. A picture would be very useful here. Someday...
    float y_zmp_offset_x = -sin(std::abs(newSupportStep->theta)) * Y_ZMP_OFFSET;
    float y_zmp_offset_y = cos(newSupportStep->theta) * Y_ZMP_OFFSET;

    //lets define the key points in the s frame. See diagram in paper
    //to use bezier curves, we would need also directions for each point
    const ufvector3 start_s = last_zmp_end_s;
    const ufvector3 end_s =
        CoordFrame3D::vector3D(newSupportStep->x +
                               gait->stance[WP::BODY_OFF_X] +
                               y_zmp_offset_x,
                               newSupportStep->y + sign*y_zmp_offset_y);
    const ufvector3 mid_s =
        CoordFrame3D::vector3D(newSupportStep->x +
                               gait->stance[WP::BODY_OFF_X] +
                               y_zmp_offset_x - X_ZMP_FOOT_LENGTH,
                               newSupportStep->y + sign*y_zmp_offset_y);

    const ufvector3 start_i = prod(si_Transform,start_s);
    const ufvector3 mid_i = prod(si_Transform,mid_s);
    const ufvector3 end_i = prod(si_Transform,end_s);

    //Now, we interpolate between the three points. The line between
    //start and mid is double support, and the line between mid and end
    //is double support

    //double support - consists of 3 phases:
    //  1) a static portion at start_i
    //  2) a moving (diagonal) portion between start_i and mid_i
    //  3) a static portion at start_i
    //The time is split between these phases according to
    //the constant gait->dblSupInactivePercentage

    //First, split up the frames:
    const int halfNumDSChops = //DS - DoubleStaticChops
	static_cast<int>(static_cast<float>(newSupportStep->doubleSupportFrames)*
			 newSupportStep->zmpConfig[WP::DBL_SUP_STATIC_P]/2.0f);
    const int numDMChops = //DM - DoubleMovingChops
        newSupportStep->doubleSupportFrames - halfNumDSChops*2;

    //Phase 1) - stay at start_i
    for(int i = 0; i< halfNumDSChops; i++){
        zmp_ref_x.push_back(start_i(0));
        zmp_ref_y.push_back(start_i(1));
    }

    //phase 2) - move from start_i to
    for(int i = 0; i< numDMChops; i++){
        ufvector3 new_i = start_i +
            (static_cast<float>(i)/
             static_cast<float>(numDMChops) ) *
            (mid_i-start_i);

        zmp_ref_x.push_back(new_i(0));
        zmp_ref_y.push_back(new_i(1));
    }

    //phase 3) - stay at mid_i
    for(int i = 0; i< halfNumDSChops; i++){
        zmp_ref_x.push_back(mid_i(0));
        zmp_ref_y.push_back(mid_i(1));
    }


    //single support -  we want to stay over the new step
    const int numSChops = newSupportStep->singleSupportFrames;
    for(int i = 0; i< numSChops; i++){
//    const int numSChops = gait->stepDurationFrames;
//    for(int i = 0; i< gait->stepDurationFrames; i++){

        ufvector3 new_i = mid_i +
            (static_cast<float>(i) /
             static_cast<float>(numSChops) ) *
            (end_i-mid_i);

        zmp_ref_x.push_back(new_i(0));
        zmp_ref_y.push_back(new_i(1));
    }

    //update our reference frame for the next time this method is called
    si_Transform = prod(si_Transform,get_s_sprime(newSupportStep));
    //store the end of the zmp in the next s frame:
    last_zmp_end_s = prod(get_sprime_s(newSupportStep),end_s);
}

/**
 * Generates the ZMP reference pattern for a step when it is the support step
 * such that it will be the last step before stopping
 */
void StepGenerator::fillZMPEnd(const Step::ptr newSupportStep) {
    const ufvector3 end_s =
        CoordFrame3D::vector3D(gait->stance[WP::BODY_OFF_X],
                               0.0f);
    const ufvector3 end_i = prod(si_Transform,end_s);

    //Queue a starting step, where we step, but do nothing with the ZMP
    //so push tons of zero ZMP values
    for (unsigned int i = 0; i < newSupportStep->stepDurationFrames; i++){
        zmp_ref_x.push_back(end_i(0));
        zmp_ref_y.push_back(end_i(1));
    }

    //An End step should never move the si_Transform!
    //si_Transform = prod(si_Transform,get_s_sprime(newSupportStep));
    //store the end of the zmp in the next s frame:
    last_zmp_end_s = prod(get_sprime_s(newSupportStep),end_s);
}

/**
 * Set the speed of the walk eninge in mm/s and rad/s
 */
void StepGenerator::setSpeed(const float _x, const float _y,
			     const float _theta)  {

    //Regardless, we are changing the walk vector, so we need to scrap any future plans
    clearFutureSteps();
    hasDestination = false;

    x = _x;
    y = _y;
    theta = _theta;

#ifdef DEBUG_STEPGENERATOR
    cout << "New Walk Vector is:" << endl
         << "    x: " << x << " y: " << y << " theta: " << theta << endl;
    cout << "Are we done? " << (done ? "Yes" : "No") << endl;
#endif

    if(done){

#ifdef DEBUG_STEPGENERATOR
        cout << "The walk engine was previously inactive, so we need to do extra"
            " work in stepGen::setSpeed()"<<endl;
#endif

        //we are starting fresh from a stopped state, so we need to clear all remaining
        //steps and zmp values.
        resetQueues();

        //then we need to pick which foot to start with
        const bool startLeft = decideStartLeft(y,theta);
        resetSteps(startLeft);
    }
    done = false;

}

/**
 * Move the robot from it's current position to the destionation rel_x,
 * rel_y, rel_theta on the field. This method will move at the highest speed
 * possible, based on StepGenerator's current x,y,theta speeds or our gait's
 * maximum speeds (if setSpeed hasn't been called)
 *
 * Method will generate steps for an arbitrary relative destination, but NOTE
 * that due to slipping, model imperfections etc. it is most accurate for distances
 * of <30cm.
 *
 * @param gain optional speed modification parameter, range [0,1]
 */
int StepGenerator::setDestination(float dest_x, float dest_y, float dest_theta,
				   float gain) {
#ifdef DEBUG_DESTINATION
    cout << "StepGenerator::setDestination() destination x=" << dest_x
         << " y=" << dest_y << " theta=" << dest_theta << endl;
#endif

    using std::abs;
    using NBMath::sign;

    // sanity
    if (gain <= 0.0f || gain > 1.0f) {
        cout << "StepGenerator::setDestination() :: bad gain argument\n";
        gain = 1.0f;
    }

    if (dest_x == 0.0f && dest_y == 0.0f && dest_theta == 0.0f) {
	setSpeed(0,0,0);
	return 0; // all done!
    }

    // these parameters determined experimentally by trying lots of destinations
    // probably indicates something broken in our odometry
    // I personally apologize for this :-) --Nathan
    if (dest_x == 0)
	dest_x = -5.0f;
    dest_theta += 0.088f; // natural rotation of the robot
    dest_y *= 2; /// @see Step.h HACK HACK HACK

    float speed_x, speed_y, speed_theta;
    int framesToDestination = 0;

    // use the gait's maximum allowed x,y,theta
    if (dest_x > 0)
        speed_x = gain*gait->step[WP::MAX_VEL_X];
    else
        speed_x = gain*gait->step[WP::MIN_VEL_X];

    speed_y = gain*gait->step[WP::MAX_VEL_Y];
    speed_theta = gain*gait->step[WP::MAX_VEL_THETA];

    // now deal with the motion queues
    if (hasDestination || !done) {
	clearFutureSteps();

	// check the distances of any steps we've already commited to taking
	for (std::list<Step::ptr>::iterator step = currentZMPDSteps.begin();
	     step != currentZMPDSteps.end(); ++step)
	    countStepTowardsDestination(*step, dest_x, dest_y, dest_theta,
					framesToDestination);
    } else {
	resetQueues();
	const bool startLeft = decideStartLeft(dest_y,dest_theta);
	resetSteps(startLeft);
	generateStep(dest_x, dest_y, dest_theta);
    }
    hasDestination = true;
    done = false;


#ifdef DEBUG_DESTINATION
    cout << "destination after counting current zmp steps: x=" << dest_x
         << " y=" << dest_y << " theta=" << dest_theta << endl;
#endif

    const float CLOSE_ENOUGH_X_mm = 15.0f;
    const float CLOSE_ENOUGH_Y_mm = 15.0f;
    float CLOSE_ENOUGH_THETA_rad;

    // be more sensitive to rotation if we're going really far (40cm)
    if (dest_x*dest_x + dest_y*dest_y > 1600)
	CLOSE_ENOUGH_THETA_rad = 0.087f; // 5 degrees
    else
	CLOSE_ENOUGH_THETA_rad = 0.17f; // 10 degrees

    // loop until we get to our destination
    while (abs(dest_x) > CLOSE_ENOUGH_X_mm ||
	   abs(dest_y) > CLOSE_ENOUGH_Y_mm ||
	   abs(dest_theta) > CLOSE_ENOUGH_THETA_rad) {
	float step_x, step_y, step_theta;

	// check if we're close enough to our destination to make it this step
	if (abs(dest_x) > abs(speed_x))
	    step_x = speed_x;
	else if (abs(dest_x) <= CLOSE_ENOUGH_X_mm)
	    step_x = 0.0f;
	else
	    step_x = dest_x;

	if (abs(dest_y) > abs(speed_y))
	    step_y = speed_y * sign(dest_y);
	else if (abs(dest_y) <= CLOSE_ENOUGH_Y_mm)
	    step_y = 0.0f;
	else
	    step_y = dest_y;

	if (abs(dest_theta) > abs(speed_theta))
	    step_theta = speed_theta * sign(dest_theta);
	else if (abs(dest_theta) <= CLOSE_ENOUGH_THETA_rad)
	    step_theta = 0.0f;
	else
	    step_theta = dest_theta;

	// take the step, and use its odometry to update our progress towards dest
	generateStep(step_x, step_y, step_theta);

	countStepTowardsDestination(lastQueuedStep, dest_x, dest_y, dest_theta,
				    framesToDestination);

#ifdef DEBUG_DESTINATION
	cout << "created step: " << *lastQueuedStep << endl;
	printf("distance to destination is now %f %f %f\n",
	       dest_x, dest_y, dest_theta);
#endif
    }

#ifdef DEBUG_DESTINATION
    printf("\n final distance from dest: %f %f %f\n",
	   dest_x, dest_y, dest_theta);
    printf("Frames to destination: %d\n", framesToDestination);
#endif

    return framesToDestination;
}


// updates the destination based on how far this step went
void StepGenerator::countStepTowardsDestination(Step::ptr step, float& dest_x,
						float& dest_y, float &dest_theta,
						int& framesToDestination) {
    dest_x -= step->x;

    // necessary (HACK!) because steps will alternate +/- in Y and Theta
    if (sign(dest_y) == sign(step->y))
	dest_y -= step->y;

    if (sign(dest_theta) == sign(step->theta))
	dest_theta -= step->theta;

    framesToDestination += step->stepDurationFrames;
}


/**
 * Method to enqueue a specific number of steps and then stop
 * The input should be given in velocities (mm/s)
 */
void StepGenerator::takeSteps(const float _x, const float _y, const float _theta,
                              const int _numSteps){

#ifdef DEBUG_STEPGENERATOR
    cout << "takeSteps called with (" << _x << "," << _y<<","<<_theta
         <<") and with nsteps = "<<_numSteps<<endl;
#endif

    //Ensure that we are currently stopped -- if not, throw warning
    if(!done){
        cout<< "Warning!!! Step Command with (" << _x << "," << _y<<","<<_theta
            <<") and with "<<_numSteps<<" Steps were APPENDED because"
            "StepGenerator is already active!!" <<endl;
    }else{
	//we are starting fresh from a stopped state, so we need to clear all remaining
        //steps and zmp values.
        resetQueues();

        //then we need to pick which foot to start with
        const bool startLeft = decideStartLeft(_y,_theta);
        resetSteps(startLeft);

        //Adding this step is necessary because it was not added in the start left right
        generateStep(_x, _y, _theta);

        done = false;
    }

    for(int i =0; i < _numSteps; i++){
        generateStep(_x, _y, _theta);
    }

    //skip generating the end step, because it will be generated automatically:
    x = 0.0f; y =0.0f; theta = 0.0f;
}

/**
 *  Set up the walking engine for starting with a swinging step on the left,
 *  if startLeft is true
 */
void StepGenerator::resetSteps(const bool startLeft){
    //This is the place where we reset the controller each time the walk starts
    //over again.
    //First we reset the controller back to the neutral position
    controller_x->initState(gait->stance[WP::BODY_OFF_X],0.0f,
                            gait->stance[WP::BODY_OFF_X]);
    controller_y->initState(0.0f,0.0f,0.0f);

    //Each time we restart, we need to reset the estimated sensor ZMP:
    zmp_filter = ZmpEKF();

    sensorAngles.reset();

    //Third, we reset the memory of where to generate ZMP from steps back to
    //the origin
    si_Transform = CoordFrame3D::identity3D();
    last_zmp_end_s = CoordFrame3D::vector3D(0.0f,0.0f);


    Foot dummyFoot = LEFT_FOOT;
    Foot firstSupportFoot = RIGHT_FOOT;
    float supportSign = 1.0f;
    if(startLeft){
#ifdef DEBUG_STEPGENERATOR
        cout << "StepGenerator::startLeft"<<endl;
#endif

        //start off in a double support phase where the right leg swings first
        //HOWEVER, since the first support step is END, there will be no
        //actual swinging - the first actual swing will be 2 steps
        //after the first support step, in this case, causing left to swing first
        leftLeg.startRight();
        rightLeg.startRight();
        leftArm.startRight();
        rightArm.startRight();

        //depending on we are starting, assign the appropriate steps
        dummyFoot = RIGHT_FOOT;
        firstSupportFoot = LEFT_FOOT;
        supportSign = 1.0f;
        nextStepIsLeft = false;

    }else{ //startRight
#ifdef DEBUG_STEPGENERATOR
        cout << "StepGenerator::startRight"<<endl;
#endif

        //start off in a double support phase where the left leg swings first
        //HOWEVER, since the first support step is END, there will be no
        //actual swinging - the first actual swing will be 2 steps
        //after the first support step, in this case, causing right to swing first
        leftLeg.startLeft();
        rightLeg.startLeft();
        leftArm.startLeft();
        rightArm.startLeft();

        //depending on we are starting, assign the appropriate steps
        dummyFoot = LEFT_FOOT;
        firstSupportFoot = RIGHT_FOOT;
        supportSign = -1.0f;
        nextStepIsLeft = true;

    }

    //we need to re-initialize the if_Transform matrix to reflect which
    //side the we are starting.
    const ufmatrix3 initStart =
        CoordFrame3D::translation3D(0.0f,
                                    supportSign*(HIP_OFFSET_Y));
    if_Transform.assign(initStart);

    update_FtoI_transform();


    //When we start out again, we need to let odometry know to store
    //the distance covered so far. This needs to happen before
    //we reset any coordinate frames
    resetOdometry(gait->stance[WP::BODY_OFF_X],-supportSign*HIP_OFFSET_Y);

    //Support step is END Type, but the first swing step, generated
    //in generateStep, is REGULAR type.
    Step::ptr firstSupportStep =
	Step::ptr(new Step(ZERO_WALKVECTOR,
			   *gait,
			   firstSupportFoot,ZERO_WALKVECTOR,END_STEP));
    Step::ptr dummyStep =
        Step::ptr(new Step(ZERO_WALKVECTOR,
			   *gait,
			   dummyFoot));
    //need to indicate what the current support foot is:
    currentZMPDSteps.push_back(dummyStep);//right gets popped right away
    fillZMP(firstSupportStep);
    //addStartZMP(firstSupportStep);
    currentZMPDSteps.push_back(firstSupportStep);//left will be sup. during 0.0 zmp
    lastQueuedStep = firstSupportStep;
}


/**
 *  Creates a new step at the specified location (x,y,theta) specified in mm
 */
void StepGenerator::generateStep( float _x,
                                  float _y,
                                  float _theta) {
    //We have this problem that we can't simply start and stop the robot:
    //depending on the step type, we generate different types of ZMP
    //which means after any given step, only certain other steps types are
    //possible. For example, an END_STEP places the ZMP at 0,0, so it is a bad
    //idea to try to have a REGUALR_STEP follow it.  Also, the START_STEP
    //generates the same ZMP pattern as the REGULAR_STEP, but walking leg
    //wont lift up the leg.
    //PROBLEM:
    // In order to follow these guidlines, we introduce some hackish, error
    //prone code (see below) which must be repeated for all three 'directions'.

    //MUSINGS on better design:
    //1)We should probably have two different StepTypes
    //  one that determines what kind of ZMP we want, and another that
    //  determines if we should lift the foot as we approach the destination
    //  determined by that step.
    //2)We would ideally be able to call Generate step no matter what - i.e.
    //  also to create the starting steps, etc. This probably means we need to
    //  figure out how we are externally starting and stopping this module
    //  and also means we need to store class level state information (another
    //  FSA?)
    //3)All this is contingent on building in the idea that with motion vector=
    //  (0,0,0) we imply that the robot stops.  We could encode this as two
    //  different overall behaviors: check if we want to start, else if we
    //  want to start moving, else if are already moving.
    StepType type;

    if(gait->step[WP::WALKING] == WP::NON_WALKING_GAIT){
	type = END_STEP;
	_x = 0.0f;
	_y = 0.0f;
	_theta = 0.0f;

    } else if (_x ==0 && _y == 0 && _theta == 0){//stopping, or stopped
//         if(lastQueuedStep->x != 0 || lastQueuedStep->theta != 0 ||
//            (lastQueuedStep->y - (lastQueuedStep->foot == LEFT_FOOT ?
//                                  1:-1)*HIP_OFFSET_Y) != 0)
//             type = REGULAR_STEP;
//         else
	type = END_STEP;

    }else{
        //we are moving somewhere, and we must ensure that the last step
        //we enqued was not an END STEP
        if(lastQueuedStep->type == END_STEP){
            if (lastQueuedStep->zmpd){//too late to change it! make this a start
                type = REGULAR_STEP;
                _x = 0.0f;
                _y = 0.0f;
		_theta = 0.0f;
            }else{
                type = REGULAR_STEP;
                lastQueuedStep->type = REGULAR_STEP;
            }
        }else{
            //the last step was either start or reg, so we're fine
            type = REGULAR_STEP;
        }
    }



    //The input here is in velocities. We need to convert it to distances perstep
    //Also, we need to scale for the fact that we can only turn or strafe every other step

    const WalkVector new_walk = {_x,_y,_theta};

    Step::ptr step(new Step(new_walk,
			    *gait,
			    (nextStepIsLeft ?
			     LEFT_FOOT : RIGHT_FOOT),
			    lastQueuedStep->walkVector,
			    type));

#ifdef DEBUG_STEPGENERATOR
    cout << "Generated a new step: "<<*step<<endl;
#endif
    futureSteps.push_back(step);
    lastQueuedStep = step;
    //switch feet after each step is generated
    nextStepIsLeft = !nextStepIsLeft;
}

/**
 * Method to return the default stance of the robot (including arms)
 *
 */
vector<float>
StepGenerator::getDefaultStance(const Gait& wp){
    const ufvector3 lleg_goal =
        CoordFrame3D::vector3D(-wp.stance[WP::BODY_OFF_X],
                               wp.stance[WP::LEG_SEPARATION_Y]*0.5f,
                               -wp.stance[WP::BODY_HEIGHT]);
    const ufvector3 rleg_goal =
        CoordFrame3D::vector3D(-wp.stance[WP::BODY_OFF_X],
                               -wp.stance[WP::LEG_SEPARATION_Y]*0.5f,
                               -wp.stance[WP::BODY_HEIGHT]);

    const vector<float> lleg = WalkingLeg::getAnglesFromGoal(LLEG_CHAIN,
                                                             lleg_goal,
                                                             wp.stance);
    const vector<float> rleg = WalkingLeg::getAnglesFromGoal(RLEG_CHAIN,
                                                             rleg_goal,
                                                             wp.stance);

    const vector<float> larm(LARM_WALK_ANGLES,&LARM_WALK_ANGLES[ARM_JOINTS]);
    const vector<float> rarm(RARM_WALK_ANGLES,&RARM_WALK_ANGLES[ARM_JOINTS]);

    vector<float> allJoints;

    //now combine all the vectors together
    allJoints.insert(allJoints.end(),larm.begin(),larm.end());
    allJoints.insert(allJoints.end(),lleg.begin(),lleg.end());
    allJoints.insert(allJoints.end(),rleg.begin(),rleg.end());
    allJoints.insert(allJoints.end(),rarm.begin(),rarm.end());
    return allJoints;
}

/**
 * Method returns the transformation matrix that goes between the previous
 * foot ('f') coordinate frame and the next f coordinate frame rooted at 'step'
 */
const ufmatrix3 StepGenerator::get_fprime_f(const Step::ptr step){
    const float leg_sign = (step->foot == LEFT_FOOT ? 1.0f : -1.0f);

    const float x = step->x;
    const float y = step->y;
    const float theta = step->theta;

    ufmatrix3 trans_fprime_s =
        CoordFrame3D::translation3D(0,-leg_sign*HIP_OFFSET_Y);

    ufmatrix3 trans_s_f =
        prod(CoordFrame3D::rotation3D(CoordFrame3D::Z_AXIS,-theta),
             CoordFrame3D::translation3D(-x,-y));
    return prod(trans_s_f,trans_fprime_s);
}

/**
 * DIFFERENT Method, returns the transformation matrix that goes between the f
 * coordinate frame rooted at 'step' and the previous foot ('f') coordinate
 * frame rooted at the last step.  Really just the inverse of the matrix
 * returned by the 'get_fprime_f'
 */
const ufmatrix3 StepGenerator::get_f_fprime(const Step::ptr step){
    const float leg_sign = (step->foot == LEFT_FOOT ? 1.0f : -1.0f);

    const float x = step->x;
    const float y = step->y;
    const float theta = step->theta;

    ufmatrix3 trans_fprime_s =
        CoordFrame3D::translation3D(0,leg_sign*HIP_OFFSET_Y);

    ufmatrix3 trans_s_f =
        prod(CoordFrame3D::translation3D(x,y),
             CoordFrame3D::rotation3D(CoordFrame3D::Z_AXIS,theta));
    return prod(trans_fprime_s,trans_s_f);
}

/**
 * Translates points in the sprime frame into the s frame, where
 * the difference between sprime and s is based on 'step'
 */
const ufmatrix3 StepGenerator::get_sprime_s(const Step::ptr step){
    const float leg_sign = (step->foot == LEFT_FOOT ? 1.0f : -1.0f);

    const float x = step->x;
    const float y = step->y;
    const float theta = step->theta;

    const ufmatrix3 trans_f_s =
        CoordFrame3D::translation3D(0,leg_sign*HIP_OFFSET_Y);

    const ufmatrix3 trans_sprime_f =
        prod(CoordFrame3D::rotation3D(CoordFrame3D::Z_AXIS,-theta),
             CoordFrame3D::translation3D(-x,-y));
    return prod(trans_f_s,trans_sprime_f);
}

/**
 * Yet another DIFFERENT Method, returning the matrix that translates points
 * in the next s frame back to the previous one, based on the intervening
 * Step (s' being the last s frame).
 */
const ufmatrix3 StepGenerator::get_s_sprime(const Step::ptr step){
    const float leg_sign = (step->foot == LEFT_FOOT ? 1.0f : -1.0f);

    const float x = step->x;
    const float y = step->y;
    const float theta = step->theta;

    const ufmatrix3 trans_f_s =
        CoordFrame3D::translation3D(0,-leg_sign*HIP_OFFSET_Y);

    const ufmatrix3 trans_sprime_f =
        prod(CoordFrame3D::translation3D(x,y),
             CoordFrame3D::rotation3D(CoordFrame3D::Z_AXIS,theta));
    return prod(trans_sprime_f,trans_f_s);
}

/**
 * Reset (remove all elements) in both the step queues (even ones which
 * we have already committed to), as well as any zmp reference points
 */
void StepGenerator::resetQueues(){
    futureSteps.clear();
    currentZMPDSteps.clear();
    zmp_ref_x.clear();
    zmp_ref_y.clear();
}

/**
 * Returns the cumulative odometry changes since the last call
 *
 * The basic idea is to keep track of where the start position is located
 * in two c-type frames. The new c frame is the c frame of the robot at the time
 * this method is called. The old c frame was the c frame of the robot when this
 * method was last called (or alternately since instantiation).
 *
 * The odometry update is then calculated by looking at the difference
 * between the location of the global origin (origin_i) in each of those frames.
 * This allows us to see how to translate, then rotate, from the old c frame
 * to the new one.
 *
 * Note that since we reset the location of the controller when we restart walking
 * it is vital to call 'resetOdometry()' in order to make sure any movement
 * since the last call to getOdometryUpdate doesnt get lost
 */
vector<float> StepGenerator::getOdometryUpdate() const {
    const float rotation = -safe_asin(cc_Transform(1,0));
    const ufvector3 odo = prod(cc_Transform,CoordFrame3D::vector3D(0.0f,0.0f));
    const float odoArray[3] = {odo(0),odo(1),rotation};
    //printf("Odometry update is (%g,%g,%g)\n",odoArray[0],odoArray[1],odoArray[2]);

    // NOTE: commented out because it breaks const correctness
    // HACK HACK HACK
//    cc_Transform = CoordFrame3D::translation3D(0.0f,0.0f);
    return vector<float>(odoArray,&odoArray[3]);
}

/**
 * Method to reset our odometry counters when coordinate frames are switched
 * Ensures we don't loose odometry information if the walk is restarted.
 */
void StepGenerator::resetOdometry(const float initX, const float initY){
    cc_Transform = CoordFrame3D::translation3D(-initX,-initY);
}

/**
 * Called once per motion frame to update the odometry
 *
 * For odometry, we average the position of both legs to find a point that
 * is approximately in the middle of our convex hull. We do this so that
 * the odometry remains stable over time, instead of oscillating in the X/Y/T
 * as the robot takes steps. This method also applies the delta odometry to
 * build cc_Transform (doc'd elsewhere)
 */
void StepGenerator::updateOdometry() {
    vector<float> left = leftLeg.getOdoUpdate();
    vector<float> right = rightLeg.getOdoUpdate();

    vector<float> deltaOdo(3,0);

    /* odometry explodes in the X during the swinging phase, so we always ask
       the supporting leg.
       Explanation: the swinging leg moves from negative to positive in X in the
         F frame, and the difference values used for odometry explode when the leg
	 crosses X-axis zero.
     */
    // NOTE: x odometry is currently set up to be filtered, but we're using a 1-width
    // Boxcar filter so no filtering actually takes place.
    if (leftLeg.isSupporting())
	deltaOdo[0] = static_cast<float>(xOdoFilter.X(left[0]));
    else
	deltaOdo[0] = static_cast<float>(xOdoFilter.X(right[0]));

    /* average the motion delta from the legs (Y)
       this tracks a point approximately between our two legs, and reflects
       actual robot motion much better */
    deltaOdo[1] = (left[1] + right[1])*0.5f;

    // use the hacked delta theta calculated in swapSupportLegs
    deltaOdo[2] = dThetaPerMotionFrame;

#ifdef DEBUG_ODOMETRY_UPDATE
    static int fCount;
    cout << fCount++ << " "
	 << deltaOdo[0] << " "
	 << deltaOdo[1] << " "
	 << deltaOdo[2] << " "
	 << endl;
#endif
    const ufmatrix3 odoUpdate = prod(CoordFrame3D::translation3D(deltaOdo[0],
                                                                 deltaOdo[1]),
                                     CoordFrame3D::rotation3D(CoordFrame3D::Z_AXIS,
                                                              -deltaOdo[2]));
    const ufmatrix3 new_cc_Transform  = prod(cc_Transform,odoUpdate);
    cc_Transform = new_cc_Transform;

}

/**
 * Method to figure out when to start swinging with the left vs. right left
 */
const bool StepGenerator::decideStartLeft(const float lateralVelocity,
					  const float radialVelocity){
    //Currently, the logic is very simple: if the strafe direction
    //or the turn direction go left, then start that way
    //Strafing takes precedence over turning.
    if(lateralVelocity == 0.0f){
        return radialVelocity > 0.0f;
    }
    return lateralVelocity > 0.0f;
    //An alternate algorithm might compute a test value like
    // lateralVelocity + 0.5f*radialVelocity  and decide on that
}

/**
 * Method to return the arm joints during the walk
 */
WalkArmsTuple StepGenerator::tick_arms(){

    return WalkArmsTuple(leftArm.tick(supportStep_f),
                         rightArm.tick(supportStep_f));
}

/**
 * Clears any future steps which are far enough in the future that we
 * haven't committed to them yet
 */
void StepGenerator::clearFutureSteps(){
    //first, we need to scrap all future steps:
    futureSteps.clear();
    if(currentZMPDSteps.size() > 0){
        lastQueuedStep = currentZMPDSteps.back();
        //Then, we need to make sure that the next step we generate will be of the correct type
        //If the last ZMPd step is left, the next one shouldn't be
        nextStepIsLeft = (lastQueuedStep->foot != LEFT_FOOT);
    }
}

void StepGenerator::update_FtoI_transform(){
    static ublas::matrix<float> identity(ublas::identity_matrix<float>(3));
    ublas::matrix<float> test (if_Transform);
    fi_Transform = solve(test,identity);
}

void StepGenerator::debugLogging(){


#ifdef DEBUG_CONTROLLER_COM
    float pre_x = zmp_ref_x.front();
    float pre_y = zmp_ref_y.front();
    float zmp_x = controller_x->getZMP();
    float zmp_y = controller_y->getZMP();

    vector<float> bodyAngles = sensors->getBodyAngles();
    float lleg_angles[LEG_JOINTS],rleg_angles[LEG_JOINTS];
    int bi = HEAD_JOINTS+ARM_JOINTS;
    for(unsigned int i = 0; i < LEG_JOINTS; i++, bi++)
        lleg_angles[i] = bodyAngles[bi];
    for(unsigned int i = 0; i < LEG_JOINTS; i++, bi++)
        rleg_angles[i] = bodyAngles[bi];

    //pick the supporting leg to decide how to calc. actual com pos
    //Currently hacked pretty heavily, and only the Y actually works
    //need to apply some coord. translations for this to actually work
    //also, beware of the time delay between sent commands and the robot
    ufvector3 leg_dest_c =
        -Kinematics::forwardKinematics((leftLeg.isSupporting()?
					LLEG_CHAIN: RLEG_CHAIN),//LLEG_CHAIN,
				       (leftLeg.isSupporting()?
					lleg_angles: rleg_angles));
    leg_dest_c(2) = 1.0f;

    ufvector3 leg_dest_i = prod(fi_Transform,leg_dest_c);
    float real_com_x = leg_dest_i(0);
    float real_com_y = leg_dest_i(1);// + 2*HIP_OFFSET_Y;
    float joint_com_i_x = joints_com_i(0);
    float joint_com_i_y = joints_com_i(1) + COM_I_Y_OFFSET;

    Inertial inertial = sensors->getInertial();
    FSR leftFoot = sensors->getLeftFootFSR();
    FSR rightFoot = sensors->getRightFootFSR();

    static float ttime = 0;
    fprintf(com_log,"%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t"
            "%f\t%f\t%f\t%f\t%f\t%f\t%f\t"
            "%f\t%f\t%f\t%f\t%f\t%f\t%f\t%d\n",
            ttime,com_i(0),com_i(1),pre_x,pre_y,zmp_x,zmp_y,
            est_zmp_i(0),est_zmp_i(1),
            zmp_filter.get_zmp_x(),zmp_filter.get_zmp_y(),
            real_com_x,real_com_y,joint_com_i_x,joint_com_i_y,
            inertial.angleX, inertial.angleY,
            acc_filter.getX(),acc_filter.getY(),acc_filter.getZ(),
            // FSRs
            leftFoot.frontLeft,leftFoot.frontRight,leftFoot.rearLeft,leftFoot.rearRight,
            rightFoot.frontLeft,rightFoot.frontRight,rightFoot.rearLeft,rightFoot.rearRight,
            leftLeg.getSupportMode());
    ttime += MOTION_FRAME_LENGTH_S;
#endif


#ifdef DEBUG_SENSOR_ZMP
    const float preX = zmp_ref_x.front();
    const float preY = zmp_ref_y.front();

    const float comX = com_i(0);
    const float comY = com_i(1);

    const float comPX = controller_x->getZMP();
    const float comPY = controller_y->getZMP();

    Inertial acc = sensors->getUnfilteredInertial();
    const float accX = accInWorldFrame(0);
    const float accY = accInWorldFrame(1);
    const float accZ = accInWorldFrame(2);
    static float stime = 0;

    fprintf(zmp_log,"%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\n",
            stime,preX,preY,comX,comY,comPX,comPY,accX,accY,accZ,
	    zmp_filter.get_zmp_x(),zmp_filter.get_zmp_y(),
            acc.angleX,acc.angleY);
    stime+= MOTION_FRAME_LENGTH_S;
#endif
}
