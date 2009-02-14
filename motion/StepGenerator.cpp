
#include <iostream>
#include <boost/shared_ptr.hpp>
using boost::shared_ptr;

#include "StepGenerator.h"

StepGenerator::StepGenerator(shared_ptr<Sensors> s ,
                             const WalkingParameters *params)
  : x(0.0f), y(0.0f), theta(0.0f),
    _done(true),com_i(CoordFrame3D::vector3D(0.0f,0.0f)),
    est_zmp_i(CoordFrame3D::vector3D(0.0f,0.0f)),
    zmp_ref_x(list<float>()),zmp_ref_y(list<float>()), futureSteps(),
    currentZMPDSteps(),
    si_Transform(CoordFrame3D::identity3D()),
    last_zmp_end_s(CoordFrame3D::vector3D(0.0f,0.0f)),
    if_Transform(CoordFrame3D::identity3D()),
    initStartLeft(CoordFrame3D::translation3D(0.0f,HIP_OFFSET_Y)),
    initStartRight(CoordFrame3D::translation3D(0.0f,-HIP_OFFSET_Y)),
    sensors(s),
    walkParams(params), nextStepIsLeft(true),
    leftLeg(LLEG_CHAIN,params), rightLeg(RLEG_CHAIN,params),
    controller_x(new PreviewController()),
    controller_y(new PreviewController()){

    //COM logging
#ifdef DEBUG_CONTROLLER_COM
    com_log = fopen("/tmp/com_log.xls","w");
    fprintf(com_log,"time\tcom_x\tcom_y\tpre_x\tpre_y\tzmp_x\tzmp_y\tsensor_zmp_x\tsensor_zmp_y\treal_com_x\treal_com_y\tstate\n");
#endif
    controller_x->initState(walkParams->hipOffsetX,0.1f,walkParams->hipOffsetX);

    //hack
    //setSpeed(0.0f,0.0f,0.0f);
}
StepGenerator::~StepGenerator(){
#ifdef DEBUG_CONTROLLER_COM
    fclose(com_log);
#endif
    delete controller_x; delete controller_y;

}

/**
 * Central method to get the previewed zmp_refernce values
 * In the process of getting these values, this method handles the following:    80
 *
 *  * Handles transfer from futureSteps list to the currentZMPDsteps list.
 *    When the Future ZMP values we want run out, we pop the next future step
 *    add generate ZMP from it, and put it into the ZMPDsteps List
 *
 */
zmp_xy_tuple StepGenerator::generate_zmp_ref() {
    //Generate enough ZMPs so a) the controller can run
    //and                     b) there are enough steps
    while (zmp_ref_y.size() <= PreviewController::NUM_PREVIEW_FRAMES ||
           futureSteps.size() + currentZMPDSteps.size() < MIN_NUM_ENQUEUED_STEPS) {
        if (futureSteps.size() < 1  || futureSteps.size() +
            currentZMPDSteps.size() < MIN_NUM_ENQUEUED_STEPS){
            generateStep(x, y, theta); // with the current walk vector
        }
        else {
            shared_ptr<Step> nextStep = futureSteps.front();
            fillZMP(nextStep);

            //transfer the nextStep element from future to current list
            futureSteps.pop_front();
            currentZMPDSteps.push_back(nextStep);

        }
    }
    zmp_ref_x.pop_front();
    zmp_ref_y.pop_front();
    return zmp_xy_tuple(&zmp_ref_x, &zmp_ref_y);
}

