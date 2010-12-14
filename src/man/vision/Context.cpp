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
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the
// GNU Lesser Public License for more details.
//
// You should have received a copy of the GNU General Public License
// and the GNU Lesser Public License along with Man.  If not, see
// <http://www.gnu.org/licenses/>.


/*
 * Context - holds context information on field objects in order to
 * better recognize them and identify them.
 */

#include <iostream>
#include "Context.h"
#include "debug.h"
#include "FieldConstants.h"
#include "Utility.h"
#include <vector>
#include <list>
#include <boost/shared_ptr.hpp>

using namespace std;

Context::Context(Vision *vis, Threshold* thr, Field* fie)
    : vision(vis), thresh(thr), field(fie)
{
    // Initialize the array of VisualFieldObject which we use for distance
    // based identification of corners
    allFieldObjects[0] = vision->bgrp;
    allFieldObjects[1] = vision->bglp;
    allFieldObjects[2] = vision->ygrp;
    allFieldObjects[3] = vision->yglp;

    init();
}

void Context::init() {
    rightYellowPost = false;
    leftYellowPost = false;
    unknownYellowPost = false;
    yellowPost = false;
    rightBluePost = false;
    leftBluePost = false;
    unknownBluePost = false;
    bluePost = false;
    tCorner = 0;
    lCorner = 0;
    iCorner = 0;
    oCorner = 0;
    cross = false;
    unknownCross = false;
    yellowCross = false;
    blueCross = false;
    ball = false;
    seeGoalBoxLines = false;
    debugIdentifyCorners = true;
}

/**
 * Given a list of VisualCorners, attempts to assign ConcreteCorners (ideally
 * one, but sometimes multiple) that correspond with where the corner could
 * possibly be on the field.  For instance, if we have a T corner and it is
 * right next to the blue goal left post, then it is the blue goal right T.
 * Modifies the corners passed in by calling the setPossibleCorners method;
 * in certain cases the shape of a corner might be switched too (if an L
 * corner is determined to be a T instead, its shape is changed accordingly).
 */
