#include "RobotSonarDetector.hpp"



/**
 * Tests whether the robot heading is within a sonar start and end heading.
 */
static bool withinSonar(float heading, float start, float end) {
    return (start < heading && heading < end);
}


/**
 * @prereq sonar.size() > 0
 *
 * Goes through each of the sonar information and returns the sonar distance
 * that is closest to the estimate visual distance.
 */
static int distanceForSonar(int distance, const std::vector<int> &sonarDistances) {
    //assert(sonarDistances.size() > 0);

    int closest = 0;
    int closestIndex = -1;
    for (unsigned int i = 0; i < sonarDistances.size(); ++i) {
        const int difference = abs(distance - sonarDistances[i]);

        if (closestIndex == -1 || difference < closest) {
            closest = difference;
            closestIndex = i;
        }
    }

    //Should have found the best sonar distance as one in the vector.
    //assert(0 <= closestIndex && closestIndex < sonarDistances.size());
    return sonarDistances[closestIndex];
}




/**
 * Looks at the sonar distances for the current data and updates the
 * bestSonarDistance if it has found sonar distance information closer to the
 * feet distance.
 */
static void determineSonarForSensor(const RRCoord &feet,
        const float &start, const float &end, const std::vector<int> &sonarData,
        int &bestSonarDistance) {

    if (withinSonar(feet.heading(), start, end) && sonarData.size() > 0) {
        const int sonarDistance = distanceForSonar(feet.distance(), sonarData);
        const int currentBestDifference = abs(bestSonarDistance - feet.distance());
        const int sonarDifference = abs(sonarDistance - feet.distance());

        if (bestSonarDistance == -1 ||
            currentBestDifference > sonarDifference) {
            bestSonarDistance = sonarDistance;
        }
    }
}



/**
 * Goes through the possible robots and checks if there is any sonar data. It will
 * use the sonar information which is the closest to the robots estimated distance.
 */
void RobotSonarDetector::applySonarData(
        std::vector<PossibleRobot> &possibleRobots,
        std::vector<std::vector <int> > sonar) const {

    //Only do it if there is sonar data.
    if (sonar.size() == Sonar::SIZE) {
        for (unsigned int i = 0; i < possibleRobots.size(); ++i) {
            PossibleRobot &robot = possibleRobots[i];
            const RRCoord &feet = robot.feet;
            int bestSonarDistance = -1;
            determineSonarForSensor(feet, Sonar::LEFT_START, Sonar::LEFT_END, sonar[Sonar::LEFT], bestSonarDistance);
            determineSonarForSensor(feet, Sonar::MIDDLE_START, Sonar::MIDDLE_END, sonar[Sonar::MIDDLE], bestSonarDistance);
            determineSonarForSensor(feet, Sonar::RIGHT_START, Sonar::RIGHT_END, sonar[Sonar::RIGHT], bestSonarDistance);

            int bestDistanceDifference = abs(feet.distance() - bestSonarDistance);

            robot.sonarDist = bestSonarDistance;
            robot.sonarDifference = bestDistanceDifference;
        }
    }
}
