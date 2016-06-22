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

#include "BayesianRobotValidator.hpp"

#include <cmath>
#include <fstream>
#include <string>
#include <sstream>

/**
 * Splits a string with a certain deliminator into a vector of fields.
 *
 * @param s string to split
 * @param delim deliminator of line
 *
 * @return vector of strings representing the fields.
 */
static std::vector<std::string> splitString(std::string s, char delim) {
	std::vector<std::string> elems;
	std::istringstream ss(s);
	std::string item;
	while (std::getline(ss, item, delim)) {
		elems.push_back(item);
	}
	return elems;
}


void BayesianRobotValidator::readMachineLearningData() {
	totalTestCases = 0;

	std::ifstream dataFile;
	dataFile.open(ROBOT_BAYESIAN_FILE_LOCATION, std::ifstream::in);
	if (!dataFile.is_open()) {
		dataFile.open(OFFNAO_BAYESIAN_FILE_LOCATION, std::ifstream::in);
		if (!dataFile.is_open()) {
			std::cout << "Could not open machine learning file" << std::endl;
			return;
		}
	}

	std::string line;
	while (dataFile >> line) {
		if (line[0] == '#') {
			continue;
		}

		//split into each part
		std::vector<std::string> elements = splitString(line, ',');
		double gradient = atof(elements[0].c_str());
		double percentageWhite = atof(elements[1].c_str());
		double percentageRed = atof(elements[2].c_str());
		double percentageBlue = atof(elements[3].c_str());
		double sonarDifference = atof(elements[4].c_str());
		char robotType = elements[5][0];
		bool isRobot = (robotType != 'f');

		if (isRobot) {
			trueData.addTrainingExample(gradient, percentageWhite, percentageRed, percentageBlue, sonarDifference);
		} else {
			falseData.addTrainingExample(gradient, percentageWhite, percentageRed, percentageBlue, sonarDifference);
		}
		++totalTestCases;
	}
	dataFile.close();

	priorTrueRobot = (double)trueData.count / (double)totalTestCases;
	priorFalseRobot = (double)falseData.count / (double)totalTestCases;

}


BayesianRobotValidator::BayesianRobotValidator() {
	readMachineLearningData();
}


std::pair<double, double> BayesianRobotValidator::calculateWhitePercentage(VisionFrame &frame, const Fovea &saliency, const PossibleRobot &robot) const {
	const double percentageWhite = robot.getPercentageOfColour(saliency, cWHITE);

	const double percentageTrue = trueData.getProbabilityForExample(TrainingSet::WHITE, percentageWhite, LAPLACE_ESTIMATE);
	const double percentageFalse = falseData.getProbabilityForExample(TrainingSet::WHITE, percentageWhite, LAPLACE_ESTIMATE);

	return std::pair<double, double>(percentageTrue, percentageFalse);
}

std::pair<double, double> BayesianRobotValidator::calculateRedPercentage(VisionFrame &frame, const Fovea &saliency, const PossibleRobot &robot) const {
	const double percentageRed = robot.getPercentageOfColour(saliency, cROBOT_RED);

	const double percentageTrue = trueData.getProbabilityForExample(TrainingSet::RED, percentageRed, LAPLACE_ESTIMATE);
	const double percentageFalse = falseData.getProbabilityForExample(TrainingSet::RED, percentageRed, LAPLACE_ESTIMATE);

	return std::pair<double, double>(percentageTrue, percentageFalse);
}

std::pair<double, double> BayesianRobotValidator::calculateBluePercentage(VisionFrame &frame, const Fovea &saliency, const PossibleRobot &robot) const {
	const double percentageBlue = robot.getPercentageOfColour(saliency, cROBOT_BLUE);

	const double percentageTrue = trueData.getProbabilityForExample(TrainingSet::BLUE, percentageBlue, LAPLACE_ESTIMATE);
	const double percentageFalse = falseData.getProbabilityForExample(TrainingSet::BLUE, percentageBlue, LAPLACE_ESTIMATE);

	return std::pair<double, double>(percentageTrue, percentageFalse);
}