void Context::identifyCorners(list <VisualCorner> &corners)
{

    if (debugIdentifyCorners)
        cout << "Beginning identifyCorners() with " << corners.size()
             << " corners" << endl;

	int numCorners = corners.size(), numTs = 0, numLs = 0;
	if (numCorners > 1) {

        int cornerNumber = 0;
		list <VisualCorner>::iterator i = corners.begin();
		for ( ; i != corners.end(); i++){
            cornerNumber++;
			if (i->getShape() == T) {
				numTs++;
			}
			if (i->getShape() == INNER_L || i->getShape() == OUTER_L) {
				numLs++;
			}
            // find out if the current corner is connected to any other
			bool wasConnected = false;
            list <VisualCorner>::iterator j = corners.begin();
            for (int k = 1; j != corners.end(); j++, k++) {
                if (k > cornerNumber) {
                    if (i->getLine1() == j->getLine1() ||
                        i->getLine1() == j->getLine2() ||
                        i->getLine2() == j->getLine1() ||
                        i->getLine2() == j->getLine2()) {
                        findCornerRelationship(*i, *j);
						wasConnected = true;
                    }
                }
            }
		}
	}

    vector <const VisualFieldObject*> visibleObjects = getVisibleFieldObjects();

	// We might later use uncertain objects, but they cause problems. e.g. if you see
	// one post as 2 posts (both the left and right), you get really bad things or
	// if one post is badly estimated.
	if (visibleObjects.empty())
		visibleObjects = getAllVisibleFieldObjects();

    // No explicit movement of iterator; will do it manually
    for (list <VisualCorner>::iterator i = corners.begin();i != corners.end();){
		// before we start, analyze the corner a bit more
		if (i->getSecondaryShape() == UNKNOWN && i->getShape() == T) {
			// really long TStems indicate that we have a center T
            cout << "Testing T " << i->getTStem()->getLength() << endl;
			// in this particular case sometimes we should be able to absolutely
			// identify the T - if the stem is pointing relatively left or right
			if (i->getTStem()->getLength() > 2 * GOALBOX_DEPTH) {
                cout << "Setting to center with length " << i->getTStem()->getLength() << endl;
				i->setSecondaryShape(SIDE_T);
			}
		}

        if (debugIdentifyCorners) {
            cout << endl << "Before identification: Corner: "
                 << endl << "\t" << *i << endl;
			cout << "Shape info: " << i->getSecondaryShape() << endl;
        }

        const list <const ConcreteCorner*> possibleClassifications =
            classifyCornerWithObjects(*i, visibleObjects);

        // Keep it completely abstract
		// NOTE: this is dumb - we need to be smarter.  E.g. at least
		// eliminate posts that are WAY off
		// Sometimes the distances fail because the object is occluded
		// so we need another method for this case
		// also, this may be a chance to detect a bad post
        cout << "Test " << endl;
        if (possibleClassifications.empty()) {
            i->setPossibleCorners(ConcreteCorner::getPossibleCorners(
                                      i->getShape(), i->getSecondaryShape()));
            if (debugIdentifyCorners) {
                cout << "No matches were found for this corner; going to keep "
                     << "it completely abstract." << endl;
                printPossibilities(i->getPossibleCorners());
            }
            ++i;
        }

        // It is unique, append to the front of the list
        // For localization we want the positively identified corners to come
        // first so  that  they can inform the localization system and help
        // identify abstract corners that might be in the frame
        else if (possibleClassifications.size() == 1) {
            if (debugIdentifyCorners) {
                cout << "Only one possibility; appending to the front of the "
                     << "list " << endl;
                printPossibilities(possibleClassifications);
            }

            VisualCorner copy = *i;
            copy.setPossibleCorners(possibleClassifications);
            // This has the effect of incrementing our iterator and deleting the
            // corner from our list.
            i = corners.erase(i);
            corners.push_front(copy);
        }
        // More than 1 possibility for the corner
        else {
			// if we have more corners then those may help us ID the corner
			if (numCorners > 1) {
				if 	(i->getShape() == T) {
					if (numTs > 1) {
						// for now we'll just toss these
						// @TODO: Theoretically we can classify these
						if (debugIdentifyCorners) {
							cout << "Two Ts found - for now we throw them both out" << endl;
						}
						corners.clear();
						return;
					}
				}
			}

            // If either of the lines forming the corner are cc lines, then
            // the corner must be a cc intersection
            if (i->getShape() == CIRCLE ||
                i->getLine1()->getCCLine() ||
                i->getLine2()->getCCLine()) {
                i->setPossibleCorners(ConcreteCorner::ccCorners());
                i->setShape(CIRCLE);
            } else {
                i->setPossibleCorners(possibleClassifications);
				i->identifyLinesInCorner();
            }
            if (debugIdentifyCorners) {
                printPossibilities(i->getPossibleCorners());
            }
            ++i;
        }
    }

    for (list <VisualCorner>::iterator i = corners.begin();
		 i != corners.end(); ++i){
		i->identifyFromLines();
		i->identifyLinesInCorner();
		if (debugIdentifyCorners) {
			printPossibilities(i->getPossibleCorners());
        }
	}

	// If our corners have no identity, set them to their shape possibilities
    for (list <VisualCorner>::iterator i = corners.begin();
		 i != corners.end(); ++i){
		if (i->getPossibleCorners().empty()) {
			i->setPossibleCorners(ConcreteCorner::getPossibleCorners(i->getShape(),
																	 i->getSecondaryShape()));
        }
	}
}

/** Given two corners that we know are connected, explor to find out the
    possible relationships.  This should help us reduce the number of
    possible corners each can be and ultimately help the identification
    process.
 */
