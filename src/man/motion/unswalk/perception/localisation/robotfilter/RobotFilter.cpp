/*
Copyright 2014 The University of New South Wales (UNSW).

This file is part of the 2014 team rUNSWift RoboCup entry. You may
redistribute it and/or modify it under the terms of the GNU General
Public License as published by the Free Software Foundation; either
version 2 of the License, or (at your option) any later version as
modified below. As the original licensors, we add the following
conditions to that license:

In paragraph 2.b), the phrase "distribute or publish" should be
interpreted to include entry into a competition, and hence the source
of any derived work entered into a competition must be made available
to all parties involved in that competition under the terms of this
license.

In addition, if the authors of a derived work publish any conference
proceedings, journal articles or other academic papers describing that
derived work, then appropriate academic citations to the original work
must be included in that publication.

This rUNSWift source is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
General Public License for more details.

You should have received a copy of the GNU General Public License along
with this source code; if not, write to the Free Software Foundation,
Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
*/

#include "RobotFilter.hpp"

#include <string>
#include <sstream>

const int NO_CLOSE_OBSERVATION = -1;

//Make my life easier for iterating over vectors
#define FOR_EACH(index, vector) for (unsigned int index = 0; index < vector.size(); ++index)

std::vector<RobotObstacle> RobotFilter::update(const RobotFilterUpdate &update) {

    //Only update visual robots if not incapacitated
    if (!update.isIncapacitated) {

        std::vector<GroupedRobots>::iterator it = groupedRobots.begin();

        while (it != groupedRobots.end()) {
            GroupedRobots &group = (*it);
            group.tick(update.odometryDiff, update.headYaw, update.robotPos);
            if (group.isEmpty()) {
                it = groupedRobots.erase(it);
            } else {
                ++it;
            }
        }


        //Greedy algorithm to determine what observation goes into what group.
        //Current assumptions: multiple observations cannot go into the same group,
        //even if they are close together. The closest observation gets merge
        //into the group so there is many cases where this is suboptimal but as
        //this is a NP Complete problem it is good enough.
        std::vector<bool> observationMerged(update.visualRobots.size(), false);
        FOR_EACH(groupIndex, groupedRobots) {
            GroupedRobots &group = groupedRobots[groupIndex];

            int smallestIndex = NO_CLOSE_OBSERVATION;
            double smallestDistance = 0;
            FOR_EACH(visualIndex, update.visualRobots) {

                if (!observationMerged[visualIndex]) {

                    const UNSWRobotInfo &visualRobot = update.visualRobots[visualIndex];
                    if (group.canMergeRobot(visualRobot)) {

                        double distance = group.distanceToRobot(visualRobot);
                        if (smallestIndex == -1 || smallestDistance > distance) {
                            smallestIndex = visualIndex;
                            smallestDistance = distance;
                        }
                    }
                }
            }

            if (smallestIndex != -1) {
                const UNSWRobotInfo &observation = update.visualRobots[smallestIndex];
                group.mergeRobot(observation);
                observationMerged[smallestIndex] = true;
            }
        }

        FOR_EACH(visualIndex, update.visualRobots) {
            if (!observationMerged[visualIndex]) {
                GroupedRobots group(update.visualRobots[visualIndex]);
                groupedRobots.push_back(group);
            }
        }
    }

    filteredRobots = generateRobotObstacles();

    return filteredRobots;
}

std::vector<RobotObstacle> RobotFilter::generateRobotObstacles() const {
    std::vector<RobotObstacle> obstacles;

    FOR_EACH(groupedIndex, groupedRobots) {
        const GroupedRobots &group = groupedRobots[groupedIndex];
        if (group.isOnField() && group.isImportantObstacle()) {
            std::string type;
            if (group.getType() == UNSWRobotInfo::rUnknown) {
                type = "unknown";
            } else if (group.getType() == UNSWRobotInfo::rBlue) {
                type = "blue";
            } else {
                type = "red";
            }
            obstacles.push_back(group.generateRobotObstacle());
        }
    }

    return obstacles;
}
