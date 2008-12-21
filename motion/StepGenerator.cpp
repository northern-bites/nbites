#include "StepGenerator.h"
#include <iostream>

StepGenerator::StepGenerator(const WalkingParameters *params)
    : x(0.0f), y(0.0f), theta(0.0f),
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

    setWalkVector(0,0,0); // for testing purposes. The function doesn't even
    // honor the parameters passed to it yet
}
StepGenerator::~StepGenerator(){
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
    static float lastZMP_x = 0;
    static float lastZMP_y = 0;

    while (zmp_ref_y.size() <= PreviewController::NUM_PREVIEW_FRAMES) {
        if (futureSteps.size() < 1  || futureSteps.size() +
            currentZMPDSteps.size() < MIN_NUM_ENQUEUED_STEPS)
            generateStep(x, y, theta); // with the current walk vector
        else {
            boost::shared_ptr<Step> nextStep = futureSteps.front();

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

    com_x = controller_x->tick(zmp_ref.get<0>());
    com_y = controller_y->tick(zmp_ref.get<1>());
    //cout << "Com x: " << com_x << endl;
}

/** Central method for moving the walking legs. It handles important stuff like:
 *
 *  * Switching support feet
 *  * Handles poping from the ZMPDStep list when we switch support feet
 */


WalkLegsTuple StepGenerator::tick_legs(){
    static bool test = true;
    if  (test){
        test = false;
        for(list<boost::shared_ptr<Step> >::iterator itr = currentZMPDSteps.begin(); itr !=currentZMPDSteps.end();itr++ ){
            cout << "Step in currentZMPDSteps is type "<<(*itr)->foot<<endl;
        }
    }
    //Decide if this is the first frame into any double support phase
    //which is the critical point when we must swap coord frames, etc
    if(leftLeg.isSwitchingSupportMode() && leftLeg.stateIsDoubleSupport()){
        //pop from the front of the current steps

        int numCurrentSteps = static_cast<int>(currentZMPDSteps.size());
        int numFutureSteps  = static_cast<int>(futureSteps.size());
        if (numCurrentSteps  + numFutureSteps < MIN_NUM_ENQUEUED_STEPS)
            throw "Insufficient steps";

        //We may eventually be able to do withouth this switch
        switch(min(numCurrentSteps,3)){
        case 3:
            //there are three elements in the list, pop the obsolete one
            //and the first step is the support one now, the second the swing
            currentZMPDSteps.pop_front();
            swingingStep  = *(++currentZMPDSteps.begin());
            supportStep   =  *currentZMPDSteps.begin();
            break;
        case 2:
            //there are only two elements in the list of active steps
            //this means we need to look at one step in the future steps
            currentZMPDSteps.pop_front();
            swingingStep  =  *futureSteps.begin();
            supportStep   =  *currentZMPDSteps.begin();
            break;
        case 1:
            //there is only one elements in the list of active steps
            //this means we need to look at two step in the future steps list
            currentZMPDSteps.pop_front();
            //note the absence of break!
        case 0:
            //there is nothing to pop, so just look at future steps.
            swingingStep  =  *(++futureSteps.begin());
            supportStep   =  *futureSteps.begin();
            break;
        default:
            throw "Something odd is happening in a StepGen switch statement";

        }

        //update the translation matrix between i and f coord. frames
        ublas::matrix<float> stepTransform = get_fprime_f(supportStep);
        //cout <<"Step transform" << stepTransform <<endl;
        if_Transform = prod(stepTransform,if_Transform);
        cout <<"I to F transform: " <<if_Transform <<endl;
        cout << "support step says it is : " << supportStep->foot << endl;
        cout << "Left leg says it is : " << leftLeg.getSupportMode() << endl;
        //TODO: -> start work here. Need to verify how coord frames work!!
        //express the supporting foot and swinging foots locations in f coord.
	
	cout<< "Should be identity" << prod(get_fprime_f(supportStep),get_f_fprime(supportStep))<<endl;
    }

    //calculate the f to c translation matrix

    //translate the targets for support and swinging foot into c frame

    //update leftLeg, rightLeg with targets in c frame

    //Temporary conversion to c frame for controller target
    float dest_L_x = -com_x + walkParams->hipOffsetX; //targetX for this leg
    float dest_L_y = -com_y + HIP_OFFSET_Y;  //targetY

    float dest_R_x = -com_x + walkParams->hipOffsetX; //targetX for this leg
    float dest_R_y = -com_y - HIP_OFFSET_Y;  //targetY

    vector<float> left  = leftLeg.tick(dest_L_x,dest_L_y);
    vector<float> right = rightLeg.tick(dest_R_x,dest_R_y);

    return WalkLegsTuple(left,right);
}



//Step length always must be a multiple of the motion frame length
void StepGenerator::fillZMP(const boost::shared_ptr<Step> newStep ){
    //look at the last ZMPD Step and the newStep, and make ZMP values
    float stepTime = newStep->time;
    int numChops = walkParams->stepDurationFrames;//static_cast<int>(stepTime / walkParams->motion_frame_length_s);
    float start_x = lastZMPDStep->x + coordOffsetLastZMPDStep.x;
    float start_y = lastZMPDStep->y + coordOffsetLastZMPDStep.y;
    float end_x = newStep->x + coordOffsetLastZMPDStep.x;
    float end_y = newStep->y + coordOffsetLastZMPDStep.y;

    //std::cout << "start_x: " << start_x << "end_x: " << end_x << std::endl;

    for(int i = 0; i< numChops; i++){
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
    int sign = (newStep->foot == LEFT_FOOT ? 1 : -1);
    lastZMPDStep = newStep;
    coordOffsetLastZMPDStep.x += newStep->x;
    coordOffsetLastZMPDStep.y += newStep->y - sign*HIP_OFFSET_Y; //shift to 0
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

    //start off in a double support phase where the right leg swings first
    leftLeg.startRight();//setSupportMode(PERSISTENT_DOUBLE_SUPPORT);
    rightLeg.startRight();//setSupportMode(DOUBLE_SUPPORT);
    if_Transform.assign(initStartRight);

    //setup memory to corroborate dummy step
    lastZMPDStep = boost::shared_ptr<Step>(new Step(0,HIP_OFFSET_Y,0,
                                                    walkParams->stepDuration, LEFT_FOOT));
    boost::shared_ptr<Step> dummyStep =
        boost::shared_ptr<Step>(new Step(0,HIP_OFFSET_Y,0,
                                         walkParams->stepDuration, LEFT_FOOT));
    //need to indicate what the current support foot is:
    currentZMPDSteps.push_back(lastZMPDStep);
    currentZMPDSteps.push_back(dummyStep);
    coordOffsetLastZMPDStep = point<float>(0,0);
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

    futureSteps.push_back(step);
    cout<<"Made a step with type"<<step->foot<<" and y coord " <<step->y<<endl;
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