void Context::findCornerRelationship(VisualCorner & first, VisualCorner & second) {
    boost::shared_ptr<VisualLine> common;
    if (first.getLine1() == second.getLine1()) {
        common = first.getLine1();
    } else if (first.getLine1() == second.getLine2()) {
        common = first.getLine1();
    } else {
        common = first.getLine2();
    }
    VisualCorner* t;
    VisualCorner* l1;
    if (first.getShape() == T) {
        t = &first;
        if (second.getShape() == OUTER_L ||
            second.getShape() == INNER_L) {
            l1 = &second;
        } else {
            // T to CIRCLE or T to T
            cout << "T to something weird" << endl;
            if (second.getShape() == T) {
                // T to T, one is almost certainly the center circle
                if (common == first.getTStem()) {
                    if (first.getTStem()->getLength() > GOALBOX_DEPTH * 2) {
                        first.setSecondaryShape(SIDE_T);
                        second.setShape(CIRCLE);
                    }
                } else {
                    if (second.getTStem()->getLength() > GOALBOX_DEPTH * 2) {
                        second.setSecondaryShape(SIDE_T);
                        first.setShape(CIRCLE);
                    }
                }
            }
			return;
        }
    } else if (second.getShape() == T) {
        t = &second;
        if (first.getShape() == OUTER_L ||
            first.getShape() == INNER_L) {
            l1 = &first;
        } else {
            cout << "T to something weird" << endl;
            return;
        }
    } else {
        cout << "Two non T corners with common length " << common->getLength() << endl;
        // it is likely that one of the corners is actually a T
        return;
    }
    if (t->getTStem() == common) {
		t->setSecondaryShape(GOAL_T);
		l1->setSecondaryShape(GOAL_L);
        // looks very good - ultimately we should check line length too
        cout << "T connect to an L, should be goal box " <<
             t->getTStem()->getLength() << endl;
        // can we determine which side?
        if (l1->getShape() == OUTER_L) {
            // look at the non-common line, figure out which direction it goes
            if (l1->getLine1() == common) {
                cout << "Got common " << endl;
            }
        }
    } else {
        cout << "T connect to an L should be goal line to corner " <<
            t->getTBar()->getLength() << endl;
		l1->setSecondaryShape(CORNER_L);
		// check length  -- it its REALLY long to corner then we know
		// its a sideline T
    }
}

/**
 * Compare the given VisualCorner with other visible objects
 *  to see if the distances fit any sort of ConcreteCorners.
 *  Sets the possiblilities of the VisualCorner.
 *
 *  @param corner Corner to compare with VisualFieldObjects
 *  @param visibleObjects Visible objects to compare corner dists to.
 *  @return List of all ConcreteCorners whose distances fit the given objects.
 */
const list<const ConcreteCorner*> Context::classifyCornerWithObjects(
    const VisualCorner &corner,
    const vector <const VisualFieldObject*> &visibleObjects) const
{

	// Get all the possible corners given the shape of the corner
	vector <const ConcreteCorner*> possibleCorners =
		ConcreteCorner::getPossibleCorners(corner.getShape(), corner.getSecondaryShape());

    if (debugIdentifyCorners) {
        cout << endl
             << "Beginning to get possible classifications for corner given "
             << visibleObjects.size() << " visible objects and "
             << possibleCorners.size() << " corner possibilities" << endl
             << endl;
    }

	// Get all the possible corners given the shape of the corner
	list<const ConcreteCorner*> possibleClassifications =
        compareObjsCorners(corner, possibleCorners, visibleObjects);

	// If we found nothing that time, try again with all the corners to
	// see if possibly the corner was misidentified (e.g. saw an L, but
	// actually a T)
	if (possibleClassifications.empty()){

		possibleCorners = ConcreteCorner::concreteCorners();
		possibleClassifications =
			compareObjsCorners(corner, possibleCorners, visibleObjects);
	}

    return possibleClassifications;
}

