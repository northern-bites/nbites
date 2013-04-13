// This file is part of Man, a robotic perception, locomotion, and
// team strategy application created by the Northern Bites RoboCup
// team of Bowdoin College in Brunswick, Maine, for the Aldebaran
// Nao robot.
//
// Man is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Man is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser Public License for more details.
//
// You should have received a copy of the GNU General Public License
// and the GNU Lesser Public License along with Man.  If not, see
// <http://www.gnu.org/licenses/>.


/*
 * Robots.cpp is where we do our robot recognition work in vision.
 */

#include <iostream>
#include "Robots.h"
#include <vector>

using namespace std;

namespace man {
namespace vision {

#ifdef OFFLINE
static const bool ROBOTSDEBUG = false;
#else
static const bool ROBOTSDEBUG = false;
#endif

Robots::Robots(Vision* vis, Threshold* thr, Field* fie, Context* con,
               unsigned char col)
	: vision(vis), thresh(thr), field(fie), context(con), color(col)
{
    const int MAX_ROBOT_RUNS = 400;
    blobs = new Blobs(MAX_ROBOT_RUNS);
    allocateColorRuns();
}

/* Initialize the data structure.
 * @param s     the slope corresponding to the robot's head tilt
 */
void Robots::init()
{
	for (int i = 0; i <IMAGE_WIDTH/widthScale; i++){
		for (int j = 0; j < IMAGE_HEIGHT/heightScale; j++){
			imageBoxes[i][j] = 0;
		}
	}
	blobs->init();
	numberOfRuns = 0;
}

/* Allocate the required amount of memory dependent on the primary color
 */
void Robots::allocateColorRuns()
{
	const int RUN_VALUES = 3;         // for x, y and h
	const int RUNS_PER_SCANLINE = 15;
	const int RUNS_PER_LINE = 5;

	int run_num = RUN_VALUES;
	// depending on the color we have more or fewer runs available
	run_num = IMAGE_WIDTH * RUNS_PER_SCANLINE;
	runsize = IMAGE_WIDTH * RUNS_PER_LINE;
	runs = (run*)malloc(sizeof(run) * run_num);
}


/* Robot recognition methods
 */

void Robots::preprocess() {
    // do some robot scanning stuff - we're going to analyze our runs and see if
	//they could be turned into viable robot runs
    int bigh = IMAGE_HEIGHT, firstn = -1, lastn = -1, bot = -1;
    // first do the Navy robots
    for (int i = 0; i < IMAGE_WIDTH - 1; i+= 1) {
		int colorRun = thresh->getRobotTop(i, color);
		// if we saw a robot run in this scanline then process it
        if (colorRun != -1) {
			// our goal is to find a swath of runs and essentially grab them all at once
			// in a sense we're blobbing here
            firstn = i;
            lastn = 0;
            bigh = colorRun;
            bot = thresh->getRobotBottom(i, color);
			// as long as we're connected to more runs, keep scooping them up
            while ((thresh->getRobotTop(i, color) != -1 ||
					thresh->getRobotTop(i+1, color) != -1)
				   && i < IMAGE_WIDTH - 3) {
                if (thresh->getRobotTop(i, color) < bigh &&
					thresh->getRobotTop(i, color) != -1) {
                    bigh = thresh->getRobotTop(i, color);
                }
                if (thresh->getRobotBottom(i, color) > bot) {
                    bot = thresh->getRobotBottom(i, color);
                }
                i+=1;
                lastn+=1;
            }
			// now feed them all into our run structure
            for (int k = firstn; k < firstn + lastn; k+= 1) {
                newRun(k, bigh, bot - bigh);
                // cout << "Runs " << k << " " << bigh << " " << (bot - bigh)
                //      << endl;
            }
            //cout << "Last " << lastn << " " << bigh << " " << bot << endl;
            //drawRect(firstn, bigh, lastn, bot - bigh, RED);
        }
    }
}


/* Try and recognize robots.  Basically we're doing blobbing here, but with lots
   of extra twists.  Mainly we're being extremely loose with the conditions
   under which we consider blobs to be "connected."  We're trying to take
   advantage of the properties of the robots - namely that they stand vertically
   normally.
   @param cross     The Field cross data structure - contains white blob info
 */


void Robots::robot(Cross* cross)
{
	if (numberOfRuns < 1) return;
    const int lastRunXInit = -30;
    const int resConst = 10;
    const int robotBlobMin = 10;

    int lastrunx = lastRunXInit, lastruny = 0, lastrunh = 0;

    // loop through all of the runs of this color
    for (int i = 0; i < numberOfRuns; i++) {
        if (runs[i].x < lastrunx + resConst) {
            for (int k = lastrunx; k < runs[i].x; k+= 1) {
                blobs->blobIt(k, lastruny, lastrunh, true);
            }
        }
		// now we can add the run normally
        blobs->blobIt(runs[i].x, runs[i].y, runs[i].h, true);
		// set the current as the last
        lastrunx = runs[i].x; lastruny = runs[i].y; lastrunh = runs[i].h;
    }
    mergeBigBlobs();
    // check each of the candidate blobs to see if it might reasonably be
    // called a piece of a robot
	int viable = 0;
    for (int i = 0; i < blobs->number(); i++) {
        if (!sanityChecks(blobs->get(i), cross)) {
            if (blobs->get(i).getRight() > 0) {
                if (debugRobots) {
                    vision->drawRect(blobs->get(i).getLeft(),
                                     blobs->get(i).getTop(),
                                     blobs->get(i).width(),
                                     blobs->get(i).height(),
                                     WHITE);
                }
            }
            blobs->init(i);
        } else {
            if (debugRobots) {
                vision->drawRect(blobs->get(i).getLeft(), blobs->get(i).getTop(),
                                 blobs->get(i).width(), blobs->get(i).height(),
                                 MAROON);
			}
            viable++;
        }
    }
    // if we have some viable robots then let everyone know
	if (viable > 0) {
		int robotnum;
		for (int i = 1; i < viable + 1 && i < 4; i++) {
			robotnum = blobs->getBiggest();
			updateRobots(i, robotnum);
		}
	}
}



/* Try and recognize robots.  Basically we're doing blobbing here, but with lots
   of extra twists.  Mainly we're being extremely loose with the conditions
   under which we consider blobs to be "connected."  We're trying to take
   advantage of the properties of the robots - namely that they stand vertically
   normally.
   @param cross     The Field cross data structure - contains white blob info
*/

//findRobots is a wrapper that calls findUniforms which in turn calls blobRobots
//to determine an estimate for the entire robot
void Robots::findRobots(Cross* cross){
	whiteBlobs = cross->getBlobs();
	if (Utility::isUndefined(color)) {
		findRobotParts();
	} else {
        findUniforms();
	}
	blobs->init();
}

//finds uniform blobs and deems them viable robot uniform blobs or not
void Robots::findUniforms(){
	//in the following loop, we go through the entire image, looking for marked pixels.
	//once we find a pixel, we blob it, and indicate that is has been blobbed.
	for (int i = 0; i < IMAGE_WIDTH/widthScale; i++) {
		for (int j = 0; j < IMAGE_HEIGHT/heightScale; j++) {
			if (imageBoxes[i][j] == 1) {
				imageBoxes[i][j] = 2;
				blobs->newBlobIt(i, j, true);
			}
		}
	}
	mergeBigBlobs();

	//sort to examine largest blobs first since they are most likely to be robots
	blobs->sort();

	// Perform sanity checks on each blob to see if each might be part of a robot
	int viable = 0;
	for (int i = 0; i < blobs->number(); i++) {
		if (!sanityChecks(i)) {
			if (blobs->get(i).getRight() > 0) {
				if (debugRobots) {
					/*vision->drawRect(blobs->get(i).getLeft(),
					  blobs->get(i).getTop(),
					  blobs->get(i).width(),
					  blobs->get(i).height(),
					  WHITE);*/
				}
			}
			blobs->init(i);
		} else {
			if (debugRobots) {
				vision->drawRect(blobs->get(i).getLeft(), blobs->get(i).getTop(),
								 blobs->get(i).width(), blobs->get(i).height(),
								 MAROON);
			}
			viable++;
			//creates an area of interest for a blob, and performs corrections on it
			blobs->set(i, correctBlob(createAreaOfInterest(blobs->get(i))));

			//add the full robot blob to unid's blobs so that unid has all known robots
			thresh->unid->blobs->add(blobs->get(i));

			//let visual robots know of the robot blob
			updateRobots(viable, i);
		}
	}
}

//attempts to find robots where no uniform is visible
//NOTE: currently dangerous in that it gives false positives. needs substantial work
void Robots::findRobotParts(){
	//merge overlapping white blobs into a single blob
	for (int i = 0; i < whiteBlobs->number(); i++){
		for (int j = 0; j < whiteBlobs->number(); j++){
			if (i == j) continue;
			if (whiteBlobs->blobsOverlap(i, j)){
				whiteBlobs->mergeBlobs(i, j);
			}
		}
	}
	//sort the white blobs in descending order by size
	whiteBlobs->sort();

	for (int i = 0; i < whiteBlobs->number(); i++){
		bool aligned = false;
		bool side = false;

		//true if the blob is at the edge of the screen. For now we are only looking
		//at these blobs, since looking at all blobs leads to too many false positives
		if (whiteBlobs->get(i).getLeft() <= 10 ||
			whiteBlobs->get(i).getRight() >= IMAGE_WIDTH-10 ||
			whiteBlobs->get(i).getTop() <= 10 ||
			whiteBlobs->get(i).getBottom() >= IMAGE_HEIGHT-10) {
			side = true;
		}
		if (side){
			//checks if the white blob is aligned with a robot of known team.
			//if it is, we have already indentified this blob
			for (int j = 0; j < blobs->number(); j++){
				if (whiteBlobs->get(i).isAligned(blobs->get(j))) {
					aligned = true;
					break;
				}
			}
		}

		//true if we have found a potential robot part
		if (!aligned && side) {
			Blob part = correctBlob(whiteBlobs->get(i));
			//for now just outlines the region we found. Once this method is functional,
			//it should also call updateRobots in some manner
			vision->drawRect(part.getLeft(),
							 part.getTop(),
							 part.width(),
							 part.height(),
							 WHITE);
		}
	}
}

/* Given a viable uniform blob, add all vertically
   alligned white blobs to create an "area of interest" for where to more
   rigorously determine where a robot is
   @param robotBlob     potential uniform blob
   @return              blob indicating an area of interest for robot detection
*/
Blob Robots::createAreaOfInterest(Blob robotBlob) {
	int left = robotBlob.getLeft();
	int right = robotBlob.getRight();
	int top = robotBlob.getTop();
	int bottom = robotBlob.getBottom();
	int height = robotBlob.height();
	int width = right-left;

	for (int i = 0; i < whiteBlobs->number(); i++){
		if (whiteBlobs->get(i).isAligned(robotBlob)) {
			robotBlob.merge(whiteBlobs->get(i));
		}
	}

	//places constraints the on the area of interest with knowledge of the relative
	//ratio of the dimensions of the robot to the dimensions of its uniform
    //NOTE: this ratio could be more accurately defined
	if (robotBlob.getLeft() < left-width) robotBlob.setLeft(left-width);
	if (robotBlob.getRight() > right+width) robotBlob.setRight(right+width);
	if (robotBlob.getTop() < top-5*height) robotBlob.setLeft(top-5*height);
	if (robotBlob.getBottom() > bottom+6*height) robotBlob.setBottom(bottom+6*height);

	return robotBlob;
}

//correct the area of interest to a reasonable estimate of the robot
Blob Robots::correctBlob(Blob area){

	int left = max(0, area.getLeft());
	int right = min(area.getRight(), IMAGE_WIDTH);
	int top = max(0, area.getTop());
	int bottom = min(area.getBottom(), IMAGE_HEIGHT);
	int width = right - left;
	int height = bottom - top;

	int nonRobot = 0;

	//uncomment for debugging of corrections. Drawn rect is blob before correction
	//vision->drawRect(left,top, width, height, WHITE);

	//correct left side
	for (int x = left; x < left + (width/2); x++) {
		for (int y = top; y < bottom; y++) {
			if (Utility::isGreen(thresh->getThresholded(y,x)) ||
				Utility::isOrange(thresh->getThresholded(y,x))) {
				nonRobot++;
			}
			if (nonRobot > .4*height) {
				area.setLeft(x+1);
				nonRobot = -1;
				break;
			}
		}
		if (nonRobot != -1) break;
		nonRobot = 0;
	}

	//correct right side
	for (int x = right; x > left + (width/2); x--) {
		for (int y = top; y < bottom; y++) {
			if (Utility::isGreen(thresh->getThresholded(y,x)) ||
				Utility::isOrange(thresh->getThresholded(y,x))) {
				nonRobot++;
			}
			if (nonRobot > .4*height) {
				area.setRight(x-1);
				nonRobot = -1;
				break;
			}
		}
		if (nonRobot != -1) break;
		nonRobot = 0;
	}

	//correct top
	for (int y = top; y < top + (height/2); y++) {
		for (int x = left; x < right; x++) {
			if (Utility::isGreen(thresh->getThresholded(y,x)) ||
				Utility::isOrange(thresh->getThresholded(y,x))) {
				nonRobot++;
			};
			if (nonRobot > .4*width) {
				area.setTop(y+1);
				nonRobot = -1;
				break;
			}
		}
		if (nonRobot != -1) break;
		nonRobot = 0;
	}

	//correct bottom
	for (int y = bottom; y > top + (height/2); y--) {
		for (int x = left; x < right; x++) {
			if (Utility::isGreen(thresh->getThresholded(y,x)) ||
				Utility::isOrange(thresh->getThresholded(y,x))) {
				nonRobot++;
			}
			if (nonRobot > .4*width) {
				area.setBottom(y-1);
				nonRobot = -1;
				break;
			}
		}
		if (nonRobot != -1) break;
		nonRobot = 0;
	}

	return area;
}

/* Robot sanity checks.  Takes a candidate blob and puts it through a
   bunch of tests to make sure it is ok.
   @param  candidate        the blob to check
   @return                  whether we judge it to be reasonable
 */

bool Robots::sanityChecks(Blob candidate, Cross* cross) {
    const int blobHeightMin = 8;
    int height = candidate.height();
	int bottom = candidate.getBottom();
    if (candidate.getRight() > 0) {
        // the bottom of the uniform shouldn't be above field horizon
        //if (bottom < field->horizonAt(candidate.getLeft())) {
		//  return false;
        //}
        // blobs must be big enough
        if (candidate.height() < blobHeightMin) {
            return false;
        }
        // uniforms should be wider than they are tall
        if (candidate.height() > candidate.width()) {
            return false;
        }
        // there ought to be some white below the uniform
        if (bottom < IMAGE_HEIGHT - 10 &&
			!cross->checkForRobotBlobs(candidate)) {
			if (debugRobots) {
				cout << "Bad robot from cross check" << endl;
			}
            return false;
        }
        // the last check was pretty general, let's improve
        if (candidate.getBottom() < IMAGE_HEIGHT - candidate.height() * 2
            && !whiteBelow(candidate)) {
			if (debugRobots) {
				cout << "Got rid for lack of white below" << endl;
			}
            return false;
        }
        if (candidate.getTop() > candidate.height() * 2
            && !whiteAbove(candidate)) {
			if (debugRobots) {
				cout << "Got rid for lack of white above" << endl;
			}
            return false;
        }
        // for some blobs we check even harder for white
        if (height < 2 * blobHeightMin && noWhite(candidate)) {
			if (debugRobots) {
				cout << "Got rid of small one for white" << endl;
			}
            return false;
        }

		if (color == NAVY_BIT && vision->pose->getHorizonY(0) < 0 &&
			notGreen(candidate)) {
			return false;
		}
        return true;
    }
    return false;
}


/* Robot sanity checks.  Takes a candidate blob and puts it through a
   bunch of tests to make sure it is ok.
   @param  index        index of the blob to check in our blobs data structure
   @return              whether we judge it to be reasonable
*/

bool Robots::sanityChecks(int index) {
	Blob candidate = blobs->get(index);
	//thresholds for smallest allowable blob
	const int blobHeightMin = 3;
	const int blobAreaMin = 15;

	int height = candidate.height();
	int bottom = candidate.getBottom();
	if (candidate.getRight() > 0) {

		//blobs must be large enough
		if (candidate.height()*candidate.width() < blobAreaMin) {
			if (debugRobots){
				cout << "Blob area was too small" << endl;
			}
			return false;
		}

		// uniforms should be wider than they are tall
		if (candidate.height() > candidate.width()) {
			if (debugRobots){
				cout << "Blob was taller than it was wide" << endl;
			}
			return false;
		}

		//a robot cannot be inside another robot
		for (int i = 0; i < blobs->number(); i++) {
			if (i == index) continue;
			if (blobs->blobsOverlap(index, i)) {
				blobs->mergeBlobs(index, i);
				if (debugRobots){
					cout << "Blob was inside other robot" << endl;
				}
				return false;
			}
		}

		// there ought to be some white below the uniform
		if (bottom < IMAGE_HEIGHT - 10 &&
			!checkWhiteAllignment(candidate)) {
			if (debugRobots) {
				cout << "Bad robot from white alignment check" << endl;
			}
			return false;
		}
		// the last check was pretty general, let's improve
		if (candidate.getBottom() < IMAGE_HEIGHT - candidate.height() * 2
			&& !whiteBelow(candidate)) {
			if (debugRobots) {
				cout << "Got rid for lack of white below" << endl;
			}
			return false;
		}
		if (candidate.getTop() > candidate.height() * 2
			&& !whiteAbove(candidate)) {
			if (debugRobots) {
				cout << "Got rid for lack of white above" << endl;
			}
			return false;
		}
		// for some blobs we check even harder for white
		if (height < 2 * blobHeightMin && noWhite(candidate)) {
			if (debugRobots) {
				cout << "Got rid of small one for white" << endl;
			}
			return false;
		}

		if (color == NAVY_BIT && vision->pose->getHorizonY(0) < 0 &&
			notGreen(candidate)) {
			if (debugRobots){
				cout << "Got rid for pose check" << endl;
			}
			return false;
		}
		return true;
	}
	return false;
}

/* When we are looking down, the shadowed carpet often has lots of Navy.
   Make sure we aren't just looking at carpet
*/
bool Robots::notGreen(Blob candidate) {
	int bottom = candidate.getBottom();
	int top = candidate.getTop();
	int left = candidate.getLeft();
	int right = candidate.getRight();
	int area = candidate.width() * candidate.height() / 5;
	int greens = 0;
	for (int i = left; i < right; i++) {
		for (int j = top; j < bottom; j++) {
			if (Utility::isGreen(thresh->getThresholded(j, i))) {
				greens++;
				if (greens > area) {
					return true;
				}
			}
		}
	}
	return false;
}

/* Make sure there is some white alligned with the candidate blob
 */
bool Robots::checkWhiteAllignment(Blob candidate) {
	for (int i = 0; i < whiteBlobs->number(); i++) {
		if (whiteBlobs->get(i).isAligned(candidate)) {
			return true;
		}
	}
	return false;
}

/* Since the white blob check catches a lot of extra stuff like lines,
   we need to check a bit more carefully.
*/
bool Robots::whiteBelow(Blob candidate) {
	int bottom = candidate.getBottom();
	int height = candidate.height();
	int scanline = bottom + height;
	for (int y = scanline; y < IMAGE_HEIGHT && y < scanline + height; y += 3) {
		int white = 0;
		int green = 0;
		for (int x = candidate.getLeft(); x < candidate.getRight(); x++) {
			if (Utility::isWhite(thresh->getThresholded(y, x))) {
				white++;
			} else if (Utility::isGreen(thresh->getThresholded(y, x))) {
				green++;
			}
		}
		if (green > candidate.width() / 2 && white == 0) {
			return false;
		}
		if (white > candidate.width() / 4) {
			return true;
		}
	}
	return false;
}

/* Since the white blob check catches a lot of extra stuff like lines,
   we need to check a bit more carefully.
*/
bool Robots::whiteAbove(Blob candidate) {
	int top = candidate.getTop();
	int height = candidate.height();
	int scanline = top - height;
	for (int y = scanline; y > 0 && y > scanline - height; y -= 3) {
		int white = 0;
		int green = 0;
		for (int x = candidate.getLeft(); x < candidate.getRight(); x++) {
			if (Utility::isWhite(thresh->getThresholded(y, x))) {
				white++;
			} else if (Utility::isGreen(thresh->getThresholded(y, x))) {
				green++;
			}
		}
		if (green > candidate.width() / 2 && white == 0) {
			return false;
		}
		if (white > candidate.width() / 4) {
			return true;
		}
	}
	return false;
}

/* Since our uniforms often are wrinkled and whatnot, sometimes the blobs
   are not continuous.  See if we can merge some.
*/
void Robots::mergeBigBlobs() {
	for (int i = 0; i < blobs->number() - 1; i++) {
		Blob merge = blobs->get(i);
		if (merge.height() > 10) {
			for (int j = i + 1; j < blobs->number(); j++) {
				// check if the blobs are relatively near
				if (blobs->get(j).height() > 10 && closeEnough(i, j)) {
                    checkMerge(i, j);
				} else {
				}
			}
		}
	}
}

/* Are two blobs close enough that they might actually be the same?
   Note: if the area between is bigger than one of the blobs then can it?
*/
bool Robots::closeEnough(int i, int j) {
	Blob a = blobs->get(i);
	Blob b = blobs->get(j);
	int width1 = a.width();
	int width2 = b.width();
	int height1 = a.height();
	int height2 = b.height();
	int bigWidth = max(width1, width2);
	int bigHeight = max(height1, height2);
	int dist1 = distance(a.getLeft(), a.getRight(), b.getLeft(), b.getRight());
	int dist2 = distance(a.getTop(), a.getBottom(), b.getTop(), b.getBottom());
	if (a.getRight() <= 0 || b.getRight() <= 0) {
		return false;
	}
	if (dist1 > 5 && dist2 > 5) {
		return false;
	}
	if (dist1 < bigHeight && dist2 < bigWidth) {
		return true;
	}
	return false;
}

/* We have two blobs that are reasonably close.  See if they should be
   merged.
*/
void Robots::checkMerge(int i, int j) {
	Blob a = blobs->get(i);
	Blob b = blobs->get(j);
	if (debugRobots) {
		cout << endl << endl << "Checking merge" << endl;
		printBlob(a);
		printBlob(b);
	}
	int dist1 = distance(a.getLeft(), a.getRight(), b.getLeft(), b.getRight());
	int dist2 = distance(a.getTop(), a.getBottom(), b.getTop(), b.getBottom());
	int green = 0;
	int col = 0;
	int miss = 0;
	// top to bottom defines the y region between the two
	int top = max(a.getTop(), b.getTop());
	int bottom = min(a.getBottom(), b.getBottom());
	int midx = (a.getLeft() + a.getRight()) / 2;
	// left to right defines the x region between the two
	int left = a.getRight();
	int right = b.getLeft();
	int midy = (a.getTop() + a.getBottom()) / 2;
	int ii;
	// check if they overlap in either dimension
	if (left > right) {
		// they overlap in x so we'll swap left and right
		int temp = left;
		left = right;
		right = temp;
	}
	if (top > bottom) {
		int temp = top;
		top = bottom;
		bottom = temp;
	}
	int width = right - left + 1;
	int height = bottom - top + 1;
	int area = max(10, width * height / 27);
	int stripe = max(width, height);
	for (int x = left; x < right; x+=3) {
		for (int y = top; y < bottom; y+=3) {
			if (debugRobots) {
				vision->drawPoint(x, y, MAROON);
			}
			if (Utility::colorsEqual(thresh->getThresholded(y, x), color)) {
				col++;
				if (col > area || col > stripe) {
					blobs->mergeBlobs(i, j);
					if (debugRobots) {
						cout << "Merge" << endl;
					}
					return;
				}
			} else if (Utility::isGreen(thresh->getThresholded(y, x))) {
				green++;
				if (green > 5) {
					return;
				}
			} else if (Utility::isWhite(thresh->getThresholded(y, x))) {
				miss++;
				if (miss > area / 9 || miss > stripe) {
					return;
				}
			}
		}
	}
	if (col > min(width, height) && green < 3) {
		blobs->mergeBlobs(i, j);
		if (debugRobots) {
			cout << "Merge" << endl;
		}
	}
}

/* Calculate the horizontal distance between two objects
 * (the end of one to the start of the other).
 * @param x1	left x of one object
 * @param x2	right x of the object
 * @param x3	left x of the other
 * @param x4	right x of the other
 * @return		the distance between the objects in the x dimension
 */
int Robots::distance(int x1, int x2, int x3, int x4) {
	if (x2 < x3) {
		return x3 - x2;
	}
	if (x1 > x4) {
		return x1 - x4;
	}
    return 0;
}

/* We have a swatch of color.  Let's make sure that there is some white
   around somewhere as befits our robots.
*/
bool Robots::noWhite(Blob b) {
    const int MINWHITE = 5;

    int left = b.getLeft(), right = b.getRight();
    int top = b.getTop(), bottom = b.getBottom();
    int width = b.width();
    int tops, bottoms;
    for (int i = 1; i < b.height(); i++) {
        tops = 0; bottoms = 0;
        for (int x = left; x <= right; x++) {
            if (top - i >= 0 && Utility::isWhite(thresh->getThresholded(top - i,x)))
                tops++;
            if (bottom + i < IMAGE_HEIGHT &&
                Utility::isWhite(thresh->getThresholded(bottom+i,x)))
                bottoms++;
            if (tops > width / 2 || tops == width) return false;
            if (bottoms > width / 2 || tops == width) return false;
        }
    }
    return true;
}


/*
  We have detected something big enough to be called a robot.  Set the appropriate
  field object.
  @param which    whether it is the biggest or the second biggest object
  @param index    the index of the blob in question
*/

void Robots::updateRobots(int which, int index)
{
	if (debugRobots) {
		cout << "Updating robot " << which << " " << color << endl;
	}
	//printBlob(blobs[index]);
	if (color == RED_BIT) {
		if (which == 1) {
			vision->red1->updateRobot(blobs->get(index));
		} else if (which == 2) {
			vision->red2->updateRobot(blobs->get(index));
		} else {
			vision->red3->updateRobot(blobs->get(index));
		}
	} else if (color == NAVY_BIT) {
		if (which == 1) {
			vision->navy1->updateRobot(blobs->get(index));
		} else if (which == 2) {
			vision->navy2->updateRobot(blobs->get(index));
		} else {
			vision->navy3->updateRobot(blobs->get(index));
		}
	}
}

/* Adds a new run to the basic data structure.

   runs structure contains:
   -x of start column
   -y of start column
   -height of run

   @param x     x value of run
   @param y     y value of top of run
   @param h     height of run
*/
void Robots::newRun(int x, int y, int h)
{
    const int RUN_VALUES = 3;    // x, y, and h of course
	const int SKIPS = 8;

    if (numberOfRuns < runsize) {
        int last = numberOfRuns - 1;
        // skip over noise --- jumps over two pixel noise currently.
        //HW--added CONSTANT for noise jumps.
        if (last > 0 && runs[last].x == x &&
            (runs[last].y - (y + h) <= SKIPS)) {
            runs[last].h += runs[last].y - y; // merge run lengths
            runs[last].y = y; // reset the new y val
            h = runs[last].h;
            numberOfRuns--; // don't count this merge as a new run
        } else {
            runs[numberOfRuns].x = x;
            runs[numberOfRuns].y = y;
            runs[numberOfRuns].h = h;
        }
        numberOfRuns++;
    }
}


void Robots::setImageBox(int i, int j, int value){
	imageBoxes[i][j] = value;
}

/* Print debugging information for a blob.
 * @param b    the blob
 */
void Robots::printBlob(Blob b) {
#if defined OFFLINE
	cout << "Outputting blob" << endl;
	cout << b.getLeftTopX() << " " << b.getLeftTopY() << " " << b.getRightTopX() << " "
		 << b.getRightTopY() << endl;
	cout << b.getLeftBottomX() << " " << b.getLeftBottomY() << " " << b.getRightBottomX()
		 << " " << b.getRightBottomY() << endl;
#endif
}

}
}
