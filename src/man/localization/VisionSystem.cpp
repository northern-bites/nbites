#include "VisionSystem.h"

namespace man
{
namespace localization
{

VisionSystem::VisionSystem(){}

VisionSystem::~VisionSystem(){}

/**
 * Updates the particle set according to the observations from Vision
 * Assume that we are given NEW information (ie we checked for repeat message elsewhere)
 *
 * @return if observations were made
 */
bool VisionSystem::update(ParticleSet& particles,
                          const messages::VisionField& obsv)
{
    ParticleIt iter;
    // Record totalWeight for normalization
    float totalWeight = 0.0f;
    bool madeObsv = false;

    float times = 0;
    float lowestParticleError = 10000000.f;
    float sumParticleError = 0.f;

    for(iter = particles.begin(); iter != particles.end(); iter++)
    {
        Particle* particle = &(*iter);

        float curParticleError = 0;
        int numObsv = 0;
        for (int i=0; i<obsv.visual_corner_size(); i++)
        {
            if(obsv.visual_corner(i).visual_detection().distance() > 0.f) {
                madeObsv = true;

                float newError = scoreFromVisDetect(*particle,
                                                    obsv.visual_corner(i).visual_detection());
                curParticleError+= newError;
                numObsv++;

            }
        }

        if (obsv.has_goal_post_l() && obsv.goal_post_l().visual_detection().on()
            && (obsv.goal_post_l().visual_detection().distance() > 0.f)) {
            madeObsv = true;
            float newError = scoreFromVisDetect(*particle,
                                                obsv.goal_post_l().visual_detection());
            curParticleError+= newError;
            numObsv++;
        }

        if (obsv.has_goal_post_r() && obsv.goal_post_r().visual_detection().on()
            && (obsv.goal_post_r().visual_detection().distance() > 0.f)) {
            madeObsv = true;
            float newError = scoreFromVisDetect(*particle,
                                                obsv.goal_post_r().visual_detection());
            curParticleError+= newError;
            numObsv++;
        }

        if (obsv.visual_cross().distance() > 0.f) {
            madeObsv = true;
            float newError = scoreFromVisDetect(*particle,
                                                obsv.visual_cross());
            curParticleError+= newError;
            numObsv++;
        }

        // We never updated the new particle weight, so no observations been made
        if(!madeObsv)
        {
            // std::cout << "In the Vision System, given a message with no observations...\n";
            return false;
        }
        else
        {
            // Set the particle weight to 1/predictionError (no golf scores...)
            // std::cout << "Particle Error\t" << curParticleError << std::endl;
            float avgErr = curParticleError/(float)numObsv;
            particle->setWeight(1/avgErr);
            totalWeight += particle->getWeight();

            // Update the total swarm error
            sumParticleError += avgErr;
            particle->setError(avgErr);
            if (avgErr < lowestParticleError)
                lowestParticleError = avgErr;

            lastNumObsv = numObsv;
        }
    }

    //std::cout << "Vision System given observations, should resample" << std::endl;
    //std::cout << "Normalize the weights\n";


    // normalize the particle weights and calculate the weighted avg error
    weightedAvgError = 0.f;
    for(iter = particles.begin(); iter != particles.end(); iter++)
    {
        Particle* particle = &(*iter);
        particle->normalizeWeight(totalWeight);

        weightedAvgError += particle->getWeight() * particle->getError();
    }

    // Calc avgError by dividing the total by the num particles
    avgError = sumParticleError / (float)particles.size();

    if (madeObsv)
        currentLowestError = lowestParticleError;

    // Clear out the reconstructed observation list
    reconstructedLocations.clear();

    // for each corner add the reconstructions
    for (int i=0; i<obsv.visual_corner_size(); i++)
    {
        if(obsv.visual_corner(i).visual_detection().distance() > 0.f)
            addCornerReconstructionsToList(obsv.visual_corner(i));
    }

    // Succesfully updated particles with Vision!
    return true;
}

/**
 * @brief Takes a PVisualObservation and particle & returns the
 *        distance between the observations real location and its
 *        expected one
 */
float VisionSystem::scoreFromVisDetect(const Particle& particle,
                                       const messages::VisualDetection& obsv)
{
    float bestScore = 100000;

    for (int i=0; i<obsv.concrete_coords_size(); i++)
    {
        // Convert from obsv in polar to rep in cartesian
        // @Todo:  Explain these calculations somewhere!!
        float sin, cos;
        sincosf((particle.getLocation().h() + obsv.bearing()), &sin, &cos);
        float calcX = obsv.distance()*cos + particle.getLocation().x();
        float calcY = obsv.distance()*sin + particle.getLocation().y();

        // Calc distance between calculated coordinates and the concrete coords
        float dist = std::sqrt(NBMath::square(calcX - obsv.concrete_coords(i).x())
                               + NBMath::square(calcY - obsv.concrete_coords(i).y()));

        float score = dist;
        if (score < bestScore)
            bestScore = score;
    }
    //std::cout <<"Scored a particle\n";
    return bestScore;
}

/**
 * Takes in a corner observation as well as the dist, bear measurements to
 * that observation to calculate the exact location of the robot on the field
 */
void VisionSystem::addCornerReconstructionsToList(messages::VisualCorner corner)
{
    int concreteNum = 0;
    // Loop through all concrete coords of the corner
    for (int i=0; i< corner.visual_detection().concrete_coords_size(); i++)
    {
        concreteNum++;

        //angle between the robot's visual heading line (or bearing to corner line)
        //and the line parallel to the x axis oriented towards the corner
        //(so x axis flipped)
        float globalPhysicalOrientation = corner.physical_orientation()
            + corner.visual_detection().concrete_coords(i).field_angle();

        float sin_global_orientation, cos_global_orientation;
        //sin and cos altogether (faster)
        sincosf(globalPhysicalOrientation, &sin_global_orientation, &cos_global_orientation);

        float pose_x = corner.visual_detection().concrete_coords(i).x()
                       - corner.visual_detection().distance()*cos_global_orientation;
        float pose_y = corner.visual_detection().concrete_coords(i).y()
                       - corner.visual_detection().distance()*sin_global_orientation;
        float pose_h = globalPhysicalOrientation - corner.visual_detection().bearing();
        float side   = pose_x < CENTER_FIELD_X;

        ReconstructedLocation newLoc(pose_x, pose_y, pose_h, side);

        reconstructedLocations.push_back(newLoc);
    }
    //std::cout << concreteNum << " particles should be injected" << std::endl;
}

void VisionSystem::opitmizeReconstructions()
{
    // // The idea here is to go through the list and only keep locations
    // // which are reconstructed more than once.

    std::list<ReconstructedLocation> optimized;
    // Add all of the corners from the list to the map with key 0
    std::list<ReconstructedLocation>::const_iterator iter1, iter2;
    iter1 = reconstructedLocations.begin();
    for(int i=0; i<(int)reconstructedLocations.size(); i++)
    {
        // Compare each particle to all the others
        int numSimReconstructions = 0;
        for(int j=0; j<(int)reconstructedLocations.size(); j++)
        {
            iter2 = reconstructedLocations.begin();
            //compare
            if ( (*iter1) == (*iter2))
                numSimReconstructions++;

            iter2++;
        }
        if (numSimReconstructions > 1) {
            optimized.push_back((*iter1));
            std::cout << "Injecting a confident corner reconstruction" << std::endl;
        }
        iter1++;
    }

    reconstructedLocations.clear();
    std::list<ReconstructedLocation>::const_iterator optIt;
    for(optIt = optimized.begin(); optIt != optimized.end(); optIt++)
        reconstructedLocations.push_back((*optIt));
}

} // namespace localization
} // namespace man