// Given a list of concrete corners that the visual corner could possibly be,
// weeds out the bad ones based on distances to visible objects and returns
// those that are still in the running.
list <const ConcreteCorner*> Context::compareObjsCorners(
	const VisualCorner& corner,
	const vector<const ConcreteCorner*>& possibleCorners,
	const vector<const VisualFieldObject*>& visibleObjects) const
{
	list<const ConcreteCorner*> possibleClassifications;

    // For each field object that we see, calculate its real distance to
    // each possible concrete corner and compare with the visual estimated
    // distance. If it fits, add it to the list of possibilities.
    vector<const ConcreteCorner*>::const_iterator j =
        possibleCorners.begin();

    // Note: changed the order of the loops 12/3/2010 so we can check
    // every object against the corner instead of just finding one good one
    for (; j != possibleCorners.end(); ++j) {
		bool isOk = true;
        for (vector <const VisualFieldObject*>::const_iterator k =
                 visibleObjects.begin(); k != visibleObjects.end() && isOk; ++k) {

            // outerls are never going to be corners of the field
            if (corner.getShape() == OUTER_L) {
                if ((*j)->getID() == BLUE_CORNER_TOP_L ||
                    (*j)->getID() == BLUE_CORNER_BOTTOM_L ||
                    (*j)->getID() == YELLOW_CORNER_TOP_L ||
                    (*j)->getID() == YELLOW_CORNER_BOTTOM_L) {
					isOk = false;
                    continue;
                }
            } else if (corner.getShape() == T) {
                const float estimatedDistance = getEstimatedDistance(&corner, *k);
                // if we have a T corner and a goal post, then we can determine which
                // one it is definitely - look at whether the Stem is going up or down
                // if it is down (normal case) just look at whether T is left or right
                // if it is up, then reverse the results because you are over the endline
                // Is stem pointing up or down?
                if ((*j)->getID() == CENTER_TOP_T ||
                    (*j)->getID() == CENTER_BOTTOM_T) {
                    // needs to be related to some field constant
                    if (estimatedDistance > 250.0) {
                        // we can tell which one by looking at the direction of the T
                        if (corner.doesTPointRight()) {
                            if ((*k)->getID() == BLUE_GOAL_LEFT_POST ||
                                (*k)->getID() == BLUE_GOAL_RIGHT_POST) {
                                if ((*j)->getID() == CENTER_BOTTOM_T) {
                                    continue;
                                } else {
                                    isOk = false;
                                    continue;
                                }
                            } else {
                                if ((*j)->getID() == CENTER_TOP_T) {
                                    continue;
                                } else {
                                    isOk = false;
                                    continue;
                                }
                            }
                        } else {
                            if ((*k)->getID() == BLUE_GOAL_LEFT_POST ||
                                (*k)->getID() == BLUE_GOAL_RIGHT_POST) {
                                if ((*j)->getID() == CENTER_TOP_T) {
                                    continue;
                                } else {
                                    isOk = false;
                                    continue;
                                }
                            } else {
                                if ((*j)->getID() == CENTER_BOTTOM_T) {
                                    continue;
                                } else {
                                    isOk = false;
                                    continue;
                                }
                            }
                        }
                    }
                }
                bool down = corner.doesTPointDown();
                bool right;
                if (corner.getX() > (*k)->getX()) {
                    right = true;
                } else {
                    right = false;
                }
                if (down) {
                    cout << "T points down" << endl;
                    if (right) {
                        if ((*j)->getID() == BLUE_GOAL_RIGHT_T ||
                            (*j)->getID() == YELLOW_GOAL_RIGHT_T) {
							isOk = false;
                            continue;
                        }
                    } else {
                        if ((*j)->getID() == BLUE_GOAL_LEFT_T ||
                            (*j)->getID() == YELLOW_GOAL_LEFT_T) {
							isOk = false;
                            continue;
                        }
                    }
                } else {
                    cout << "T points up we are off the field!" << endl;
                    if (!right) {
                        if ((*j)->getID() == BLUE_GOAL_RIGHT_T ||
                            (*j)->getID() == YELLOW_GOAL_RIGHT_T) {
							isOk = false;
                            continue;
                        }
                    } else {
                        if ((*j)->getID() == BLUE_GOAL_LEFT_T ||
                            (*j)->getID() == YELLOW_GOAL_LEFT_T) {
							isOk = false;
                            continue;
                        }
                    }
                }
                // need a constant related to field constants here
                if (estimatedDistance < 100.0f && getRealDistance(*j, *k, 0) < 100.0f) {
                    continue;
                }
            }

            const float estimatedDistance = getEstimatedDistance(&corner, *k);
            const float distanceToCorner = corner.getDistance();
			// The visual object might be abstract, so we should check
			// all of its possible objects to see if we're close enough to one
			// and add all the possibilities up.
			list<const ConcreteFieldObject*>::const_iterator i =
				(*k)->getPossibleFieldObjects()->begin();

			bool close = false;
			for (int p = 0; i != (*k)->getPossibleFieldObjects()->end() && !close; ++i, ++p) {

				if (arePointsCloseEnough(estimatedDistance, *j, *k,
										 distanceToCorner, p)) {
					close = true;
                }
            }
			// if the corner wasn't close enough to any possible object
			if (!close) {
				isOk = false;
			}
		}
		// if we made it through all possible field objects
		if (isOk) {
			possibleClassifications.push_back(*j);
			if (debugIdentifyCorners) {
				cout << "Corner is possibly a " << (*j)->toString() << endl;
			}
		}
	}
	return possibleClassifications;
}

