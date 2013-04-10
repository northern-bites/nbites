#include "MMKalmanFilter.h"

namespace man{
namespace balltrack{

MMKalmanFilter::MMKalmanFilter(MMKalmanFilterParams params_)
{
    params = params_;

    for(int i=0; i< params.numFilters; i++)
        filters.push_back(new KalmanFilter());

    framesWithoutBall = params.framesTillReset;
}

MMKalmanFilter::~MMKalmanFilter()
{
    delete &filters;
}

void MMKalmanFilter::update(messages::VisionBall visionBall,
                            messages::Motion     motion)
{
    // Predict the filters!

    // HACK HACK HACK for offline
    predictFilters(motion.odometry(),1.f);

    for(unsigned i=0; i<filters.size(); i++)
    {
        std::cout << "Filter " << i << "x est:\t"
                  <<filters.at(i)->getRelXPosEst() << "\n"
                  <<"y est:\t" << filters.at(i)->getRelYPosEst()
                  <<std::endl;
    }


    // Update with sensor if we have made an observation
    if(visionBall.on()) // We see the ball
    {
        //Before we mess with anything, decide if we aw the ball twice in a row
        consecutiveObservation = (framesWithoutBall == 0) ? true : false;

        lastVisRelX = visRelX;
        lastVisRelY = visRelY;

        //Calc relx and rely from vision
        float sinB,cosB;
        sincosf(visionBall.bearing(),&sinB,&cosB);
        visRelX = visionBall.distance()*cosB;
        visRelY = visionBall.distance()*sinB;

        // We haven't seen a ball for a long time
        if (framesWithoutBall >= params.framesTillReset)
        {
            std::cout << "Haven't seen ball in a long time" << std::endl;

            // Reset the filters
            initialize(visRelX, visRelY, params.initCovX, params.initCovY);

            // Reset relevant variables
            framesWithoutBall = 0;
        }

        updateWithVision(visionBall);

        std::cout << "UPDATE WITH VISION" << std::endl;
        for(unsigned i=0; i<filters.size(); i++)
        {
            std::cout << "Filter " << i << "x est:\t"
                      <<filters.at(i)->getRelXPosEst() << "\n"
                      <<"y est:\t" << filters.at(i)->getRelYPosEst()
                      <<std::endl;
        }
        //Normalize all of the filter weights, find best one
        bestFilter = normalizeFilterWeights();

        std::cout << "Best Filter\t" << bestFilter << std::endl;
    }

    else
    {
        consecutiveObservation = false;
        framesWithoutBall ++;
    }

    // Now update our estimates before housekeeping
    prevStateEst = stateEst;
    prevCovEst   = covEst;
    stateEst = filters.at(bestFilter)->getStateEst();
    covEst   = filters.at(bestFilter)->getCovEst();

    // Kill the two worst estimates and re-init them if we made an observation
    if(visionBall.on())
        cycleFilters();

    // Housekeep
    framesWithoutBall = (visionBall.on()) ? (0) : (framesWithoutBall+1);
    stationary = filters.at(bestFilter)->isStationary();


}

void MMKalmanFilter::cycleFilters()
{
    std::cout << "---------------------" << "CYCLE FILTERS"<<std::endl;
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
    newCov(0,0) = params.initCovX;
    newCov(1,1) = params.initCovY;

    filters.at((unsigned) worstStationary)->initialize(newX, newCov);

    // Re-init the worst moving filter if we can calc a velocity
    if (consecutiveObservation){
        std::cout << "Re-init moving filter\t"<<worstMoving<<std::endl;
        newX(2) = (visRelX - lastVisRelX) / deltaTime;
        newX(3) = (visRelY - lastVisRelY) / deltaTime;
        newCov(2,2) = 30.f;
        newCov(3,3) = 30.f;

        filters.at((unsigned) worstMoving)->initialize(newX, newCov);
    }


}

unsigned MMKalmanFilter::normalizeFilterWeights(){
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

void MMKalmanFilter::initialize(float relX, float relY, float covX, float covY)
{
    // clear the filters
    filters.clear();

    // make a random generator for initilizing different filters
    boost::mt19937 rng;
    rng.seed(std::time(0));
    boost::uniform_real<float> posCovRange(-2.f, 2.f);
    boost::variate_generator<boost::mt19937&,
                             boost::uniform_real<float> > positionGen(rng, posCovRange);
    boost::uniform_real<float> randVelRange(20.f, 150.f);
    boost::variate_generator<boost::mt19937&,
                             boost::uniform_real<float> > velocityGen(rng, randVelRange);

    // // make 6 stationary
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
        stationaryFilter->initialize(x, cov);

        filters.push_back(stationaryFilter);
    }

    // make 6 moving
    for (int i=0; i<params.numFilters/2; i++)
    {
        // Needs to be stationary, have given mean, and add noise
        //   to the covariance matrix
        KalmanFilter *movingFilter = new KalmanFilter(false);
        ufvector4 x = boost::numeric::ublas::zero_vector<float>(4);
        x(0)= relX;
        x(1)= relY;
        x(2) = velocityGen();
        x(3) = velocityGen();
        //= vector4D(relX, relY, velocityGen(), velocityGen());

        ufmatrix4 cov = boost::numeric::ublas::zero_matrix<float>(4);
        cov(0,0) = covX;// + positionGen();
        cov(1,1) = covY;// + positionGen();
        cov(2,2) = 20.f;
        cov(3,3) = 20.f;

        movingFilter->initialize(x, cov);
        filters.push_back(movingFilter);
    }
}
// for offline
void MMKalmanFilter::predictFilters(messages::RobotLocation odometry, float t)
{
    deltaTime = t;
    for (std::vector<KalmanFilter *>::iterator it = filters.begin(); it != filters.end(); it++)
    {
        (*it)->predict(odometry, deltaTime);
    }
}

void MMKalmanFilter::predictFilters(messages::RobotLocation odometry)
{
    updateDeltaTime();

    for (std::vector<KalmanFilter *>::iterator it = filters.begin(); it != filters.end(); it++)
    {
        (*it)->predict(odometry, deltaTime);
    }
}

void MMKalmanFilter::updateWithVision(messages::VisionBall visionBall)
{
    for (std::vector<KalmanFilter *>::iterator it = filters.begin(); it != filters.end(); it++)
    {
        (*it)->updateWithObservation(visionBall);
    }
}

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

}
}
