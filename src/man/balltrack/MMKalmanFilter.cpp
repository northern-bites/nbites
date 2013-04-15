#include "MMKalmanFilter.h"

namespace man{
namespace balltrack{

/**
 * @ Brief- Constructor of my 'Puppetmaster'
 *          Grab the params, gen a bunch of filters to avoid nulls
 *          Set frames w/o ball high so we re-init based on initial observations
 *          Clearle consecutive observation is false if we havent seen the ball...
 *          State est and vis history can stay zero,
 *          Same with bestFilter, stationary, and lastUpdateTime
 */
MMKalmanFilter::MMKalmanFilter(MMKalmanFilterParams params_)
{


    params = params_;

    for(int i=0; i< params.numFilters; i++)
        filters.push_back(new KalmanFilter());

    framesWithoutBall = params.framesTillReset;
    consecutiveObservation = false;
    bestFilter = 0;
    obsvBuffer = new CartesianObservation[params.bufferSize];
    curEntry = 0;
    fullBuffer = false;
    initialize();
}

/**
 * @Brief - #nomememoryleaks
 *           (hopefully)
 */
MMKalmanFilter::~MMKalmanFilter()
{
    delete &filters;
}

/**
 * @Brief - Main interface, takes in an update with a visionBall and a motion message
 *          Should be called whenever new information, and simply dont pass it the same
 *               message twice!
 * @params - visionball is a NEW vision message
           - motion is a NEW motion message
 */
void MMKalmanFilter::update(messages::VisionBall    visionBall,
                            messages::RobotLocation odometry)
{
    // Predict the filters!
    // for(unsigned i=0; i<filters.size(); i++)
    //     filters.at(i)->printEst();
    predictFilters(odometry);
    // std::cout << "Predicted Filters" << std::endl;
    // for(unsigned i=0; i<filters.size(); i++)
    //     filters.at(i)->printEst();

    // Update with sensor if we have made an observation
    if(visionBall.on()) // We see the ball
    {
        // std::cout << "BALL SEEN ----------------------------------------" << std::endl;
        //Before we mess with anything, decide if we saw the ball twice in a row
        consecutiveObservation = (framesWithoutBall == 0) ? true : false;

        // Update our visual observation history
        lastVisRelX = visRelX;
        lastVisRelY = visRelY;
        //Calc relx and rely from vision
        float sinB,cosB;
        sincosf(visionBall.bearing(),&sinB,&cosB);
        visRelX = visionBall.distance()*cosB;
        visRelY = visionBall.distance()*sinB;

        // Update our observation buffer for re-initializing the moving filter
        curEntry = (curEntry+1)%params.bufferSize;
        obsvBuffer[curEntry] = CartesianObservation(visRelX, visRelY);

        if (!fullBuffer && curEntry==0)
        { // If buffer wasnt full but is now
            fullBuffer = true;
        }

        // If we havent seen the ball for a long time, re-init our filters
        if (framesWithoutBall >= params.framesTillReset)
        {
            // Reset the filters
            initialize(visRelX, visRelY, params.initCovX, params.initCovY);

            // Reset relevant variables
            framesWithoutBall = 0;
        }

        // #HACK for competition - If we get into a bad observation cycle then change it
        else if (filters.at((unsigned)1)->getSpeed() > 700.f && consecutiveObservation){
            initialize(visRelX, visRelY, params.initCovX, params.initCovY);
        }

        else if (fullBuffer) {
            // Calc velocity through these frames, if high reset moving filter
            CartesianObservation vel = calcVelocityOfBuffer();
            float speedThroughFrames = calcSpeed(vel.relX, vel.relY);

            // Calc diff between observation and estimata
            float estDiff = calcSpeed(visRelX - filters.at((unsigned)0)->getRelXPosEst(),
                                      visRelY - filters.at((unsigned)0)->getRelYPosEst());

            // Much higher than our current estimate
            // if ((speedThroughFrames > filters.at((unsigned)1)->getSpeed() + 60.f)
            //     && (estDiff > params.badStationaryThresh))
            //if(estDiff > params.badStationaryThresh)

            // If moving velocity <10, give this a try
            if (std::abs(filters.at((unsigned)1)->getSpeed()) < 10.f)
            {
                //std::cout << "\nBall Kicked!" << std::endl;
                ufvector4 newMovingX = filters.at((unsigned)0)->getStateEst();
                newMovingX(2) = vel.relX;
                newMovingX(3) = vel.relY;
                ufmatrix4 newMovingCov = boost::numeric::ublas::identity_matrix <float>(4);
                newMovingCov(0,0) = 10.f;
                newMovingCov(1,1) = 10.f;
                newMovingCov(2,2) = 20.f;
                newMovingCov(3,3) = 20.f;

                filters.at((unsigned)1)->initialize(newMovingX, newMovingCov);
            }
        }


        // Now correct our filters with the vision observation
        updateWithVision(visionBall);

        //Normalize all of the filter weights, currently ignore 'best' weight
        normalizeFilterWeights();

        updatePredictions();
    }

    else {
        consecutiveObservation = false;

        // don't use/wipeout buffer
        fullBuffer = false;
        curEntry = 0;
    }

    // #HACK - shouldnt know how many filters there are but... US OPEN!
        //Determine if we are using the stationary
    // std::cout << "Velocity Mag:\t" << filters.at((unsigned)1)->getSpeed() << std::endl;
    if (filters.at((unsigned)1)->getSpeed() > params.movingThresh)
    { // consider the ball to be moving
        bestFilter = 1;
    }
    else {
        bestFilter = 0;
    }

    // Now update our estimates before housekeeping
    prevStateEst = stateEst;
    prevCovEst   = covEst;

    stateEst = filters.at((unsigned)bestFilter)->getStateEst();
    covEst   = filters.at((unsigned)bestFilter)->getCovEst();

    /** commented out due to using only 2 filters **/
    // Kill the two worst estimates and re-init them if we made an observation
    // if(visionBall.on())
    //     cycleFilters();

    // Housekeep
    framesWithoutBall = (visionBall.on()) ? (0) : (framesWithoutBall+1);
    stationary = filters.at((unsigned)bestFilter)->isStationary();
    // std::cout << "Frames w/o Ball\t" << framesWithoutBall << std::endl;
    // std::cout << "Consecutive\t" << consecutiveObservation << std::endl;

}

/**
 * @brief - In charge of cycling through the filters, finding the worst stationary
 *           and replacing it with a new init filter. Also re-inits a new moving filter
 *            if we have had two consecutive observations and can calculate velocity
 */
void MMKalmanFilter::cycleFilters()
{
    //Find the two worst filters
    int worstStationary = -1;
    int worstMoving = -1;
    for(unsigned i=0; i<filters.size(); i++)
    {
        if (filters.at(i)->isStationary()){
            if(worstStationary<0)
                worstStationary = (int)i;
            else if (filters.at(i)->getWeight() < filters.at((unsigned) worstStationary)->getWeight())
                worstStationary = (int)i;
        }
        else
            if(worstMoving<0)
                worstMoving = (int)i;
            else if (filters.at(i)->getWeight() < filters.at((unsigned) worstMoving)->getWeight())
                worstMoving = (int)i;
    }

    // Re-init the worst stationary filter
    ufvector4 newX = boost::numeric::ublas::zero_vector<float>(4);
    newX(0) = visRelX;
    newX(1) = visRelY;
    ufmatrix4 newCov = boost::numeric::ublas::zero_matrix<float>(4);
    newCov(0,0) = .5f;//params.initCovX;
    newCov(1,1) = .5f;//params.initCovY;
    filters.at((unsigned) worstStationary)->initialize(newX, newCov);

    // Re-init the worst moving filter if we can calc a velocity
    if (consecutiveObservation){
        newX(2) = (visRelX - lastVisRelX) / deltaTime;
        newX(3) = (visRelY - lastVisRelY) / deltaTime;

        // HACK - magic number. need this in master asap though
        newCov(2,2) = 30.f;
        newCov(3,3) = 30.f;

        filters.at((unsigned) worstMoving)->initialize(newX, newCov);
    }
}

/*
 * @brief - Normalize the filter weights, pretty standard
 */
unsigned MMKalmanFilter::normalizeFilterWeights(){
    // Calc sum of the weights to normalize
    float totalWeights = 0;
    for(unsigned i=0; i<filters.size(); i++)
    {
        float curWeight = filters.at(i)->getWeight();
        // Don't want approx zero or =0 weights
        if (curWeight < .000001)
            filters.at(i)->setWeight(.000001f);
        totalWeights += (curWeight);
    }

    // Normalize and choose the best one
    unsigned tempBestFilter = 0;
    float bestWeight = filters.at(0)->getWeight();
    for(unsigned i=1; i<filters.size(); i++)
    {
        float curWeight = filters.at(i)->getWeight();
        if (curWeight > bestWeight)
        {
            tempBestFilter = i;
            bestWeight = curWeight;
        }
        filters.at(i)->setWeight(curWeight/totalWeights);
    }

    return tempBestFilter;
}

void MMKalmanFilter::printBothFilters() {
    std::cout << "-------Stationary Filter---------" << std::endl;
    filters.at((unsigned) 0)->printEst();
    std::cout << std::endl << "-------Moving Filter---------" << std::endl;
    filters.at((unsigned) 1)->printEst();
}

/**
 * @brief - Initialize all the filters!
 * @params- given a relX and relY for the position mean
 *          also a covX and covY since we may want to init
 *          w/ diff certainties throughout the life
 * @choice  I chose to have the velocities randomly initialized since there are
            soooo many combos
 */
void MMKalmanFilter::initialize(float relX, float relY, float covX, float covY)
{
    // std::cout << "Initialize the MMKalmanFilter" << std::endl;
    // std::cout << "RelX, RelY, CovX, CovY\t" << relX << "\t" << relY << "\t" << covX << "\t" << covY << std::endl;

    // clear the filters
    filters.clear();

    // make a random generator for initilizing different filters
    boost::mt19937 rng;
    rng.seed(std::time(0));
    boost::uniform_real<float> posCovRange(-2.f, 2.f);
    boost::variate_generator<boost::mt19937&,
                             boost::uniform_real<float> > positionGen(rng, posCovRange);
    boost::uniform_real<float> randVelRange(-5.f, 5.f);
    boost::variate_generator<boost::mt19937&,
                             boost::uniform_real<float> > velocityGen(rng, randVelRange);

    //make stationary
    for (int i=0; i<params.numFilters/2; i++)
    {
        // Needs to be stationary, have given mean, and add noise
        //   to the covariance matrix
        KalmanFilter *stationaryFilter = new KalmanFilter(true);
        ufvector4 x = boost::numeric::ublas::zero_vector<float>(4);
        x(0) = relX;
        x(1) = relY;
        x(2) = 0.f;
        x(3) = 0.f;

        ufmatrix4 cov = boost::numeric::ublas::zero_matrix<float>(4);
        cov(0,0) = covX + positionGen();
        cov(1,1) = covY + positionGen();

        // init and push it back
        stationaryFilter->initialize(x, cov);
        filters.push_back(stationaryFilter);
    }

    // make moving
    for (int i=0; i<params.numFilters/2; i++)
    {
        // Needs to be stationary, have given mean, and add noise
        //   to the covariance matrix
        KalmanFilter *movingFilter = new KalmanFilter(false);
        ufvector4 x = boost::numeric::ublas::zero_vector<float>(4);
        x(0)= relX;
        x(1)= relY;
        x(2) = 10.f;
        x(3) = 10.f;

        // Choose to assum obsv mean is perfect and just have noisy velocity
        ufmatrix4 cov = boost::numeric::ublas::zero_matrix<float>(4);
        cov(0,0) = covX;
        cov(1,1) = covY;
        cov(2,2) = 20.f;
        cov(3,3) = 20.f;

        movingFilter->initialize(x, cov);
        filters.push_back(movingFilter);
    }

}

// for offline testing, need to be able to specify the time which passed
void MMKalmanFilter::predictFilters(messages::RobotLocation odometry, float t)
{
    deltaTime = t;
    for (std::vector<KalmanFilter *>::iterator it = filters.begin(); it != filters.end(); it++)
    {
        (*it)->predict(odometry, deltaTime);
    }
}
/**
 * @brief - Predict each of the filters given info on where robot has moved
 *          Grab delta time from the system and then call the predict on each filter
 *
 */
void MMKalmanFilter::predictFilters(messages::RobotLocation odometry)
{
    // Update the time passed
    updateDeltaTime();

    // Update each filter
    for (std::vector<KalmanFilter *>::iterator it = filters.begin(); it != filters.end(); it++)
        (*it)->predict(odometry, deltaTime);
}

/**
 * @brief - Correct each filter given an observation
 *          Pretty straightforward...
 */
void MMKalmanFilter::updateWithVision(messages::VisionBall visionBall)
{
    for (std::vector<KalmanFilter *>::iterator it = filters.begin(); it != filters.end(); it++)
    {
        (*it)->updateWithObservation(visionBall);
    }
}

/**
 * @brief - update the filters predictions for where the ball will stop moving
 */
void MMKalmanFilter::updatePredictions()
{
    for (std::vector<KalmanFilter *>::iterator it = filters.begin(); it != filters.end(); it++)
        (*it)->predictBallDest();
}

/**
 * @brief - Update the delta time from the system. Delta time is in Seconds
 *
 */
void MMKalmanFilter::updateDeltaTime()
{
    // Get time since last update
    const long long int time = monotonic_micro_time(); //from common
    deltaTime = static_cast<float>(time - lastUpdateTime)/
        1000000.0f; // u_s to sec

    // Guard against a zero dt (maybe possible?)
    if (deltaTime == 0.0){
        deltaTime = 0.0001f;
    }
    lastUpdateTime = time;
}

CartesianObservation MMKalmanFilter::calcVelocityOfBuffer()
{
    CartesianObservation calcVel;

    float sumVelX = 0;
    float sumVelY = 0;
    for(int i=1; i<params.bufferSize; i++)
    {
        sumVelX += (obsvBuffer[i].relX - obsvBuffer[i-1].relX) / deltaTime;
        sumVelY += (obsvBuffer[i].relY - obsvBuffer[i-1].relY) / deltaTime;
    }

    calcVel.relX = sumVelX / (float)params.bufferSize;
    calcVel.relY = sumVelY / (float)params.bufferSize;

    // SANITY CHECKS
    // Major Concern with calculating from a large history is we don't want our
    // calculation to be watered down by observations when the ball was stationary
    // So check if there is a drastic inconsistency (off by 100 cm/s) in the speed
    bool consistent = true;
    float estSpeed = calcSpeed(calcVel.relX, calcVel.relY);
    for(int i=1; i<params.bufferSize; i++)
    {
        //current speed
        float curSpeed = calcSpeed((obsvBuffer[i].relX - obsvBuffer[i-1].relX)/deltaTime,
                                   (obsvBuffer[i].relY - obsvBuffer[i-1].relY)/deltaTime);

        if (diff(curSpeed,estSpeed) > 100)
            consistent = false;
    }

    if (consistent)
        return calcVel;
    else //wasnt consistent so return no velocity
        return CartesianObservation(0.f,0.f);
}

float MMKalmanFilter::diff(float a, float b)
{
    return std::abs(std::abs(a) - std::abs(b));
}

float MMKalmanFilter::calcSpeed(float a, float b)
{
    return std::sqrt(a*a + b*b);
}

}
}