/**
 * Compares the given estimated distance against the actual distance between
 * the given concrete corner and the field object's true field position. Returns
 * true if the estimated distance is acceptable.
 *
 * @param estimatedDistance Estimated distance between VisualCorner and VisualFieldObject
 * @param j Corner whose distance from the FieldObject is being measured
 * @param k FieldObject used for measuremnts
 * @param distToCorner Distance from robot to corner.
 */
const bool Context::arePointsCloseEnough(const float estimatedDistance,
											const ConcreteCorner* j,
											const VisualFieldObject* k,
											const float distToCorner, int n) const
{
	const float realDistance = getRealDistance(j, k, n);
	const float absoluteError = fabs(realDistance - estimatedDistance);

	const float relativeErrorReal = absoluteError / realDistance * 100.0f;

	// If we have already one good distance between this corner and a
	// field object, we only require that the next objects have a small
	// relative error.
	const float MAX_RELATIVE_ERROR = 70.0f;
	const float USE_RELATIVE_DISTANCE = 250.0f;
    const float MAX_ABSOLUTE_ERROR = 200.f;

    if ( relativeErrorReal < MAX_RELATIVE_ERROR &&
         k->getDistance() > USE_RELATIVE_DISTANCE &&
         distToCorner > USE_RELATIVE_DISTANCE &&
         absoluteError < MAX_ABSOLUTE_ERROR) {
		if (debugIdentifyCorners) {
			cout << "\tDistance between " << j->toString() << " and "
				 << k->toString() << " was fine! Relative error of "
                 << relativeErrorReal
				 << " and absolute error of "
				 << absoluteError
				 << " corner pos: (" << j->getFieldX() << ","
				 << j->getFieldY()
				 << " goal pos: (" << k->getFieldX() << ","
				 << k->getFieldY() << endl;
		}
		return true;
    } else if (absoluteError > getAllowedDistanceError(k) ||
        absoluteError > MAX_ABSOLUTE_ERROR) {
		if (debugIdentifyCorners) {
			cout << "\tDistance between " << j->toString() << " and "
				 << k->toString() << " too large." << endl
				 << "\tReal: " << realDistance
				 << "\tEstimated: " << estimatedDistance << endl
				 << "\tAbsolute error: " << absoluteError
				 << "\tRelative error: " << relativeErrorReal << "% , "
				 << relativeErrorReal << "%"
				 << endl;
		}
		return false;
    } else {
		if (debugIdentifyCorners) {
			cout << "\tDistance between " << j->toString() << " and "
				 << k->toString() << " was fine! Absolute error of "
				 << absoluteError
				 << " corner pos: (" << j->getFieldX() << ","
				 << j->getFieldY() << ")"
				 << " goal pos: (" << k->getFieldX() << ","
				 << k->getFieldY() << ")" << endl;
		}
		return true;
	}
}

// @TODO A real distance error calculation. For now, just uses 2 times
// the FieldObject's Distance SD
float Context::getAllowedDistanceError(const VisualFieldObject * obj) const
{
    return obj->getDistanceSD() * 2;
}

// Given two points on the screen, estimates the straight line distance
// between them, on the field
float Context::getEstimatedDistance(const point<int> &point1,
                                       const point<int> &point2) const
{

    const estimate point1Est = vision->pose->pixEstimate(point1.x, point1.y, 0);
    const estimate point2Est = vision->pose->pixEstimate(point2.x, point2.y, 0);

    const float point1Dist = point1Est.dist;
    const float point2Dist = point2Est.dist;
    // We cannot accurately calculate the distance because the pix estimate
    // is too bad
    if (point1Dist <= 0 || point2Dist <= 0) { return BAD_DISTANCE; }

    return Utility::getLength(point1Est.x, point1Est.y,
                              point2Est.x, point2Est.y);
}

