#pragma once


#include "perception/vision/Fovea.hpp"
#include "perception/vision/VisionDefs.hpp"

#include <set>

class PossibleRobot {

public:
	enum Type {
		RED_TEAM,
		BLUE_TEAM,
		UNKNOWN
	};

	PossibleRobot();
	PossibleRobot(const BBox &robotRegion, const RRCoord &feetCoord);
	double getPercentageOfColour(const Fovea &saliency, const Colour &desiredColour) const;
	double getPercentageOfColours(const Fovea &saliency, const std::set<Colour> &colours) const;
	double getPercentageOfRobotColours(const Fovea &saliency) const;


	BBox region;
	RRCoord feet;
	int sonarDist;
	int sonarDifference;
	Type type;
};