void StepGenerator::tick_controller(){
    #define G 9.81
    Inertial inertial = sensors->getInertial();

    ufvector3 accel_c = CoordFrame3D::vector3D(inertial.accX,inertial.accY);
    float angle_fc = asin(fc_Transform(1,0));
    float angle_if = asin(if_Transform(1,0));
    float tot_angle = -(angle_fc+angle_if);
    ufvector3 accel_i = prod(CoordFrame3D::rotation3D(CoordFrame3D::Z_AXIS,
                                                      tot_angle),
                            accel_c);

    est_zmp_i(0) = com_i(0) - (walkParams->bodyHeight/G)*accel_i(0);
    est_zmp_i(1) = com_i(1) - (walkParams->bodyHeight/G)*accel_i(1);

    //cout << "zmp: "<< est_zmp_i(0) << ", " <<est_zmp_i(1)<<endl;

    zmp_xy_tuple zmp_ref = generate_zmp_ref();
    //Tick the controller (input: ZMPref, sensors -- out: CoM x, y)

    const float com_x = controller_x->tick(zmp_ref.get<0>());
    const float com_y = controller_y->tick(zmp_ref.get<1>());
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
    //Decide if this is the first frame into any double support phase
    //which is the critical point when we must swap coord frames, etc
    if(leftLeg.isSwitchingSupportMode() && leftLeg.stateIsDoubleSupport()){
        swapSupportLegs();
    }

    //hack-ish for now to do hyp pitch crap
    leftLeg.setSteps(swingingStepSource_f, swingingStep_f,supportStep_f);
    rightLeg.setSteps(swingingStepSource_f, swingingStep_f,supportStep_f);

    //Each frame, we must recalculate the location of the center of mass
    //relative to the support leg (f coord frame), based on the output
    //of the controller (in tick_controller() )
    ufvector3 com_f = prod(if_Transform,com_i);

    //We want to get the incremental rotation of the center of mass
    //we need to ask one of the walking legs to give it:
    const float body_rot_angle_fc = leftLeg.getFootRotation()/2; //relative to f

    //Using the location of the com in the f coord frame, we can calculate
    //a transformation matrix to go from f to c
    fc_Transform = prod(CoordFrame3D::rotation3D(CoordFrame3D::Z_AXIS,
                                                 body_rot_angle_fc),
                        CoordFrame3D::translation3D(-com_f(0),-com_f(1)));

    //Now we need to determine which leg to send the coorect footholds/Steps to
    shared_ptr<Step> leftStep_f,rightStep_f;
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
    vector<float> left  = leftLeg.tick(leftStep_f,swingingStepSource_f,
                                       swingingStep_f,fc_Transform);
    vector<float> right = rightLeg.tick(rightStep_f,swingingStepSource_f,
                                        swingingStep_f,fc_Transform);

    //HACK check to see if we are done
    if(supportStep_s->type == END_STEP && swingingStep_s->type == END_STEP
       && lastStep_s->type == END_STEP){
        cout << "step generator done = true" << endl;
        _done = true;
    }

    debugLogging();

    return WalkLegsTuple(left,right);
}


void StepGenerator::swapSupportLegs(){

        if (currentZMPDSteps.size() +  futureSteps.size() <
            MIN_NUM_ENQUEUED_STEPS)
            throw "Insufficient steps";

        //there are three elements in the list, pop the obsolete one
        //(currently use last step to determine when to stop, hackish-ish)
        //and the first step is the support one now, the second the swing
        lastStep_s = *currentZMPDSteps.begin();
        currentZMPDSteps.pop_front();
        swingingStep_s  = *(++currentZMPDSteps.begin());
        supportStep_s   =  *currentZMPDSteps.begin();

        //update the translation matrix between i and f coord. frames
        ufmatrix3 stepTransform = get_fprime_f(supportStep_s);
        if_Transform = prod(stepTransform,if_Transform);

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
        float swing_dest_angle = -asin(swing_reverse_trans(1,0));

        //we use the swinging source to calc. a path for the swinging foot
        //it is not clear now if we will need to angle offset or what
        float swing_src_angle = -asin(stepTransform(1,0));

        //in the F coordinate frames, we express Steps representing
        // the three footholds from above
        supportStep_f =
            shared_ptr<Step>(new Step(supp_pos_f(0),supp_pos_f(1),
                                      0.0f,supportStep_s));
        swingingStep_f =
            shared_ptr<Step>(new Step(swing_pos_f(0),swing_pos_f(1),
                                      swing_dest_angle,swingingStep_s));
        swingingStepSource_f  =
            shared_ptr<Step>(new Step(swing_src_f(0),swing_src_f(1),
                                      swing_src_angle,lastStep_s));

}

