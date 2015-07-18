#include "VisionSystem.h"

#include "../vision/Hough.h"
#include "FieldConstants.h"
#include "Profiler.h"
#include "HighResTimer.h"

#include <limits>

namespace man {
namespace localization {

VisionSystem::VisionSystem() 
    : injections(), numObservations(0), avgError(0)
{
    lineSystem = new LineSystem;
    landmarkSystem = new LandmarkSystem;
}

VisionSystem::~VisionSystem() 
{
    delete lineSystem;
    delete landmarkSystem;
}

bool VisionSystem::update(ParticleSet& particles,
                          const messages::Vision& vision,
                          const messages::FilteredBall* ball,
                          const messages::RobotLocation& lastEstimate)
{
    numObservations = 0;
    avgError = 0;

    // Count observations
    for (int i = 0; i < vision.line_size(); i++) {
        if (LineSystem::shouldUse(vision.line(i), lastEstimate))
            numObservations++;
    }
    for (int i = 0; i < vision.corner_size(); i++)
        numObservations++;
    if (vision.circle().on())
        numObservations++;
    bool useBall = (ball != NULL) && ball->vis().on();
    if (useBall)
        numObservations++;

    // If no observations, return without updating weights
    if (!numObservations)
        return false;

    // Loop over particles and adjust weights
    ParticleIt iter;
    double totalWeight = 0;

    // Timing
    HighResTimer timer;
    PROF_ENTER(P_LOCV_SCORING)

    for (iter = particles.begin(); iter != particles.end(); iter++) {
        Particle* particle = &(*iter);
        float curParticleError = 1;

        // Score particle from line observations
        for (int i = 0; i < vision.line_size(); i++) {
            if (!LineSystem::shouldUse(vision.line(i), lastEstimate))
                continue;
            curParticleError = curParticleError*lineSystem->scoreLine(vision.line(i), particle->getLocation());
            break;
            // IMPORTANT currently we only have the time to process a single line
            //           optimizing this inner loop, so more lines can be 
            //           processed is an excellent idea
        }

        // Score particle from corner observations
        for (int i = 0; i < vision.corner_size(); i++)
            curParticleError = curParticleError*landmarkSystem->scoreCorner(vision.corner(i), particle->getLocation());

        // Score particle from center circle if on
        if (vision.circle().on())
            curParticleError = curParticleError*landmarkSystem->scoreCircle(vision.circle(), particle->getLocation());

        // Score particle from ball observation if in game set
        if (useBall)
            curParticleError = curParticleError*landmarkSystem->scoreBallInSet(*ball, particle->getLocation());

        // Set the particle's weight
        particle->setWeight(curParticleError);
        totalWeight += particle->getWeight();
    }

    PROF_EXIT(P_LOCV_SCORING)
    double time = timer.end();

    // China 2015 hack
    // Loc is sometimes causing cognition thread to overrun, these
    // print outs print when loc took longer than we'd like
    // if (time > 15) {
    //     printf("==================\n");
    //     printf("LOC TOOK: %f\n", time);
    //     printf("With %d observations\n", numObservations);
    //     printf("Circle bool: %d\n", vision.circle().on());
    //     printf("UseBall bool: %d\n", useBall);
    //     printf("NumCorner: %d\n", vision.corner_size());
    //     printf("NumLines:  %d\n", vision.line_size());
    // }

    // Normalize the particle weights
    for (iter = particles.begin(); iter != particles.end(); iter++) {
        Particle* particle = &(*iter);
        particle->normalizeWeight(totalWeight);
    }

    // Calculate error metric for particle filter
    avgError = totalWeight / static_cast<float>(particles.size());

    // Particle injections
    PROF_ENTER(P_LOCV_INJECTION)
    injections.clear();

    // (1) Reconstruct pose from ball in set
    if (useBall && ball->vis().frames_on() > 5) {
        messages::FieldLine midline;
        double minDist = std::numeric_limits<double>::max();

        // Ball, polar to cartesian
        double ballRelX, ballRelY;
        vision::polarToCartesian(ball->distance(), ball->bearing(), ballRelX, ballRelY);

        // Find line that is closest to the ball, should be midline since in set
        for (int i = 0; i < vision.line_size(); i++) {
            const messages::FieldLine& field = vision.line(i);
            const messages::HoughLine& inner = field.inner();

            // Rotate line to loc rel robot coordinate system 
            vision::GeoLine line;
            line.set(inner.r(), inner.t(), inner.ep0(), inner.ep1());

            // Project ball onto line, find distance to line
            double distToLine = fabs(line.pDist(ballRelX, ballRelY));

            // Check for min distance
            if (minDist > distToLine) {
                midline = field;
                minDist = distToLine;
            }
        }

        // If sufficiently close, found the midline, reconstruct location
        if (minDist < 60) {
            // Recontruct x and h from midline and y from ball
            messages::RobotLocation fromLine = lineSystem->reconstructWoEndpoints(LocLineID::OurMidline, midline);
            messages::RobotLocation fromLineAndBall = fromLine;

            // Rotate to absolute coordinate system
            double ballAbsX, ballAbsY;
            vision::translateRotate(ballRelX, ballRelY, 0, 0, fromLine.h(), ballAbsX, ballAbsY);
            fromLineAndBall.set_y(CENTER_FIELD_Y - ballAbsY);

            // Add injection and return
            ReconstructedLocation reconstructed(fromLineAndBall.x(), fromLineAndBall.y(), fromLineAndBall.h(), 2, 2, 0.01);
            if (fromLineAndBall.x() > CENTER_FIELD_X)
                std::cout << "Major bug in ball in set injections!" << std::endl;
            else if (reconstructed.onField())
                injections.push_back(reconstructed);
        }
    // China 2015 hack
    // Don't inject off of any features but ball when in set
    } else if (ball == NULL) {
        // (2) Reconstruct pose from top goalbox
        // TODO reconstructing location from a line and a landmark is general
        //      functionality that LineSystem and/or LandmarkSystem should
        //      support, refactor after competition
        for (int i = 0; i < vision.line_size(); i++) {
            const messages::FieldLine& field = vision.line(i);

            // If found top goalbox, inject based on convex corner
            if (field.id() == static_cast<int>(vision::LineID::TopGoalbox)) {
                const messages::HoughLine& inner = field.inner();
                LocLineID id = (lastEstimate.x() > CENTER_FIELD_X ? LocLineID::TheirTopGoalbox : LocLineID::OurTopGoalbox);

                // Rotate line to loc rel robot coordinate system 
                vision::GeoLine line;
                line.set(inner.r(), inner.t(), inner.ep0(), inner.ep1());

                for (int j = 0; j < vision.corner_size(); j++) {
                    const messages::Corner& corner = vision.corner(j);

                    // Project corner onto line, find distance parallel to line from origin
                    double distParallel = line.qDist(corner.x(), corner.y());

                    // If found convex corner attached to top goalbox, inject particles
                    if (corner.id() == static_cast<int>(vision::CornerID::Convex) && 
                        (corner.line1() == field.index() || corner.line2() == field.index())) {
                        // Recover x and heading from top goalbox line
                        messages::RobotLocation pose = lineSystem->reconstructWoEndpoints(id, field);

                        // Recover y from corner
                        double cornerAbsX, cornerAbsY;
                        vision::translateRotate(corner.x(), corner.y(), 0, 0, pose.h(), cornerAbsX, cornerAbsY);

                        // Right or left convex goalbox corner
                        if (fabs(distParallel - inner.ep1()) > fabs(distParallel - inner.ep0())) {
                            if (id == LocLineID::OurTopGoalbox)
                                pose.set_y(BLUE_GOALBOX_BOTTOM_Y - cornerAbsY);
                            else
                                pose.set_y(YELLOW_GOALBOX_TOP_Y - cornerAbsY);
                        } else {
                            if (id == LocLineID::OurTopGoalbox)
                                pose.set_y(BLUE_GOALBOX_TOP_Y - cornerAbsY);
                            else
                                pose.set_y(YELLOW_GOALBOX_BOTTOM_Y - cornerAbsY);
                        }

                        // Inject if reconstucted location is on field
                        ReconstructedLocation reconstructed(pose.x(), pose.y(), pose.h(), 2, 2, 0.01);
                        if (reconstructed.onField())
                            injections.push_back(reconstructed);
                    }
                }

                // Based on midpoint of top goalbox
                // NOTE only valid if line is sufficiently long, otherwise too much
                //      error in the y direction
                if (inner.ep1() - inner.ep0() > 200) {
                    messages::RobotLocation pose = lineSystem->reconstructFromMidpoint(id, field);
                    ReconstructedLocation reconstructed(pose.x(), pose.y(), pose.h(), 2, 3, 0.01);
                    injections.push_back(reconstructed);
                }
            }
        }

        // (3) Reconstruct pose from endline
        // for (int i = 0; i < vision.line_size(); i++) {
        //     const messages::FieldLine& field = vision.line(i);

        //     // If found endline, inject based on T corner
        //     if (field.id() == static_cast<int>(vision::LineID::Endline)) {
        //         const messages::HoughLine& inner = field.inner();
        //         LocLineID id = (lastEstimate.x() > CENTER_FIELD_X ? LocLineID::TheirEndline : LocLineID::OurEndline);

        //         // Rotate line to loc rel robot coordinate system 
        //         vision::GeoLine line;
        //         line.set(inner.r(), inner.t(), inner.ep0(), inner.ep1());

        //         for (int j = 0; j < vision.corner_size(); j++) {
        //             const messages::Corner& corner = vision.corner(j);

        //             // Project corner onto line, find distance parallel to line from origin
        //             double distParallel = line.qDist(corner.x(), corner.y());

        //             // If found T corner attached to endline, inject particles
        //             if (corner.id() == static_cast<int>(vision::CornerID::T) && 
        //                 (corner.line1() == field.index() || corner.line2() == field.index())) {
        //                 // Recover x and heading from endline
        //                 messages::RobotLocation pose = lineSystem->reconstructWoEndpoints(id, field);

        //                 // Recover y from corner
        //                 double cornerAbsX, cornerAbsY;
        //                 vision::translateRotate(corner.x(), corner.y(), 0, 0, pose.h(), cornerAbsX, cornerAbsY);

        //                 // Right or left T corner
        //                 if (fabs(distParallel - inner.ep1()) > fabs(distParallel - inner.ep0())) {
        //                     if (id == LocLineID::OurEndline)
        //                         pose.set_y(BLUE_GOALBOX_BOTTOM_Y - cornerAbsY);
        //                     else
        //                         pose.set_y(YELLOW_GOALBOX_TOP_Y - cornerAbsY);
        //                 } else {
        //                     if (id == LocLineID::OurEndline)
        //                         pose.set_y(BLUE_GOALBOX_TOP_Y - cornerAbsY);
        //                     else
        //                         pose.set_y(YELLOW_GOALBOX_BOTTOM_Y - cornerAbsY);
        //                 }

        //                 // Inject if reconstucted location is on field
        //                 ReconstructedLocation reconstructed(pose.x(), pose.y(), pose.h(), 2, 2, 0.01);
        //                 if (reconstructed.onField())
        //                     injections.push_back(reconstructed);
        //             }
        //         }
        //     }
        // }

        // (4) Reconstruct pose from center circle and midline
        // if (vision.circle().on()) {
        //     bool midlineFound = false;
        //     messages::FieldLine midline;

        //     // Find line that loc system classified as midline
        //     for (int i = 0; i < vision.line_size(); i++) {
        //         const messages::FieldLine& field = vision.line(i);

        //         // Check for midline
        //         if (field.id() == static_cast<int>(vision::LineID::Midline)) {
        //             midlineFound = true;
        //             midline = field;
        //         }
        //     }

        //     // If found the midline, reconstruct location
        //     if (midlineFound) {
        //         // Get appropriate line id
        //         LocLineID id = (lastEstimate.x() > CENTER_FIELD_X ? LocLineID::TheirMidline : LocLineID::OurMidline);

        //         // Recontruct x and h from midline and y from center circle
        //         messages::RobotLocation fromLine = lineSystem->reconstructWoEndpoints(id, midline);
        //         messages::RobotLocation fromLineAndCircle = fromLine;

        //         // Rotate to absolute coordinate system
        //         double circleAbsX, circleAbsY;
        //         vision::translateRotate(vision.circle().x(), vision.circle().y(), 0, 0, fromLine.h(), circleAbsX, circleAbsY);
        //         fromLineAndCircle.set_y(CENTER_FIELD_Y - circleAbsY);

        //         // Add injection and return
        //         ReconstructedLocation reconstructed(fromLineAndCircle.x(), fromLineAndCircle.y(), fromLineAndCircle.h(), 1, 1, 0.01);
        //         if (reconstructed.onField())
        //             injections.push_back(reconstructed);
        //     }
        // }
    }

    PROF_EXIT(P_LOCV_INJECTION)

    // Weights were adjusted so return true
    return true;
}

} // namespace localization
} // namespace man
