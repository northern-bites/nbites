#include "StepGenerator.h"
#include <iostream>

StepGenerator::StepGenerator(const WalkingParameters *params)
    : x(0.0f), y(0.0f), theta(0.0f),
      com_i(CoordFrame3D::vector3D(0.0f,0.0f)),
      zmp_ref_x(list<float>()),zmp_ref_y(list<float>()), futureSteps(),
      currentZMPDSteps(),
      lastZMPDStep(new Step(0,0,0,0,LEFT_FOOT)), coordOffsetLastZMPDStep(0,0),
      if_Transform(CoordFrame3D::translation3D(0.0f,0.0f)),
      initStartLeft(CoordFrame3D::translation3D(0.0f,HIP_OFFSET_Y)),
      initStartRight(CoordFrame3D::translation3D(0.0f,-HIP_OFFSET_Y)),
      walkParams(params), nextStepIsLeft(true),
      leftLeg(LLEG_CHAIN,params), rightLeg(RLEG_CHAIN,params),
      controller_x(new PreviewController()),
      controller_y(new PreviewController()){

    //COM logging
#ifdef DEBUG_CONTROLLER_COM
    com_log = fopen("/tmp/com_log.xls","w");
    fprintf(com_log,"time\tcom_x\tcom_y\tpre_x\tpre_y\n");
#endif

    setWalkVector(0,0,0); // for testing purposes. The function doesn't even
    // honor the parameters passed to it yet
}
StepGenerator::~StepGenerator(){
#ifdef DEBUG_CONTROLLER_COM
    fclose(com_log);
#endif
    delete controller_x; delete controller_y;

}

/**
 * Central method to get the previewed zmp_refernce values
 * In the process of getting these values, this method handles the following:
 *
 *  * Handles transfer from futureSteps list to the currentZMPDsteps list.
 *    When the Future ZMP values we want run out, we pop the next future step
 *    add generate ZMP from it, and put it into the ZMPDsteps List
 *
 */
zmp_xy_tuple StepGenerator::generate_zmp_ref() {
    static int fc = 4;
    static float lastZMP_x = 0;
    static float lastZMP_y = 0;

    while (zmp_ref_y.size() <= PreviewController::NUM_PREVIEW_FRAMES) {
        if (futureSteps.size() < 1  || futureSteps.size() +
            currentZMPDSteps.size() < MIN_NUM_ENQUEUED_STEPS){
            generateStep(x, y, theta); // with the current walk vector
            //cout << "Adding a step" <<endl;
/*
 * Start here: The issue is that we aren't doing a good job of keeping track of the correct target in walking leg, so actually moving forward is impossible right now
 */
            fc--;
            if (fc == 0){
                cout << "MOVE FORWARD!!"<<endl;
                //Change the x vector to be moving forward
                x =30;
            }
        }
        else {
            boost::shared_ptr<Step> nextStep = futureSteps.front();
            //cout << "Moving a step from future to current foot: "<<nextStep->foot<<endl;
            fillZMP(nextStep);

            //transfer the nextStep element from future to current list
            futureSteps.pop_front();
            currentZMPDSteps.push_back(nextStep);

        }
    }

    float newZMP_x = zmp_ref_x.front();
    float newZMP_y = zmp_ref_y.front();


    lastZMP_x = newZMP_x;
    lastZMP_y = newZMP_y;
    zmp_ref_x.pop_front();
    zmp_ref_y.pop_front();
    return zmp_xy_tuple(&zmp_ref_x, &zmp_ref_y);
}

