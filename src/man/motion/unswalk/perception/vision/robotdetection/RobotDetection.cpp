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

#include "perception/vision/robotdetection/RobotDetection.hpp"
#include "utils/basic_maths.hpp"

#include "perception/vision/robotdetection/analysis/bayesian/BayesianRobotValidator.hpp"
#include "perception/vision/robotdetection/analysis/robotwidener/RobotWidener.hpp"
#include "perception/vision/robotdetection/analysis/robotsidedetector/RobotSideDetector.hpp"
#include "perception/vision/robotdetection/analysis/robotmerger/RobotMerger.hpp"
#include "perception/vision/robotdetection/analysis/sanitychecks/RobotSanityChecker.hpp"
#include "perception/vision/robotdetection/analysis/sanitychecks/RobotSanityCheckerBottom.hpp"


RobotDetection::RobotDetection() {
	analysisSteps.push_back(new RobotWidener());
	analysisSteps.push_back(new BayesianRobotValidator());
	analysisSteps.push_back(new RobotSideDetector());
	analysisSteps.push_back(new RobotMerger());
	analysisSteps.push_back(new RobotSanityChecker());
}

RobotDetection::~RobotDetection() {
	for (unsigned int i = 0; i < analysisSteps.size(); ++i) {
		delete analysisSteps[i];
	}
}


/**
 * Uses both the top and bottom saliency to calculate the possible robots.
 */
void RobotDetection::findRobotsWithBot(VisionFrame &frame, const Fovea &topSaliency,
		const Fovea &botSaliency) {

	findPossibleRobots(frame, topSaliency, _topFieldEdges, _topObstructions, _topPossibleRobots);
    findPossibleRobots(frame, botSaliency, _botFieldEdges, _botObstructions, _botPossibleRobots);

    RobotMerger merger(false);
    _botPossibleRobots = merger.complete(frame, botSaliency, _botPossibleRobots);

	bottomDetectionMerger.complete(frame, topSaliency, botSaliency, _topPossibleRobots, _botPossibleRobots);
	RobotSanityCheckerBottom bottomSanity;

	_botPossibleRobots = bottomSanity.complete(frame, botSaliency, _botPossibleRobots);

	robotSonarDetector.applySonarData(_topPossibleRobots, _sonar);

	std::vector<PossibleRobot> analysedRobots = analyseRobots(frame, topSaliency, _topPossibleRobots);
    setDetectedRobots(topSaliency, analysedRobots, botSaliency, _botPossibleRobots);

    _robots = shoulderExclusionSanityCheck.complete(_robots);
}



/**
 * Finds possible robots for a top saliency image.
 */
void RobotDetection::findPossibleRobots(VisionFrame &frame, const Fovea &saliency,
    std::vector<int> &fieldEdges,
    std::vector<BBox> &obstructions,
    std::vector<PossibleRobot> &possibleRobots) {

    std::vector<Obstruction> fieldEdgeObscures
        = obstructionDetector.complete(frame, saliency);

    fieldEdges = obstructionDetector.fieldEdges;

    obstructions = robotFeetDetector.complete(frame, saliency, fieldEdgeObscures);
	possibleRobots = robotHeightEstimator.complete(frame, saliency, obstructions);
}

/**
 * Validates the given possible robots, assigning them to detected robots when
 * validated.
 */
std::vector<PossibleRobot> RobotDetection::analyseRobots(VisionFrame &frame, const Fovea &saliency,
		const std::vector<PossibleRobot> &possibleRobots) {
    if (MACHINE_LEARNING_FLAG) {
        BayesianRobotValidator::printMachineLearningData(frame, saliency, possibleRobots);
    }

	std::vector<PossibleRobot> validatedRobots = possibleRobots;
	for (unsigned int i = 0; i < analysisSteps.size(); ++i) {
		validatedRobots = analysisSteps[i]->complete(frame, saliency, validatedRobots);
	}

	return validatedRobots;
}

/**
 * Looks over all of the bottom detections and if there is a top robot that is
 * aligned with it saves the robot info has including both that bottom and top
 * image. Otherwise it saves each of the bot and top robot detections by
 * themself.
 */
void RobotDetection::setDetectedRobots(
        const Fovea &topSaliency,
        std::vector<PossibleRobot> &topPossibleRobots,
        const Fovea &botSaliency,
        std::vector<PossibleRobot> &botPossibleRobots) {
    _robots.clear();

    std::vector<bool> useTop(topPossibleRobots.size(), true);

    std::vector<PossibleRobot> newTopPossibleRobots;


    for (unsigned int botIndex = 0; botIndex < botPossibleRobots.size(); ++botIndex) {
        PossibleRobot &botRobot = botPossibleRobots[botIndex];

        int matchedTop = -1;

        for (unsigned int topIndex = 0; topIndex < topPossibleRobots.size(); ++topIndex) {
            PossibleRobot &topRobot = topPossibleRobots[topIndex];
            if (BottomDetectionMerger::horizontallyOverlap(topRobot, botRobot)) {
                //Assumes that there should only be one top encompassing it
                //because of BottomDetectionMerger
                assert(matchedTop == -1);
                matchedTop = topIndex;

                useTop[topIndex] = false;
            }
        }


        //TODO: Convert this to a seperate function.
        Point topLeft = botSaliency.mapFoveaToImage(botRobot.region.a);
        Point bottomRight = botSaliency.mapFoveaToImage(botRobot.region.b);
        BBox region(topLeft, bottomRight);

        UNSWRobotInfo robot(botRobot.feet, UNSWRobotInfo::rUnknown, region, UNSWRobotInfo::BOT_CAMERA);

        if (matchedTop != -1) {

            PossibleRobot &topRobot = topPossibleRobots[matchedTop];
            Point topLeft = topSaliency.mapFoveaToImage(topRobot.region.a);
            Point bottomRight = topSaliency.mapFoveaToImage(topRobot.region.b);
            BBox region(topLeft, bottomRight);

            robot.cameras = UNSWRobotInfo::BOTH_CAMERAS;
            robot.topImageCoords = region;

            UNSWRobotInfo::Type type;
            if (topRobot.type == PossibleRobot::RED_TEAM) {
                type = UNSWRobotInfo::rRed;
            } else if (topRobot.type == PossibleRobot::BLUE_TEAM) {
                type = UNSWRobotInfo::rBlue;
            } else {
                type = UNSWRobotInfo::rUnknown;
            }

            robot.type = type;
        }
        _robots.push_back(robot);
    }

    for (unsigned int topIndex = 0; topIndex < topPossibleRobots.size(); ++topIndex) {
        if (useTop[topIndex]) {
            const PossibleRobot &r = topPossibleRobots[topIndex];

           Point topLeft = topSaliency.mapFoveaToImage(r.region.a);
           Point bottomRight = topSaliency.mapFoveaToImage(r.region.b);
           BBox region(topLeft, bottomRight);

           UNSWRobotInfo::Type type;
           if (r.type == PossibleRobot::RED_TEAM) {
               type = UNSWRobotInfo::rRed;
           } else if (r.type == PossibleRobot::BLUE_TEAM) {
               type = UNSWRobotInfo::rBlue;
           } else {
               type = UNSWRobotInfo::rUnknown;
           }

           UNSWRobotInfo robot(r.feet, type, region, UNSWRobotInfo::TOP_CAMERA);

           _robots.push_back(robot);
        }
    }
}