void StepGenerator::fillZMP(const shared_ptr<Step> newSupportStep ){

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

void
StepGenerator::fillZMPRegular(const shared_ptr<Step> newSupportStep ){
    //look at the newStep, and make ZMP values:
    const float stepTime = newSupportStep->duration;
    //update the lastZMPD Step
    const int sign = (newSupportStep->foot == LEFT_FOOT ? 1 : -1);
    const int last_sign = -sign;

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
    float X_ZMP_FOOT_LENGTH = walkParams->footLengthX;

    //Another HACK (ie. zmp is not perfect)
    //This moves the zmp reference to the outside of the foot
    float Y_ZMP_OFFSET = (newSupportStep->foot == LEFT_FOOT ?
                          walkParams->leftZMPSwingOffsetY :
                          walkParams->rightZMPSwingOffsetY);

    //lets define the key points in the s frame. See diagram in paper
    //to use bezier curves, we would need also directions for each point
    const ufvector3 start_s = last_zmp_end_s;
    const ufvector3 end_s =
        CoordFrame3D::vector3D(newSupportStep->x + walkParams->hipOffsetX ,//+X_ZMP_FOOT_LENGTH,
                               newSupportStep->y + sign*Y_ZMP_OFFSET);
    const ufvector3 mid_s =
        CoordFrame3D::vector3D(newSupportStep->x + walkParams->hipOffsetX - X_ZMP_FOOT_LENGTH,
                               newSupportStep->y + sign*Y_ZMP_OFFSET);

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
    //the constant walkParams->dblSupInactivePercentage

    //First, split up the frames:
    const int halfNumDSChops = //DS - DoubleStaticChops
        int(walkParams->doubleSupportFrames*
            walkParams->dblSupInactivePercentage/2.0f);
    const int numDMChops = //DM - DoubleMovingChops
        walkParams->doubleSupportFrames - halfNumDSChops*2;

    //Phase 1) - stay at start_i
    for(int i = 0; i< halfNumDSChops; i++){
        zmp_ref_x.push_back(start_i(0));
        zmp_ref_y.push_back(start_i(1));
    }

    //phase 2) - move from start_i to
    for(int i = 0; i< numDMChops; i++){
        ufvector3 new_i = start_i +
            (static_cast<float>(i)/numDMChops)*(mid_i-start_i);

        zmp_ref_x.push_back(new_i(0));
        zmp_ref_y.push_back(new_i(1));
    }

    //phase 3) - stay at mid_i
    for(int i = 0; i< halfNumDSChops; i++){
        zmp_ref_x.push_back(mid_i(0));
        zmp_ref_y.push_back(mid_i(1));
    }


    //single support -  we want to stay over the new step
    const int numSChops = walkParams->singleSupportFrames;
    for(int i = 0; i< walkParams->singleSupportFrames; i++){
//    const int numSChops = walkParams->stepDurationFrames;
//    for(int i = 0; i< walkParams->stepDurationFrames; i++){

        ufvector3 new_i = mid_i +
            (static_cast<float>(i)/numSChops)*(end_i-mid_i);

        zmp_ref_x.push_back(new_i(0));
        zmp_ref_y.push_back(new_i(1));
    }

    //update our reference frame for the next time this method is called
    si_Transform = prod(si_Transform,get_s_sprime(newSupportStep));
    //store the end of the zmp in the next s frame:
    last_zmp_end_s = prod(get_sprime_s(newSupportStep),end_s);
}

void
StepGenerator::fillZMPEnd(const shared_ptr<Step> newSupportStep ){
    const ufvector3 end_s =
        CoordFrame3D::vector3D(walkParams->hipOffsetX,
                               0.0f);
    const ufvector3 end_i = prod(si_Transform,end_s);
    //Queue a starting step, where we step, but do nothing with the ZMP
    //so push tons of zero ZMP values
    for (int i = 0; i < walkParams->stepDurationFrames; i++){
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
    cout << "Calling setSpeed with:" << endl
         << "    x: " << _x << " y: " << _y << " theta: " << _theta << endl;
    //convert speeds in cm/s and rad/s into steps:
    const float new_x = _x*walkParams->stepDuration;
    const float new_y = _y*walkParams->stepDuration;
    //we only turn every other step, so double the turning!
    const float new_theta = _theta*walkParams->stepDuration*2.0;

    //If the walk vector isn't changing, 
    if(abs(new_x - x) <= NEW_VECTOR_THRESH_MMS &&
       abs(new_y - y) <= NEW_VECTOR_THRESH_MMS &&
       abs(new_theta - theta) <= NEW_VECTOR_THRESH_RADS){

        //and there are plenty of steps,
        if(futureSteps.size() + currentZMPDSteps.size() >= MIN_NUM_ENQUEUED_STEPS){
            cout << "Walk vector not different enough, not updating anything" <<endl;
            return;
        }
    }

    //if the new one is different, update the 
    x = new_x; y = new_y; theta = new_theta;

    // We have to reevalaute future steps, so we forget about any future plans
    futureSteps.clear();

    //TODO we also need to forget about any zmp values from before.
    //WE should do this on ending, not on starting - what if we switch gaits?
    //zmp_ref_x.clear();
    //zmp_ref_y.clear();

    if(y > 0 || theta > 0)
        startLeft();
    else
        startRight();

    _done = false;

}

/*  Set up the walking engine for starting with a swinging step on the right */
void StepGenerator::startRight(){
    //start off in a double support phase where the right leg swings first
    //HOWEVER, since the first support step is END, there will be no
    //actual swinging - the first actual swing will be 2 steps
    //after the first support step, in this case, causing right to swing first
    leftLeg.startLeft();
    rightLeg.startLeft();

    //Setup transform, such that the firstSupportStep is Right
    if_Transform.assign(initStartRight);

    //Support step is END Type, but the first swing step, generated
    //in generateStep, is START type.
    shared_ptr<Step> firstSupportStep =
        shared_ptr<Step>(new Step(0,-HIP_OFFSET_Y,0,
                                  walkParams->stepDuration,
                                  RIGHT_FOOT,END_STEP));
    shared_ptr<Step> dummyStep =
        shared_ptr<Step>(new Step(0,HIP_OFFSET_Y,0,
                                  walkParams->stepDuration, LEFT_FOOT));
    //need to indicate what the current support foot is:
    currentZMPDSteps.push_back(dummyStep);//right gets popped right away
    fillZMP(firstSupportStep);
    currentZMPDSteps.push_back(firstSupportStep);//left will be sup. during 0.0 zmp
    lastQueuedStep = firstSupportStep;
    nextStepIsLeft = true;
}

/*  Set up the walking engine for starting with a swinging step on the left */
void StepGenerator::startLeft(){
    //start off in a double support phase where the right leg swings first
    //HOWEVER, since the first support step is END, there will be no
    //actual swinging - the first actual swing will be 2 steps
    //after the first support step, in this case, causing left to swing first
    leftLeg.startRight();
    rightLeg.startRight();

    //Setup transform, such that the firstSupportStep is Left
    if_Transform.assign(initStartLeft);

    //Support step is END Type, but the first swing step, generated
    //in generateStep, is START type.
    shared_ptr<Step> firstSupportStep =
        shared_ptr<Step>(new Step(0,HIP_OFFSET_Y,0,
                                  walkParams->stepDuration,
                                  LEFT_FOOT,END_STEP));
    shared_ptr<Step> dummyStep =
        shared_ptr<Step>(new Step(0,-HIP_OFFSET_Y,0,
                                  walkParams->stepDuration, RIGHT_FOOT));
    //need to indicate what the current support foot is:
    currentZMPDSteps.push_back(dummyStep);//right gets popped right away
    fillZMP(firstSupportStep);
    currentZMPDSteps.push_back(firstSupportStep);//left will be sup. during 0.0 zmp
    lastQueuedStep = firstSupportStep;
    nextStepIsLeft = false;
}

//currently only does two sets of steps side by side
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
    if (_x ==0 && _y == 0 && _theta == 0){//stopping, or stopped
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


    //check  if we need to clip lateral movement of this leg
    if(_y > 0){
        if(!nextStepIsLeft){
            _y = 0.0f;
        }
    }else if(_y < 0){
        if(nextStepIsLeft){
            _y = 0.0f;
        }
    }

    if(_theta > 0){
        if(!nextStepIsLeft){
            _theta = 0.0f;
        }
    }else if (_theta < 0){
        if(nextStepIsLeft){
            _theta = 0.0f;
        }
    }

    shared_ptr<Step> step(new Step(_x,(nextStepIsLeft ?
                                       HIP_OFFSET_Y : -HIP_OFFSET_Y) + _y,
                                   _theta, walkParams->stepDuration,
                                   (nextStepIsLeft ?
                                    LEFT_FOOT : RIGHT_FOOT),
                                   type));

    futureSteps.push_back(step);
    lastQueuedStep = step;
    //switch feet after each step is generated
    nextStepIsLeft = !nextStepIsLeft;
}

/**
 * Method returns the transformation matrix that goes between the previous
 * foot ('f') coordinate frame and the next f coordinate frame rooted at 'step'
 */
const ufmatrix3 StepGenerator::get_fprime_f(const shared_ptr<Step> step){
    const int leg_sign = (step->foot == LEFT_FOOT ? 1 : -1);

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
const ufmatrix3 StepGenerator::get_f_fprime(const shared_ptr<Step> step){
    const int leg_sign = (step->foot == LEFT_FOOT ? 1 : -1);

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
const ufmatrix3 StepGenerator::get_sprime_s(const shared_ptr<Step> step){
    const int leg_sign = (step->foot == LEFT_FOOT ? 1 : -1);

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
const ufmatrix3 StepGenerator::get_s_sprime(const shared_ptr<Step> step){
    const int leg_sign = (step->foot == LEFT_FOOT ? 1 : -1);

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


void StepGenerator::debugLogging(){


#ifdef DEBUG_CONTROLLER_COM
    float pre_x = zmp_ref_x.front();
    float pre_y = zmp_ref_y.front();
    float zmp_x = controller_x->getZMP();
    float zmp_y = controller_y->getZMP();

    vector<float> bodyAngles = sensors->getBodyAngles();
    float lleg_angles[LEG_JOINTS],rleg_angles[LEG_JOINTS];
    int bi = HEAD_JOINTS+ARM_JOINTS;
    for(uint i = 0; i < LEG_JOINTS; i++, bi++)
        lleg_angles[i] = bodyAngles[bi];
    for(uint i = 0; i < LEG_JOINTS; i++, bi++)
        rleg_angles[i] = bodyAngles[bi];

    //pick the supporting leg to decide how to calc. actual com pos
    //Currently hacked pretty heavily, and only the Y actually works
    //need to apply some coord. translations for this to actually work
    //also, beware of the time delay between sent commands and the robot
    ufvector3 leg_dest = Kinematics::forwardKinematics(LLEG_CHAIN,
                                                       lleg_angles);
    (leftLeg.stateIsDoubleSupport()?
     LLEG_CHAIN: RLEG_CHAIN);
    float real_com_x = leg_dest(0);
    float real_com_y = leg_dest(1);// + 2*HIP_OFFSET_Y;
    //printf("real com (x,y) : (%f,%f) \n", real_com_x,real_com_y);
    static float ttime = 0;
    fprintf(com_log,"%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%d\n",
            ttime,com_i(0),com_i(1),pre_x,pre_y,zmp_x,zmp_y,
            est_zmp_i(0),est_zmp_i(1),
            real_com_x,real_com_y,leftLeg.getSupportMode());
    ttime += 0.02f;
#endif

}
