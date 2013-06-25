#include "VisionSystem.h"

namespace man {
namespace localization {

VisionSystem::VisionSystem() {
    lineSystem = new LineSystem;
}

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
    sawGoal = false;

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
            if((obsv.visual_corner(i).visual_detection().distance() > 0.f) &&
               (obsv.visual_corner(i).visual_detection().distance() < 400.f)) {
                madeObsv = true;

                float newError = scoreFromVisDetect(*particle,
                                                    obsv.visual_corner(i).visual_detection());
//                std::cout << "Corner Error:\t" << newError << std::endl;
                curParticleError+= newError;
                numObsv++;

            }
        }

        for (int i=0; i<obsv.visual_line_size(); i++) {
            if((obsv.visual_line(i).start_dist() < 300.f) || (obsv.visual_line(i).end_dist() < 300.f)) {
                Line obsvLine = prepareVisualLine(particle->getLocation(),
                                                  obsv.visual_line(i),
                                                  true);

                // Limit by line length (be safe about center circle mistake lines)
                if ((obsvLine.length() > 90.f) && (obsvLine.length() < 500.f)) {
                    madeObsv = true;
                    float newError = lineSystem->scoreObservation(obsvLine);
//                    std::cout << "Line Error:\t" << newError << std::endl;
                    curParticleError += newError;
                    numObsv++;
                }
            }
        }

        for (int i=0; i<obsv.bottom_line_size(); i++) {
            if((obsv.bottom_line(i).start_dist() < 300.f) || (obsv.bottom_line(i).end_dist() < 300.f)) {
                Line obsvLine = prepareVisualLine(particle->getLocation(),
                                                  obsv.bottom_line(i));

                // Limit by line length (be safe about center circle mistake lines)
                if ((obsvLine.length() > 40.f) && (obsvLine.length() < 100.f)) {
                    madeObsv = true;
                    float newError = lineSystem->scoreObservation(obsvLine);
//                    std::cout << "Line Error:\t" << newError << std::endl;
                    curParticleError += newError;
                    numObsv++;
                }
            }
        }

        if (obsv.has_goal_post_l() && obsv.goal_post_l().visual_detection().on()
            && (obsv.goal_post_l().visual_detection().distance() > 0.f)
            && (obsv.goal_post_l().visual_detection().distance() < 600.f)) {
            madeObsv = true;
            float newError = scoreFromVisDetect(*particle,
                                                obsv.goal_post_l().visual_detection());
//            std::cout << "Goalpost Error:\t" << newError << std::endl;
            curParticleError+= newError;
            numObsv++;
            sawGoal = true;
        }

        if (obsv.has_goal_post_r() && obsv.goal_post_r().visual_detection().on()
            && (obsv.goal_post_r().visual_detection().distance() > 0.f)
            && (obsv.goal_post_r().visual_detection().distance() < 600.f)) {
            madeObsv = true;
            float newError = scoreFromVisDetect(*particle,
                                                obsv.goal_post_r().visual_detection());
//            std::cout << "Goalpost Error:\t" << newError << std::endl;
            curParticleError+= newError;
            numObsv++;
            sawGoal = true;
        }