void StepGenerator::tick_controller(){
    zmp_xy_tuple zmp_ref = generate_zmp_ref();
    //std::cout<< "zmp y: " << zmp_ref->front() << endl;
    //Tick the controller (input: ZMPref, sensors -- out: CoM x, y)

    const float com_x = controller_x->tick(zmp_ref.get<0>());
    const float com_y = controller_y->tick(zmp_ref.get<1>());
    com_i = CoordFrame3D::vector3D(com_x,com_y);


#ifdef DEBUG_CONTROLLER_COM
    float pre_x = zmp_ref.get<0>()->front();
    float pre_y = zmp_ref.get<1>()->front();

    static float ttime = 0;
    fprintf(com_log,"%f\t%f\t%f\t%f\t\%f\n",ttime,com_x,com_y,pre_x,pre_y);
    ttime += 0.05f;
#endif
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

        int numCurrentSteps = static_cast<int>(currentZMPDSteps.size());
        int numFutureSteps  = static_cast<int>(futureSteps.size());

        if (numCurrentSteps  + numFutureSteps < MIN_NUM_ENQUEUED_STEPS)
            throw "Insufficient steps";

        //there are three elements in the list, pop the obsolete one
        //and the first step is the support one now, the second the swing
        currentZMPDSteps.pop_front();
        swingingStep_s  = *(++currentZMPDSteps.begin());
        supportStep_s   =  *currentZMPDSteps.begin();

        //update the translation matrix between i and f coord. frames
        ublas::matrix<float> stepTransform = get_fprime_f(supportStep_s);
        if_Transform = prod(stepTransform,if_Transform);

        //Express the  destination  and source for the supporting foot and
        //swinging foots locations in f coord. Since the supporting foot doesn't
        //move, we ignore its source.

        //First, do the support foot, which is always at the origin
        const ublas::vector<float> origin = CoordFrame3D::vector3D(0,0);
        const ublas::vector<float> supp_pos_f = origin;

        //Second, do the source of the swinging leg, which can be calculated
        //using the stepTransform matrix from above
        ublas::vector<float> swing_src_f = prod(stepTransform,origin);

        //Third, do the dest. of the swinging leg, which is more complicated
        //We get the translation matrix that takes points in next f-type
        //coordinate frame, namely the one that will be centered at the swinging
        //foot's destination, and puts them into the current f coord. frame
        const ublas::matrix<float> swing_reverse_trans =
            get_f_fprime(swingingStep_s);
        //This gives us the position of the swinging foot's destination
        //in the current f frame
        const ublas::vector<float> swing_pos_f = prod(swing_reverse_trans,
                                                      origin);
        //finally, we need to know how much turning there will be. Turns out,
        //we can simply read this out of the aforementioned translation matr.
        //but, it will be twice the max. angle we send to HYP joint
        //this only works because its a 3D homog. coord matr - 4D would break
        float hyp_angle = acos(swing_reverse_trans(0,0))/2;

        //we use the swinging source to calc. a path for the swinging foot
        //it is not clear now if we will need to angle offset or what
        float last_hyp_angle = 0; //HACK - may need to actually do this

        //in the F coordinate frames, we express Steps representing
        // the three footholds from above
        supportStep_f =
            boost::shared_ptr<Step>(new Step(supp_pos_f(0),supp_pos_f(1),
                                             hyp_angle,
                                             supportStep_s->duration,
                                             supportStep_s->foot));
        swingingStep_f =
            boost::shared_ptr<Step>(new Step(swing_pos_f(0),swing_pos_f(1),
                                             hyp_angle,
                                             swingingStep_s->duration,
                                             swingingStep_s->foot));
        swingingStepSource_f  =
            boost::shared_ptr<Step>(new Step(swing_src_f(0),swing_src_f(1),
                                             last_hyp_angle,
                                             supportStep_s->duration,
                                             supportStep_s->foot));
    }

    //Each frame, we must recalculate the location of the center of mass
    //relative to the support leg (f coord frame), based on the output
    //of the controller (in tick_controller() )
    ublas::vector<float> com_f = prod(if_Transform,com_i);

    //Using the location of the com in the f coord frame, we can calculate
    //a transformation matrix to go from f to c
    fc_Transform = CoordFrame3D::translation3D(-com_f(0),-com_f(1));


    //Now we need to determine which leg to send the coorect footholds/Steps to
    boost::shared_ptr<Step> leftStep_f,rightStep_f;
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
                                       fc_Transform);
    vector<float> right = rightLeg.tick(rightStep_f,swingingStepSource_f,
                                        fc_Transform);

    return WalkLegsTuple(left,right);
}



//Step length always must be a multiple of the motion frame length
void StepGenerator::fillZMP(const boost::shared_ptr<Step> newSupportStep ){
    //look at the last ZMPD Step and the newStep, and make ZMP values
    float stepTime = newSupportStep->duration;
//  std::cout << "coordOffx: " << coordOffsetLastZMPDStep.x <<" lastZMPDStepx: "
//               << lastZMPDStep->x << " new supportStepx: " <<newSupportStep->x
//               << endl;
    const float start_x =  coordOffsetLastZMPDStep.x;
    const float start_y = coordOffsetLastZMPDStep.y;
    const float end_x = newSupportStep->x + coordOffsetLastZMPDStep.x;
    const float end_y = newSupportStep->y + coordOffsetLastZMPDStep.y;

//    std::cout << "start_x: " << start_x << "end_x: " << end_x << std::endl;

    //double support - we want to switch feet
    const int numChops = walkParams->doubleSupportFrames;
    for(int i = 0; i< walkParams->doubleSupportFrames; i++){
        float new_x =
            start_x + (static_cast<float>(i)/numChops)*(end_x-start_x);
        float new_y =
            start_y + (static_cast<float>(i)/numChops)*(end_y-start_y);

        zmp_ref_x.push_back(new_x);
        zmp_ref_y.push_back(new_y);
    }
    //single support -  we want to stay over the new step
    for(int i = 0; i< walkParams->singleSupportFrames; i++){
        float new_x =
            end_x;
        //start_x + (static_cast<float>(i)/numChops)*(end_x-start_x);
        float new_y =
            end_y;
        //start_y + (static_cast<float>(i)/numChops)*(end_y-start_y);

        zmp_ref_x.push_back(new_x);
        zmp_ref_y.push_back(new_y);
    }
    //update the lastZMPD Step
    int sign = (newSupportStep->foot == LEFT_FOOT ? 1 : -1);
    lastZMPDStep = newSupportStep;

    coordOffsetLastZMPDStep.x += newSupportStep->x;
    //shift to 0 ('s' coord frame):
    coordOffsetLastZMPDStep.y += newSupportStep->y - sign*HIP_OFFSET_Y;
}


