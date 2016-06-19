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

#pragma once

#include <vector>
#include <map>
#include <utility>

#include "perception/vision/robotdetection/analysis/RobotAnalysisStep.hpp"
#include "perception/vision/robotdetection/detection/obstructiondetector/ObstructionDetector.hpp"
#include "perception/vision/robotdetection/detection/robotfeetdetector/RobotFeetDetector.hpp"
#include "perception/vision/robotdetection/detection/robotheightestimator/RobotHeightEstimator.hpp"
#include "perception/vision/robotdetection/detection/robotsonardetector/RobotSonarDetector.hpp"
#include "perception/vision/robotdetection/detection/bottomdetectionmerger/BottomDetectionMerger.hpp"
#include "perception/vision/robotdetection/analysis/sanitychecks/ShoulderExclusionSanityCheck.hpp"

#include "perception/vision/Fovea.hpp"
#include "perception/vision/VisionDefs.hpp"
#include "types/UNSWRobotInfo.hpp"
#include "perception/vision/robotdetection/types/PossibleRobot.hpp"
#include "perception/vision/robotdetection/types/Obstruction.hpp"


class RobotDetection {

	friend class VisionTab;

public:
	RobotDetection();

	~RobotDetection();

    void findRobots(VisionFrame &frame,	const Fovea &saliency);
    void findRobotsWithBot(VisionFrame &fram, const Fovea &topSaliency, const Fovea &botSaliency);

private:
	const static bool MACHINE_LEARNING_FLAG = false;

    std::vector<int> _topFieldEdges;
	std::vector<BBox> _topObstructions;
	std::vector<PossibleRobot> _topPossibleRobots;

    std::vector<int> _botFieldEdges;
	std::vector<BBox> _botObstructions;
	std::vector<PossibleRobot> _botPossibleRobots;

	std::vector<UNSWRobotInfo> _robots;
	std::vector<std::vector <int> > _sonar;

	ObstructionDetector obstructionDetector;
	RobotFeetDetector robotFeetDetector;
	RobotHeightEstimator robotHeightEstimator;
	RobotSonarDetector robotSonarDetector;
	BottomDetectionMerger bottomDetectionMerger;
	ShoulderExclusionSanityCheck shoulderExclusionSanityCheck;

	std::vector<RobotAnalysisStep *> analysisSteps;

	void findPossibleRobots(
	        VisionFrame &frame, const Fovea &saliency,
	        std::vector<int> &fieldEdges,
	        std::vector<BBox> &obstructions,
	        std::vector<PossibleRobot> &possibleRobots);

	std::vector<PossibleRobot> analyseRobots(VisionFrame &frame, const Fovea &saliency,
			const std::vector<PossibleRobot> &possibleRobots);

	void setDetectedRobots(
	        const Fovea &topSaliency,
            std::vector<PossibleRobot> &topPossibleRobots,
            const Fovea &botSaliency,
            std::vector<PossibleRobot> &botPossibleRobots);
};

