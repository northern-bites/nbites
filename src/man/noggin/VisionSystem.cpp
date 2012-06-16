#include "VisionSystem.h"

//TODO: move this to a sensible header
static const float MAX_CORNER_DISTANCE = 400.0f;

/**
 * Constructor
 */
VisionSystem::VisionSystem(Vision::const_ptr vision, LocalizationVisionParams params)
    : PF::SensorModel(), vision(vision), parameters(params)
{ }

/**
 * Update particle weights based on current visual observations.
 * @param particles the particle set to be updated with sensor
 *                  data.
 * @return the updated particle set.
 */
PF::ParticleSet VisionSystem::update(PF::ParticleSet particles)
{

#ifdef DEBUG_LOCALIZATION
    std::cout << "Using current location " << currentLocation << std::endl;
#endif
    // FOR TESTING, should not be in this scope
    int count = 0;

    float bestCornerProbabilities = 0.0f;
    int count_corner = 0;

    PF::ParticleIt partIter;
    for(partIter = particles.begin(); partIter != particles.end(); ++partIter)
    {
        float totalWeight = 0.0f;
//        int count = 0;

        // Visual objects

        // TODO: a bit nicer if we had a visual field object iterator of sorts in vision ...
//        incorporateLandmarkObservation<VisualFieldObject, ConcreteFieldObject>(*(vision->yglp),
//                *(partIter), totalWeight, count);
//        incorporateLandmarkObservation<VisualFieldObject, ConcreteFieldObject>(*(vision->ygrp),
//                *(partIter), totalWeight, count);
//
//
//        // HACK HACK HACK
//        float bestProbability = 0.0f;
//
//        if (vision->yglp->hasPositiveID() && vision->ygrp->hasPositiveID())
//        {
//            for (int i=0; i<2; i++)
//            {
//                point<float> leftPost, rightPost;
//                if(i==0){
//                    leftPost = BLUE_GOAL_TOP_POST_LOC;
//                    rightPost = BLUE_GOAL_BOTTOM_POST_LOC;
//                }
//                else{
//                    leftPost = YELLOW_GOAL_BOTTOM_POST_LOC;
//                    rightPost = YELLOW_GOAL_TOP_POST_LOC;
//                }
//
//                float i = rightPost.x - leftPost.x;
//                float j = rightPost.y - leftPost.y;
//
//                float sideA = vision->yglp->getDistance();
//                float sideB = vision->ygrp->getDistance();
//                float sideC = hypotf(i,j);
//
//                /**
//                 * Using Law of Sines
//                 *      c         b         a
//                 *   ------- = ------- = -------
//                 *    sin(C)    sin(B)    sin(A)
//                 *
//                 *     self   b
//                 *        +---------+ pt2  ^
//                 *        |C      A/       |
//                 *        |       /        |
//                 *        |      /         |
//                 *      a |     / c        |
//                 *        |    /        i  |   (x goes up, like on the field.
//                 *        |   /            |            Heading of zero is up)
//                 *        |B /             |
//                 *        | /              |
//                 *        +/               _
//                 *      pt1
//                 *            j
//                 *       |----------->
//                 */
//
//                // Calculate angle between vector (pt1,pt2) and north (zero heading)
//                float ptVecHeading = copysignf(1.0f, j) * acosf(clip(i/sideC, -1.0f, 1.0f));
//                float angleC = fabs(subPIAngle(vision->yglp->getBearing() -
//                                              vision->ygrp->getBearing()));
//
//                // Clamp the input to asin to within (-1 , 1) due to measurement
//                // inaccuracies. This prevents a nan return from asin.
//                float angleB = asinf( clip( (sideB * sin(angleC)) /sideC, -1.0f, 1.0f));
//
//                // Swap sign of angle B to place us on the correct side of the
//                // line (pt1 -> pt2)
//                if (NBMath::subPIAngle(vision->yglp->getBearing() - vision->ygrp->getBearing()) > 0){
//                    angleB = -angleB;
//                }
//
//                // x_hat, y_hat are x and y in coord frame with x axis pointed
//                // from (pt1 -> pt2) and y perpendicular
//                float x_hat = sideA * cos(angleB);
//                float y_hat = sideA * sin(angleB);
//
//                // Transform to global x and y coordinates
//                float newX = x_hat * cos(ptVecHeading) - y_hat * sin(ptVecHeading) + leftPost.x;
//                float newY = x_hat * sin(ptVecHeading) + y_hat * cos(ptVecHeading) + leftPost.y;
//
//                // Heading of line (self -> pt2)
//                // Clamp the input to (-1,1) to prevent illegal trig call and a nan return
//                float headingPt2 = acosf(clip( (rightPost.x - newX)/sideB, -1.0f, 1.0f) );
//                // Sign based on y direction of vector (self -> pt2)
//                float signedHeadingPt2 = copysignf(1.0f, rightPost.y - newY) * headingPt2;
//
//                // New global heading
//                float newH = NBMath::subPIAngle(signedHeadingPt2 - vision->ygrp->getBearing());
//
//                //Determine distanceSD and bearingSD
//                float distanceSD = hypotf(vision->yglp->getDistanceSD(),
//                                          vision->ygrp->getDistanceSD());
//                float bearingSD = hypotf(vision->yglp->getBearingSD(),
//                                          vision->ygrp->getBearingSD());
//
//                // Reconstructed to newX, newY, newH
//                PF::Location pose(newX, newY, newH);
//                float probability = scoreParticleAgainstPose(*(partIter),
//                                                             pose,
//                                                             distanceSD,
//                                                             bearingSD);
//                if (probability > bestProbability)
//                    bestProbability = probability;
//            }
//
//            totalWeight = updateTotalWeight(totalWeight, bestProbability);
//
//        }



        // Visual cross
//        incorporateLandmarkObservation<VisualCross, ConcreteCross>(*(vision->cross),
//                                *(partIter), totalWeight, count);

        std::list<VisualCorner> * corners = vision->fieldLines->getCorners();
        std::list<VisualCorner>::const_iterator vc;
        for(vc = corners->begin(); vc != corners->end(); ++vc)
        {
            if (vc->getDistance() < MAX_CORNER_DISTANCE && vc->isReliable())
            {
                float bestProbability = 0.0f;
                typedef const std::list<const ConcreteCorner*> ConcreteCorners;

                ConcreteCorners* concreteCorners = vc->getPossibilities();
                ConcreteCorners::const_iterator cc;

                for (cc = concreteCorners->begin(); cc != concreteCorners->end(); cc++) {
                    float probability = scoreFromCorner(*vc, **cc, *partIter);

                    if (probability > bestProbability)
                        bestProbability = probability;
                }

                bestCornerProbabilities+=bestProbability;
                count_corner++;

                totalWeight = updateTotalWeight(totalWeight, bestProbability);
                count++;

            } else{
//                std::cout << "We saw a corner REALLY far away: "
//                        << vc->getDistance()<< " centimeters away" <<std::endl
//                        << "They can't be more than" << MAX_CORNER_DISTANCE
//                        << " centimeters away." << std::endl;
            }
        }

        // Make sure that we have made an observation before updating weights.
        if(count > 0)
        {
#ifdef DEBUG_LOCALIZATION
            std::cout << "Updating particle of previous weight " << (*partIter).getWeight();
#endif
            // @todo although this should never happen, it is possible that underflow
            // errors cause nan's, which would be problematic for resampling.
            if(std::isnan(totalWeight) || std::abs(totalWeight) == HUGE_VAL)
            {
                std::cout << "Invalid weight calculated!" << std::endl;
                totalWeight = 0.0f;
            }
            // std::cout << "(" << (*partIter).getLocation().x
            //            << ", " << (*partIter).getLocation().y
            //            << ", " << (*partIter).getLocation().heading
            //            << ") : " << totalWeight << std::endl;
            (*partIter).setWeight(totalWeight);
#ifdef DEBUG_LOCALIZATION
            std::cout << " with new weight " << (*partIter).getWeight() << std::endl;
#endif
        } else {
            //no observations, no use trying to re-weigh the other particles
            setUpdated(false);
//            std::cout << "We made no new observations \n";
            return particles;
        }
    }

//    if (bestCornerProbabilities == 0.0f && count_corner > 0 && particles.size() > 0) {
//        throw(std::exception());
//    }

    //std::cout << "---------------------------------------------" << std::endl;
    setUpdated(true);
    return particles;
}