void StepGenerator::setWalkVector(const float _x, const float _y,
                                  const float _theta)  {
    // WARNING: This method still assumes that we start with (0,0,0) as the
    // walk vector.
    x = _x;
    y = _y;
    theta = _theta;

    // We have to reevalaute future steps, so we forget about any future plans
    futureSteps.clear();

    //Queue a dummy step, where we step, but do nothing with the ZMP

    //we will take a dummy empty step, so push tons of zero ZMP values
    for (int i = 0; i < walkParams->stepDurationFrames; i++){
        zmp_ref_y.push_back(0.0f);
        zmp_ref_x.push_back(0.0f);
    }
    //cout << "Initial ZMPd Steps: " << zmp_ref_x.size()<<endl;
    coordOffsetLastZMPDStep = point<float>(0,0); //similar to s coord. frame

    //start off in a double support phase where the right leg swings first
    leftLeg.startRight();//setSupportMode(PERSISTENT_DOUBLE_SUPPORT);
    rightLeg.startRight();//setSupportMode(DOUBLE_SUPPORT);
    if_Transform.assign(initStartLeft);//HACK to deal with dummy being RIGHT

    //setup memory to corroborate dummy step
    lastZMPDStep = boost::shared_ptr<Step>(new Step(0,HIP_OFFSET_Y,0,
                                                    walkParams->stepDuration,
                                                    LEFT_FOOT));
    boost::shared_ptr<Step> dummyStep =
        boost::shared_ptr<Step>(new Step(0,-HIP_OFFSET_Y,0,
                                         walkParams->stepDuration, RIGHT_FOOT));
    //need to indicate what the current support foot is:
    currentZMPDSteps.push_back(dummyStep);//right gets popped right away
    currentZMPDSteps.push_back(lastZMPDStep);//left will be sup. during 0.0 zmp


    nextStepIsLeft = false;
}


void StepGenerator::startLeft(){

}
void StepGenerator::startRight(){
//The next leg to take a step is the right - the left is supporting
//This means, on the first frame, we will pop a step which should be the
//the current position of the right leg. Then, the left leg will
//become the support leg, and a step not shown here will become the next target.
//except the ZMP is being held at zero, so that step will do nothing

    boost::shared_ptr<Step> cur_right_leg =
        boost::shared_ptr<Step>(new Step(0,-HIP_OFFSET_Y,0,
                                         walkParams->stepDuration, RIGHT_FOOT));

    boost::shared_ptr<Step> sup_left_leg =
        boost::shared_ptr<Step>(new Step(0,HIP_OFFSET_Y,0,
                                         walkParams->stepDuration, LEFT_FOOT));

    leftLeg.startRight();
    rightLeg.startRight();

    coordOffsetLastZMPDStep = point<float>(0,0);
    //once we actually want to walk, we need to know which leg to use
    nextStepIsLeft = false;

}

//currently only does two sets of steps side by side
void StepGenerator::generateStep(const float _x,
                                 const float _y,
                                 const float _theta) {
    boost::shared_ptr<Step> step(new Step(_x,(nextStepIsLeft ?
                                              HIP_OFFSET_Y : -HIP_OFFSET_Y),
                                          0, walkParams->stepDuration,
                                          (nextStepIsLeft ?
                                           LEFT_FOOT : RIGHT_FOOT)));
    cout << "NEW STEP with x="<<_x<<endl;

    futureSteps.push_back(step);
    //switch feet after each step is generated
    nextStepIsLeft = !nextStepIsLeft;
}

/**
 * Method returns the transformation matrix that goes between the previous
 * foot ('f') coordinate frame and the next f coordinate frame rooted at 'step'
 */
ublas::matrix<float>
StepGenerator::get_fprime_f(boost::shared_ptr<Step> step){
    const int leg_sign = (step->foot == LEFT_FOOT ? 1 : -1);

    const float x = step->x;
    const float y = step->y;
    const float theta = step->theta;

    ublas::matrix<float> trans_fprime_s =
        CoordFrame3D::translation3D(0,-leg_sign*HIP_OFFSET_Y);

    ublas::matrix<float> trans_s_f =
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
ublas::matrix<float>
StepGenerator::get_f_fprime(boost::shared_ptr<Step> step){
    const int leg_sign = (step->foot == LEFT_FOOT ? 1 : -1);

    const float x = step->x;
    const float y = step->y;
    const float theta = step->theta;

    ublas::matrix<float> trans_fprime_s =
        CoordFrame3D::translation3D(0,leg_sign*HIP_OFFSET_Y);

    ublas::matrix<float> trans_s_f =
        prod(CoordFrame3D::translation3D(x,y),CoordFrame3D::rotation3D(CoordFrame3D::Z_AXIS,theta));
    return prod(trans_s_f,trans_fprime_s);
}
