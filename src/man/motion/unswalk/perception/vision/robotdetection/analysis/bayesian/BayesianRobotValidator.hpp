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

#include "perception/vision/robotdetection/analysis/RobotAnalysisStep.hpp"
#include "TrainingSet.hpp"

#define ROBOT_BAYESIAN_FILE_LOCATION "/home/nao/data/vision/robotdetection/bayesian.data"
#define OFFNAO_BAYESIAN_FILE_LOCATION (std::string(getenv("RUNSWIFT_CHECKOUT_DIR")) + "/image" + ROBOT_BAYESIAN_FILE_LOCATION).c_str()

/**
 * Uses a bayesian machine learning algorithm to analyse the possible robots
 * and remove any that it deems not to be a robot
 */
class BayesianRobotValidator : public RobotAnalysisStep {
public:
	virtual std::vector<PossibleRobot> complete(VisionFrame &frame,	const Fovea &saliency, std::vector<PossibleRobot> obstructions) const;

	BayesianRobotValidator();

private:
	TrainingSet trueData, falseData;

	unsigned int totalTestCases;
	double priorTrueRobot;
	double priorFalseRobot;

	const static unsigned int LAPLACE_ESTIMATE = 1;

	std::pair<double, double> calculateWhitePercentage(VisionFrame &frame,	const Fovea &saliency, const PossibleRobot &robot) const;
	std::pair<double, double> calculateRedPercentage(VisionFrame &frame,	const Fovea &saliency, const PossibleRobot &robot) const;
	std::pair<double, double> calculateBluePercentage(VisionFrame &frame,	const Fovea &saliency, const PossibleRobot &robot) const;
	std::pair<double, double> calculateGradientPercentage(VisionFrame &frame,	const Fovea &saliency, const PossibleRobot &robot) const;
	std::pair<double, double> calculateSonarPercentage(VisionFrame &frame,	const Fovea &saliency, const PossibleRobot &robot) const;

	void readMachineLearningData();
	static void printMachineLearningData(VisionFrame &frame, const Fovea &saliency,
	        const std::vector<PossibleRobot> &possibleRobots);

	unsigned int findGradientBucket(double gradient) const;
};
