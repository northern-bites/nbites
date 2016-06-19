/*
 * PossibleRobot.cpp
 *
 *  Created on: 20/03/2014
 *      Author: jashmore
 */

#include "perception/vision/robotdetection/types/PossibleRobot.hpp"

PossibleRobot::PossibleRobot() :
    region()
    ,feet()
    ,sonarDist(-1)
    ,sonarDifference(-1)
    ,type(UNKNOWN) {

}

PossibleRobot::PossibleRobot(const BBox &robotRegion, const RRCoord &feetCoord) :
	region(robotRegion)
	,feet(feetCoord)
	,sonarDist(-1)
	,sonarDifference(-1)
	,type(UNKNOWN){}

double PossibleRobot::getPercentageOfColour(const Fovea &saliency, const Colour &desiredColour) const {
	std::set<Colour> colours;
	colours.insert(desiredColour);
	return getPercentageOfColours(saliency, colours);

}

double PossibleRobot::getPercentageOfColours(const Fovea &saliency, const std::set<Colour> &colours) const {
    unsigned int colourTotal = 0;
    unsigned int total = 0;


    //TODO: figure out why we should ever have this outside the bounds.
    //Bandaid fix for competition. Gross Gross Gross, need to find cause of this.
    int rows = (saliency.top) ? TOP_SALIENCY_ROWS : BOT_SALIENCY_ROWS;
    int cols = (saliency.top) ? TOP_SALIENCY_COLS : BOT_SALIENCY_COLS;
    if (region.a.x() < 0 || region.a.x() > cols || region.b.x() < 0 || region.b.x() > cols) {
        return 0;
    }

    if (region.a.y() < 0 || region.a.y() > rows || region.b.y() < 0 || region.b.y() > rows) {
       return 0;
    }

    for (int col = region.a.x(); col < region.b.x(); ++col) {
        for (int row = region.a.y(); row < region.b.y(); ++row) {
            Colour pixelColour = saliency.colour(col, row);
            if (colours.count(pixelColour)) {
                ++colourTotal;
            }
            ++total;
        }
    }

    return (double)colourTotal / (double)total;
}

double PossibleRobot::getPercentageOfRobotColours(const Fovea &saliency) const {
    std::set<Colour> colours;
    colours.insert(cWHITE);
    colours.insert(cROBOT_BLUE);
    colours.insert(cROBOT_RED);
    return getPercentageOfColours(saliency, colours);
}