std::vector<PossibleRobot> BayesianRobotValidator::complete(VisionFrame &frame,
		const Fovea &saliency, std::vector<PossibleRobot> obstructions) const {
	std::vector<PossibleRobot> robots;

	std::vector<PossibleRobot>::const_iterator it;
	for (it = obstructions.begin(); it != obstructions.end(); ++it) {
		const PossibleRobot &r = (*it);


		double robot = log(priorTrueRobot);
		double notRobot = log(priorFalseRobot);

		std::pair<double, double> whiteValues = calculateWhitePercentage(frame, saliency, r);

		robot += log(whiteValues.first);
		notRobot += log(whiteValues.second);


		std::pair<double, double> gradientValues = calculateGradientPercentage(frame, saliency, r);

		robot += log(gradientValues.first);
		notRobot += log(gradientValues.second);

		std::pair<double, double> redValues = calculateRedPercentage(frame, saliency, r);

		robot += log(redValues.first);
		notRobot += log(redValues.second);


		std::pair<double, double> blueValues = calculateBluePercentage(frame, saliency, r);

		robot += log(blueValues.first);
		notRobot += log(blueValues.second);

		std::pair<double, double> sonarValues = calculateSonarPercentage(frame, saliency, r);

		robot += log(sonarValues.first);
		notRobot += log(sonarValues.second);

		/*printf("%s grad: %3d White: %3d red: %3d blue: %3d sonar: %3d yes: %6.2f no: %6.2f\n",
				(robot > notRobot) ? "true" : "fals",
				(int)(gradientValues.first * 100 - gradientValues.second * 100),
				(int)(whiteValues.first * 100 - whiteValues.second * 100),
				(int)(redValues.first * 100 - redValues.second * 100),
				(int)(blueValues.first * 100 - blueValues.second * 100),
				(int)(sonarValues.first * 100 - sonarValues.second * 100),
				robot, notRobot);*/
		if (robot > notRobot) {
			robots.push_back(r);
		}
	}


	return robots;
}

std::pair<double, double> BayesianRobotValidator::calculateSonarPercentage(
		VisionFrame &frame,
		const Fovea &saliency,
		const PossibleRobot &robot) const {

	double sonar = (robot.sonarDifference != -1) ? (double)robot.sonarDifference / (double)robot.feet.distance() : -1;

	const double percentageTrue = trueData.getProbabilityForExample(TrainingSet::SONAR, sonar, LAPLACE_ESTIMATE);
	const double percentageFalse = falseData.getProbabilityForExample(TrainingSet::SONAR, sonar, LAPLACE_ESTIMATE);


	return std::pair<double, double>(percentageTrue, percentageFalse);
}



std::pair<double, double> BayesianRobotValidator::calculateGradientPercentage(
		VisionFrame &frame,
		const Fovea &saliency,
		const PossibleRobot &robot) const {

	const double gradient = (double)robot.region.height() / (double)robot.region.width();

	const double percentageTrue = trueData.getProbabilityForExample(TrainingSet::GRADIENT, gradient, LAPLACE_ESTIMATE);
	const double percentageFalse = falseData.getProbabilityForExample(TrainingSet::GRADIENT, gradient, LAPLACE_ESTIMATE);

	return std::pair<double,double>(percentageTrue, percentageFalse);
}


/**
 * Prints out machine learning information, such as the gradient, %white,
 * %red, %blue and the sonar difference percentage.
 */
void BayesianRobotValidator::printMachineLearningData(
        VisionFrame &frame, const Fovea &saliency,
        const std::vector<PossibleRobot> &possibleRobots) {
    for (unsigned int i = 0; i < possibleRobots.size(); ++i) {
        const PossibleRobot &robot = possibleRobots[i];
        unsigned int white = 0;
        unsigned int robotRed = 0;
        unsigned int robotBlue = 0;
        unsigned int total = 0;
        for (int col = robot.region.a.x(); col < robot.region.b.x(); ++col) {
            for (int row = robot.region.a.y(); row < robot.region.b.y(); ++row) {
                Colour c = saliency.colour(col, row);
                if (c == cWHITE) {
                    ++white;
                } else if (c == cROBOT_RED) {
                    ++robotRed;
                } else if (c == cROBOT_BLUE) {
                    ++robotBlue;
                }

                ++total;
            }
        }
        double gradient = (double)robot.region.height() / robot.region.width();
        double whitePercentage = (double) white / (double) total;
        double redPercentage = (double) robotRed / (double) total;
        double bluePercentage = (double) robotBlue / (double) total;
        double sonarDifferencePercentage = (double) robot.sonarDifference / (double) robot.feet.distance();
        if (robot.sonarDifference == -1) {
            sonarDifferencePercentage = -1;
        }
        printf("%0.4f,%0.4f,%0.4f,%0.4f,%0.4f,\n", gradient, whitePercentage, redPercentage, bluePercentage, sonarDifferencePercentage);
    }
}