        if ((obsv.visual_cross().distance() > 0.f)
            && (obsv.visual_cross().distance() < 400.f)) {
            madeObsv = true;
            float newError = scoreFromVisDetect(*particle,
                                                obsv.visual_cross());
//            std::cout << "Cross Error:\t" << newError << std::endl;
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

    // normalize the particle weights and calculate the weighted avg error
    weightedAvgError = 0.f;
    float weightSum = 0.f;
    for(iter = particles.begin(); iter != particles.end(); iter++)
    {
        Particle* particle = &(*iter);
        particle->normalizeWeight(totalWeight);

        //sanity check for potential weird bug
        weightSum += particle->getWeight();

        weightedAvgError += particle->getWeight() * particle->getError();
    }

    if (weightSum < .9f)
        std::cout << "LOC_ERROR: All particle weights do not sum to .9" << std::endl;

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

bool VisionSystem::update(ParticleSet& particles,
                          const messages::VisionField& obsv,
                          const messages::FilteredBall& ballObsv)
{
    ParticleIt iter;
    // Record totalWeight for normalization
    float totalWeight = 0.0f;
    bool madeObsv = false;
    sawGoal = false;

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
            if((obsv.visual_corner(i).visual_detection().distance() > 0.f) &&
               (obsv.visual_corner(i).visual_detection().distance() < 400.f)) {
                madeObsv = true;

                float newError = scoreFromVisDetect(*particle,
                                                    obsv.visual_corner(i).visual_detection());
//                std::cout << "Corner Error:\t" << newError << std::endl;
                curParticleError+= newError;
                numObsv++;

            }
        }

        for (int i=0; i<obsv.visual_line_size(); i++) {
            if((obsv.visual_line(i).start_dist() < 300.f) || (obsv.visual_line(i).end_dist() < 300.f)) {
                Line obsvLine = prepareVisualLine(particle->getLocation(),
                                                  obsv.visual_line(i),
                                                  true);

                // Limit by line length (be safe about center circle mistake lines)
                if ((obsvLine.length() > 90.f) && (obsvLine.length() < 500.f)) {
                    madeObsv = true;
                    float newError = lineSystem->scoreObservation(obsvLine);
                    //std::cout << "Line Error:\t" << newError << std::endl;
                    curParticleError += newError;
                    numObsv++;
                }
            }
        }

        for (int i=0; i<obsv.bottom_line_size(); i++) {
            if((obsv.bottom_line(i).start_dist() < 300.f) || (obsv.bottom_line(i).end_dist() < 300.f)) {
                Line obsvLine = prepareVisualLine(particle->getLocation(),
                                                  obsv.bottom_line(i));

                // Limit by line length (be safe about center circle mistake lines)
                if ((obsvLine.length() > 40.f) && (obsvLine.length() < 100.f)) {
                    madeObsv = true;
                    float newError = lineSystem->scoreObservation(obsvLine);
//                    std::cout << "Line Error:\t" << newError << std::endl;
                    curParticleError += newError;
                    numObsv++;
                }
            }
        }


        if (obsv.has_goal_post_l() && obsv.goal_post_l().visual_detection().on()
            && (obsv.goal_post_l().visual_detection().distance() > 0.f)
            && (obsv.goal_post_l().visual_detection().distance() < 480.f)) {
            madeObsv = true;
            float newError = scoreFromVisDetect(*particle,
                                                obsv.goal_post_l().visual_detection());
//            std::cout << "Goalpost Error:\t" << newError << std::endl;
            curParticleError+= newError;
            numObsv++;
            sawGoal = true;
        }

        if (obsv.has_goal_post_r() && obsv.goal_post_r().visual_detection().on()
            && (obsv.goal_post_r().visual_detection().distance() > 0.f)
            && (obsv.goal_post_r().visual_detection().distance() < 480.f)) {
            madeObsv = true;
            float newError = scoreFromVisDetect(*particle,
                                                obsv.goal_post_r().visual_detection());
//            std::cout << "Goalpost Error:\t" << newError << std::endl;
            curParticleError+= newError;
            numObsv++;
            sawGoal = true;
        }

        if ((obsv.visual_cross().distance() > 0.f)
            && (obsv.visual_cross().distance() < 400.f)) {
            madeObsv = true;
            float newError = scoreFromVisDetect(*particle,
                                                obsv.visual_cross());
//            std::cout << "Cross Error:\t" << newError << std::endl;
            curParticleError+= newError;
            numObsv++;
        }

        // Have we seen the ball for a few frames?
        if (ballObsv.vis().frames_on() > 3.f) {
            madeObsv = true;
            curParticleError += scoreFromBallObsv(*particle,
                                                  ballObsv);
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

    // normalize the particle weights and calculate the weighted avg error
    weightedAvgError = 0.f;
    float weightSum = 0.f;
    for(iter = particles.begin(); iter != particles.end(); iter++)
    {
        Particle* particle = &(*iter);
        particle->normalizeWeight(totalWeight);

        //sanity check for potential weird bug
        weightSum += particle->getWeight();

        weightedAvgError += particle->getWeight() * particle->getError();
    }

    if (weightSum < .9f)
        std::cout << "LOC_ERROR: All particle weights do not sum to .9" << std::endl;

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
 * @brief Takes a particle and a Visual line (with one endpoint within thresh dist,
 *        and returns a Line with the closest endpoint being start,
 *        and in global coordinates
 */
Line VisionSystem::prepareVisualLine(const messages::RobotLocation& loc,
                                     const messages::VisualLine& line,
                                     bool stdLineLength)
{
    float startGlobalX, startGlobalY, endGlobalX, endGlobalY;
    float distToStart, distToEnd;

    // Transform to global (make startGlobal closer than endGlobal)
    if ( line.start_dist() < line.end_dist() ) {
        float sinS, cosS;
        sincosf((loc.h() + line.start_bearing()), &sinS, &cosS);
        startGlobalX = line.start_dist()*cosS + loc.x();
        startGlobalY = line.start_dist()*sinS + loc.y();

        float sinE, cosE;
        sincosf((loc.h() + line.end_bearing()), &sinE, &cosE);
        endGlobalX = line.end_dist()*cosE + loc.x();
        endGlobalY = line.end_dist()*sinE + loc.y();

        distToStart = line.start_dist();
        distToEnd = line.end_dist();
    }
    else { // 'end' from vision is closer
        float sinS, cosS;
        sincosf((loc.h() + line.start_bearing()), &sinS, &cosS);
        endGlobalX = line.start_dist()*cosS + loc.x();
        endGlobalY = line.start_dist()*sinS + loc.y();

        float sinE, cosE;
        sincosf((loc.h() + line.end_bearing()), &sinE, &cosE);
        startGlobalX = line.end_dist()*cosE + loc.x();
        startGlobalY = line.end_dist()*sinE + loc.y();

        distToStart = line.end_dist();
        distToEnd = line.start_dist();
    }

    // Safety check
    if (distToEnd < distToStart)
        std::cout << "\n\n MASSIVE LOCALIZATION ERROR \n\n" << std::endl;

    Point start(startGlobalX, startGlobalY);
    Point end  (  endGlobalX,   endGlobalY);

    // Ensure reasonable distance estimates
    if (distToEnd > 300.f) {
        Line initialSegment(start,end); // Use to find end point to make shorter segment

        // Project pose onto the line
        Point pose(loc.x(), loc.y());
        Point proj = initialSegment.project(pose);

        //  Calc dists
        float poseToProj = pose.distanceTo(proj);
        float projToEnd  = proj.distanceTo(end);

        // Amount to shift for given distance
        float hyp = 300.f; // thresh
        float projToNewEnd = std::sqrt(hyp*hyp - poseToProj*poseToProj);
        float endToNewEnd  = projToEnd - projToNewEnd;

        // Shift (and confirm lands on the line)
        if (initialSegment.containsPoint(initialSegment.shiftDownLine(end, endToNewEnd))) {
            Point newEnd = initialSegment.shiftDownLine(end, endToNewEnd);
            end.x = newEnd.x;
            end.y = newEnd.y;
        }
        else if (initialSegment.containsPoint(initialSegment.shiftDownLine(end, -endToNewEnd))) {
            Point newEnd = initialSegment.shiftDownLine(end, -endToNewEnd);
            end.x = newEnd.x;
            end.y = newEnd.y;
        }
        else {
            // There is an issue in processing this line (precision) so make sure it's thrown out
            Point shitEnd = initialSegment.shiftDownLine(start, 1);
            end.x = shitEnd.x;
            end.y = shitEnd.y;
        }
    }

    Line toReturn(start, end);

    if (stdLineLength) { // want the line to be clipped at 150 cm in length
        if (toReturn.length() > 120.f) {
            // We know start is closest so lets trust that endpoint the most
            Point newEnd = toReturn.shiftDownLine(start, 120.f);
            if (!toReturn.containsPoint(newEnd)) { // missed the segment
                newEnd = toReturn.shiftDownLine(start, -120.f);
                end.x = newEnd.x;
                end.y = newEnd.y;
            }
        }
    }

    return Line(start,end);
}

float VisionSystem::scoreFromBallObsv(const Particle& particle,
                                      const messages::FilteredBall& obsv)
{
    // Note: only use the ball in set, so its in the middle of the field
    float concreteX = CENTER_FIELD_X;
    float concreteY = CENTER_FIELD_Y;

    // Convert from obsv in polar to rep in cartesian
    float sin, cos;
    sincosf((particle.getLocation().h() + obsv.bearing()), &sin, &cos);
    float calcX = obsv.distance()*cos + particle.getLocation().x();
    float calcY = obsv.distance()*sin + particle.getLocation().y();

    // Calc distance between calculated coordinates and the concrete coords
    float dist = std::sqrt(NBMath::square(calcX - concreteX)
                         + NBMath::square(calcY - concreteY));

    return dist;
}

/**
 * @brief Takes a PVisualDetection and particle & returns the
 *        distance between the observations real location and its
 *        expected one
 */
float VisionSystem::scoreFromVisDetect(const Particle& particle,
                                       const messages::VisualDetection& obsv)
{
    float bestScore = 100000;
    float bearErrOfBest = 0.f;


    for (int i=0; i<obsv.concrete_coords_size(); i++)
    {
        // Convert from obsv in polar to rep in cartesian
        float sin, cos;
        sincosf((particle.getLocation().h() + obsv.bearing()), &sin, &cos);
        float calcX = obsv.distance()*cos + particle.getLocation().x();
        float calcY = obsv.distance()*sin + particle.getLocation().y();

        // Calc distance between calculated coordinates and the concrete coords
        float dist = std::sqrt(NBMath::square(calcX - obsv.concrete_coords(i).x())
                               + NBMath::square(calcY - obsv.concrete_coords(i).y()));

        float score = dist;
        if (score < bestScore) {
            bestScore = score;

            // For experiments, record the bearing error
            // Use Law of Cosines
            Point c(particle.getLocation().x(), particle.getLocation().y());
            Point m(obsv.concrete_coords(i).x(),
                    obsv.concrete_coords(i).y());
            float b = c.distanceTo(m);

            float toACos = ( NBMath::square(obsv.distance()) + NBMath::square(b)
                             - NBMath::square(dist) )/(2*obsv.distance()*b);
            bearErrOfBest = NBMath::safe_acos(toACos);
        }
    }
    //std::cout <<"Scored a particle\n";
//    std::cout << "err " << TO_DEG*bearErrOfBest << std::endl;
    return TO_DEG*bearErrOfBest;
//    return bestScore;
}

/**
 * Takes in a corner observation as well as the dist, bear measurements to
 * that observation to calculate the exact location of the robot on the field
 */
void VisionSystem::addCornerReconstructionsToList(messages::VisualCorner corner)
{
    //We now hate center circles. so fuck em
    for (int j = 0; j < corner.poss_id_size(); j++) {
        if (corner.poss_id(j) == 30 || corner.poss_id(j) == 29)
            return;
    }

    //could it be a middle T?
    bool midT;
    for (int j = 0; j < corner.poss_id_size(); j++) {
        if (corner.poss_id(j) == 27 || corner.poss_id(j) == 28)
            midT = true;
    }

    // Only reconstruct if we saw goals (confident in the corner id) or around mid and desperate
    if (!sawGoal && !midT)
        return;

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

        // Sanity check the reconstructinos aren't off-field
        // Lets assume if we're off-field then we're already fucked
        if( (newLoc.x >= 0 && newLoc.y <= FIELD_GREEN_WIDTH) &&
            (newLoc.y >= 0 && newLoc.y <= FIELD_GREEN_HEIGHT)  ) {
//            std::cout << "Reconstruct " << newLoc.x << " " <<  newLoc.y <<std::endl;
            reconstructedLocations.push_back(newLoc);
        }
    }
    //std::cout << concreteNum << " particles should be injected" << std::endl;
}

void VisionSystem::optimizeReconstructions()
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

float VisionSystem::getConfidenceError(const messages::RobotLocation& loc,
                                    const messages::VisionField&   obsv)
{
    float sumLineError  = 0.f;
    int   numValidLines = 0;
    float lineLength    = 0.f;

    // Go through each observed line
    for (int i=0; i<obsv.visual_line_size(); i++) {
        // Only use lines that start reasonably close
        if((obsv.visual_line(i).start_dist() < 250.f) || (obsv.visual_line(i).end_dist() < 250.f)) {

            // Prepare an observed line, but no lines longer than 1.2 meters
            Line obsvLine = prepareVisualLine(loc,
                                              obsv.visual_line(i),
                                              true);

            // Limit by line length (be safe about center circle mistake lines)
            if (obsvLine.length() > 118.f) {
                sumLineError += lineSystem->scoreObservation(obsvLine);
                numValidLines++;
                lineLength += obsvLine.length();
            }
        }

    }

    if (numValidLines > 0)
        return sumLineError/(float)numValidLines;
    else
        return -1.f;
}




} // namespace localization
} // namespace man