// Uses vector subtraction in order to determine distance between corner and
// field object.
// If the field object is very close (<100 cm away) we can use pose to estimate
// bearing and distance to the field object rather than the distance and bearing
// provided by the VisualFieldObject obj itself.
float Context::getEstimatedDistance(const VisualCorner *c,
                                       const VisualFieldObject *obj) const
{
	const float objDist = obj->getDistance();
	const float objBearing = obj->getBearing();

    const float cornerDist = c->getDistance();
    // Convert degrees to radians for the sin/cos formulas
    const float cornerBearing = c->getBearing();

    const float dist = getEstimatedDistance(objDist, objBearing,
                                            cornerDist, cornerBearing);
    return dist;
}

float Context::getEstimatedDistance(const float dist1, const float bearing1,
                                 const float dist2, const float bearing2) const
{
    const float x1 = dist1 * sin(bearing1);
    const float y1 = dist1 * cos(bearing1);

    const float x2 = dist2 * sin(bearing2);
    const float y2 = dist2 * cos(bearing2);

    return Utility::getLength(x1, y1,
                              x2, y2);
}

float Context::getEstimatedDistance(const VisualCorner& corner,
                                       const point<int>& p) const
{
    const estimate pixEst = vision->pose->pixEstimate(p.x, p.y, 0);

    return getEstimatedDistance(pixEst.dist, pixEst.bearing,
                                corner.getDistance(), corner.getBearing());
}

// This method should only be called with a VisualFieldObject object that has a
// concrete location on the field (YGLP, YGRP, BGLP, BGRP, BY, and YB).
// Calculates the length of the straight line between the two objects on the
// field
float Context::getRealDistance(const ConcreteCorner *c,
                                  const VisualFieldObject *obj, int which) const
{
	if (which == 0) {
		return Utility::getLength(c->getFieldX(), c->getFieldY(),
								  obj->getFieldX(), obj->getFieldY());
	}
	return Utility::getLength(c->getFieldX(), c->getFieldY(),
								  obj->getFieldX2(), obj->getFieldY2());
}

void Context::printPossibilities(const list <const ConcreteCorner*> &_list) const
{
    cout << "Possibilities: " << endl;
    for (list<const ConcreteCorner*>::const_iterator i = _list.begin();
         i != _list.end(); ++i) {
        cout << (*i)->toString() << endl;
    }
}

// Creates a vector of all those field objects that are visible in the frame
// and have SURE certainty
vector <const VisualFieldObject*> Context::getVisibleFieldObjects() const
{
    vector <const VisualFieldObject*> visibleObjects;
    for (int i = 0; i < NUM_FIELD_OBJECTS_WITH_DIST_INFO; ++i) {
        if (allFieldObjects[i]->getDistance() > 0 &&
            // We don't want to identify corners based on posts that aren't sure,
            // for instance
            allFieldObjects[i]->getIDCertainty() == _SURE) {
            // With the Nao we need to make sure that the goal posts are near the
            // green of the field in order to use them for distance
            if (ConcreteFieldObject::isGoal(allFieldObjects[i]->getID()) &&
                !(goalSuitableForPixEstimate(allFieldObjects[i]))) {
                // We can't use it.
            }
            else {
                visibleObjects.push_back(allFieldObjects[i]);
            }
        }
    }
    return visibleObjects;
}

vector<const VisualFieldObject*> Context::getAllVisibleFieldObjects() const
{
    vector <const VisualFieldObject*> visibleObjects;
    for (int i = 0; i < NUM_FIELD_OBJECTS_WITH_DIST_INFO; ++i) {
        if (allFieldObjects[i]->getDistance() > 0){
                visibleObjects.push_back(allFieldObjects[i]);
        }
    }
    return visibleObjects;
}

// In some Nao frames, robots obscure part of the goal and the bottom is not
// visible.  We can only use pix estimates of goals whose bottoms are visible
const bool Context::goalSuitableForPixEstimate(const VisualFieldObject * goal) const
{
    // When we're the goalie, we don't expect there to be any robots blocking
    // the goal.  Furthermore, we're seeing it from a much different angle where
    // the green will not be at the bottom of the post
    const int MAX_PIXEL_DIFF = 10;
    int greenHorizon = vision->thresh->getVisionHorizon();
    int midBottomY = (goal->getLeftBottomY() + goal->getRightBottomY())/2;
    // higher values are lower in the image
    return greenHorizon - midBottomY < MAX_PIXEL_DIFF;
}
