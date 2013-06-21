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
 * Context - holds context information on field objects  and corners
 * in order to better recognize them and identify them.
 *
 * Here's how we recognize corners.  We start with the fact that
 * goal posts have already been found and identified (if possible).
 * Our first goal is to find corners that have a common line.  If that
 * is the case it highly constrains what each corner might be.  Further,
 * if we also have a goal post in site it is normally enough to tell
 * exactly what the corners are.  Also, if we have two corners that
 * aren't connected it often provides enough information to positively
 * ID each of the corners.  Mainly what we use in identifying things
 * is a combination of knowledge about the field layout and geometry.
 */

#include <iostream>
#include "Context.h"
#include "FieldConstants.h"
#include "Utility.h"
#include <vector>
#include <list>
#include <boost/shared_ptr.hpp>

using namespace std;

namespace man {
namespace vision {

// Used in discriminating Ts that are part of goalbox or not
static const float GOALBOX_FUDGE = 1.5;

Context::Context(Vision *vis, Threshold* thr, Field* fie)
    : vision(vis), thresh(thr), field(fie)
{
    // Initialize the array of VisualFieldObject which we use for distance
    // based identification of corners
    allFieldObjects[0] = vision->bgrp;
    allFieldObjects[1] = vision->bglp;
    allFieldObjects[2] = vision->ygrp;
    allFieldObjects[3] = vision->yglp;
#ifdef OFFLINE
    debugIdentifyCorners = false;
	debugDangerousBall = false;
#endif

    init();
}

/* Initialize all of the ivars.
 */
void Context::init() {
    rightPost = false;
    leftPost = false;
    unknownPost = false;
    seePost = false;
    tCorner = 0;
    lCorner = 0;
    iCorner = 0;
    oCorner = 0;
    cCorner = 0;
    cross = false;
    unknownCross = false;
    ball = false;
    seeGoalBoxLines = false;
    seeCenterCircle = false;
    sameHalf = false;
    face = FACING_UNKNOWN;
    objectRightX = -1;
    objectDistance = 0.0f;
    //debugIdentifyCorners = true;
}

/**
 * Given a list of VisualCorners, attempts to assign ConcreteCorners (ideally
 * one, but sometimes multiple) that correspond with where the corner could
 * possibly be on the field.  For instance, if we have a T corner and it is
 * right next to a goal left post, then it is the goal right T.
 * Modifies the corners passed in by calling the setPossibleCorners method;
 * in certain cases the shape of a corner might be switched too (if an L
 * corner is determined to be a T instead, its shape is changed accordingly).
 *
 * Note: 1/11/2010 - This method is still pretty similar to what we used
 * in the past.  It should probably be udpated even more.  I have not gotten
 * rid of some things simply because I'm not sure why they were there.
 * Its an ongoing process.
 * Chown
 *
 * @param corners      The list of all corners found in fieldLines
 */
void Context::identifyCorners(list <VisualCorner> &corners)
{
    if (corners.size() == 0) {
        return;
    }
    // Figure out what goal posts can provide us with contextual cues
    vector <const VisualFieldObject*> visibleObjects = getVisibleFieldObjects();
    setFacing();
    setFieldHalf();
    if (debugIdentifyCorners) {
        printContext();
    }
    int numCorners = corners.size();
    // collect up some information on corners that are connected
    if (numCorners > 1) {
        checkForConnectedCorners(corners);
    } else {
        // if there is only one corner we can still often classify it
        list <VisualCorner>::iterator one = corners.begin();
        if (one->getShape() == INNER_L) {
            classifyInnerL(*one);
        } else if (one->getShape() == OUTER_L) {
            classifyOuterL(*one);
        } else if (one->getShape() == T) {
            classifyT(*one);
        }
    }

    // We might later use uncertain objects, but they cause problems. e.g. if you
    // see one post as 2 posts (both the left and right), you get really bad things
    // or if one post is badly estimated.
    if (visibleObjects.empty()) {
        visibleObjects = getAllVisibleFieldObjects();
    }
    checkForSidelineInformation(corners);

    // No explicit movement of iterator; will do it manually
    for (list <VisualCorner>::iterator i = corners.begin();i != corners.end();){
        if (debugIdentifyCorners) {
            cout << endl << "Before identification: Corner: "
                 << endl << "\t" << *i << endl;
            cout << "     Shape info: " << i->getSecondaryShape() << endl;
            cout << "     Orientation: " << i->getOrientation() << endl;
        }
        // if the corner isn't done yet, this is our last attempt
        const list <const ConcreteCorner*> possibleClassifications =
            classifyCornerWithObjects(*i, visibleObjects);

        // It is unique, append to the front of the list
        // For localization we want the positively identified corners to come
        // first so  that  they can inform the localization system and help
        // identify abstract corners that might be in the frame
        if (possibleClassifications.size() == 1) {
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
                if  (i->getShape() == T) {
                    if (tCorner > 1) {
                        // for now we'll just toss these
                        // @TODO: Theoretically we can classify these
                        if (debugIdentifyCorners) {
                            cout << "Two Ts found - for now we throw them both out"
                                 << endl;
                        }
                        corners.clear();
                        return;
                    }
                }
            }

            i->setPossibleCorners(possibleClassifications);
            ++i;
        }
    }
    for (list <VisualCorner>::iterator i = corners.begin();
         i != corners.end(); ++i){
        // it isn't clear that it makes sense to do this since we don't
        // use line in the EKF
        i->identifyLinesInCorner();
        if (debugIdentifyCorners) {
            printPossibilities(*i->getPossibilities());
        }
    }
}

/* Loop through the corners looking for corners that may be a sideline.  If
   we see one that may help with context, plus it narrows the possibilities
   for that corner.

   *param corners        the complete list of corners
 */
void Context::checkForSidelineInformation(list<VisualCorner> &corners) {
    for (list <VisualCorner>::iterator i = corners.begin();i != corners.end();
         ++i){
        // before we start, analyze the corner a bit more
        if (i->getSecondaryShape() == UNKNOWN && i->getShape() == T &&
            face == FACING_UNKNOWN) {
            // really long TStems indicate that we have a center T
            // in this particular case sometimes we should be able to absolutely
            // identify the T - if the stem is pointing relatively left or right
            if (realLineDistance(i->getTStem()) >
                GOALBOX_FUDGE * GOALBOX_DEPTH) {
                i->setSecondaryShape(SIDE_T);
                if (face == FACING_UNKNOWN) {
                    if (debugIdentifyCorners) {
                        cout << "Updating facing to sideline" << endl;
                    }
                    face == FACING_SIDELINE;
                }
            }
        }
    }
}

/* Iterate through all of the corners to see if any of them are directly
   connected.  If so, find out what sort of information we can glean from
   that connection (e.g. what sort of T we're looking at).  We can also
   find information know that two corners are unconnected.  So look at that
   as well.

   @param corners      the complete list of corners
 */
void Context::checkForConnectedCorners(list<VisualCorner> &corners) {
    int cornerNumber = 0;
    list <VisualCorner>::iterator i = corners.begin();
    for ( ; i != corners.end(); i++){
        cornerNumber++;
        // find out if the current corner is connected to any other
        list <VisualCorner>::iterator j = corners.begin();
        for (int k = 1; j != corners.end(); j++, k++) {
            if (k > cornerNumber) {
                if (i->getLine1() == j->getLine1() ||
                    i->getLine1() == j->getLine2() ||
                    i->getLine2() == j->getLine1() ||
                    i->getLine2() == j->getLine2()) {
                    findCornerRelationship(*i, *j);
                } else {
                    findUnconnectedCornerRelationship(*i, *j);
                }
            }
        }
    }
}

/** If we have a single T corner we can often glean a lot of information
    about what it might be - especially if it is connected to a field object.
    The length of the stem may tell us that it is a sideline T for example.

    @param  first    A T corner (usually unconnected but not always)
 */
void Context::classifyT(VisualCorner & first) {
    float l1 = realLineDistance(first.getTStem());
    float l2 = realLineDistance(first.getTBar());
    int horizon = field->horizonAt(first.getX());
    float dist = realDistance(first.getX(), first.getY(),
                              first.getX(), horizon);
	float distToObject = realDistance(first.getX(), first.getY(),
									  objectRightX, objectRightY);
	bool objectIsClose = distToObject < GOALBOX_DEPTH * 2;
    if (debugIdentifyCorners) {
        cout << "Checking T " << l1 << " " << l2 << " " <<
            first.getDistance() << " " << dist << endl;
		cout << "Horizon " << horizon << " " << first.getY() << endl;
        if (objectRightX >= 0) {
            cout << "Object is at " << objectRightX << " " <<
                objectRightY << " " << objectDistance << endl;
			cout << "dist to object is " << distToObject << endl;
        }
    }

    // check if this is actually a center circle corner

	// if we see a T in the middle of nowhere far from the edge
	if (first.getDistance() > 100 && dist > 375 && first.getDistance() < 400 &&
		(first.getY() - horizon) > 25 &&
		!(face == FACING_GOAL && distToObject < 250)) {
		if (debugIdentifyCorners) {
			cout << "T is too far from horizon, must be a CC" << endl;
		}
		first.setShape(CIRCLE);
		return;
	}

    bool sideT = false;
    // we are generous normally on same half we can be more precise
    // for Ts
    if (objectDistance > MIDFIELD_X + 150.0 && distToObject > MIDFIELD_X - 150.0) {
        sameHalf = false;
    } else if (objectRightX > 0) {
		sameHalf = true;
	}

    // check if we are actually seeing a CC for far goals
    if (seeGoalBoxLines && objectDistance > 300 && first.getX() > IMAGE_WIDTH / 4
        && first.getX() < 3 * IMAGE_WIDTH / 4) {
        first.setShape(CIRCLE);
        return;
    }
	// check for side Ts first handle the really obvious case
	if (l1 > GOALBOX_DEPTH * 4) {
		sideT = true;
	} else if ((!sameHalf && face != FACING_UNKNOWN) ||
			   (l1 > 2.5 * GOALBOX_DEPTH &&
				(distToObject > MIDFIELD_X - 150 || objectRightX < 0))) {
        // if we are far away and the T stem is long and the T isn't near goal
        if (l1 > 2 * GOALBOX_DEPTH && (distToObject > MIDFIELD_X ||
									   face == FACING_UNKNOWN)) {
            sideT = true;
        } else {
            // if we are far away then if the T is near the goal
            // it should be pointing relatively straight at us
            int leftx = first.getTStem()->getLeftEndpoint().x;
            int rightx = first.getTStem()->getRightEndpoint().x;
            if (leftx < objectRightX && rightx > objectRightX) {
                sideT = true;
            } else if (rightx - leftx > IMAGE_WIDTH / 4) {
                sideT = true;
            } else if (first.getDistance() < FIELD_WHITE_WIDTH / 3) {
                sideT = true;
            }
        }
    }
    if (debugIdentifyCorners && sideT) {
        cout << "Side T is true" << endl;
    }
    if (sideT || (l1 > 3 * GOALBOX_DEPTH && objectDistance > 250) ||
        (face == FACING_UNKNOWN &&
         l1 > 2 * GOALBOX_DEPTH)) {
        first.setSecondaryShape(SIDE_T);
        if (face == FACING_UNKNOWN) {
            face = FACING_SIDELINE;
        }
    } else {
        if (face == FACING_GOAL) {
            // This could be made more robust
            if (first.doesItPointUp()) {
                // check if the T is above or below the post
                if (first.getY() > objectRightY) {
                    if (first.doesItPointLeft()) {
                        first.setSecondaryShape(LEFT_GOAL_T);
                    } else {
                        first.setSecondaryShape(RIGHT_GOAL_T);
                    }
                } else {
                    if (first.doesItPointLeft()) {
                        first.setSecondaryShape(RIGHT_GOAL_T);
                    } else {
                        first.setSecondaryShape(LEFT_GOAL_T);
                    }
                }
            } else if (first.getX() > objectRightX) {
                first.setSecondaryShape(LEFT_GOAL_T);
            } else {
                first.setSecondaryShape(RIGHT_GOAL_T);
            }
        } else if (l2 * 2 > FIELD_HEIGHT) {
            first.setSecondaryShape(SIDE_T);
            face = FACING_SIDELINE;
        }
    }
}

/** We have an OuterL which is not facing away from us.  Try and
    classify it.  Since its an OuterL we are going to assume that
    it isn't a field corner (note: that will get us in trouble when
    we are out of bounds).
    @param corner       the corner we are checking
 */
void Context::checkLowOuterL(VisualCorner & corner, bool line1IsLonger) {
    // determine which endpoint is higher up on screen
    int line1YMax = corner.getLine1()->getTopEndpoint().y;
    if (abs(corner.getLine1()->getTopEndpoint().y - corner.getY()) <
        abs(corner.getLine1()->getBottomEndpoint().y - corner.getY())) {
        line1YMax = corner.getLine1()->getBottomEndpoint().y;
    }
    int line2YMax = corner.getLine2()->getTopEndpoint().y;
    if (abs(corner.getLine2()->getTopEndpoint().y - corner.getY()) <
        abs(corner.getLine2()->getBottomEndpoint().y - corner.getY())) {
        line2YMax = corner.getLine2()->getBottomEndpoint().y;
    }
    bool line1IsUp = line1YMax < line2YMax;
    if (abs(line1YMax-line2YMax) < 5) {
        line1IsUp = corner.getLine1()->getAngle() >
            corner.getLine2()->getAngle();
    }
    bool topIsLonger = ((line1IsUp && line1IsLonger) ||
                        (!line1IsUp && !line1IsLonger));
    if (debugIdentifyCorners) {
        cout << "Single Outer L" << endl;
        cout << "angles " << corner.getLine1()->getAngle() <<
            " " << corner.getLine2()->getAngle() << endl;
        if (corner.doesItPointRight()) {
            cout << "Corner points to the right" << endl;
        } else {
            cout << "Corner points to the left" << endl;
        }
        if (topIsLonger) {
            cout << "Top is longer " << endl;
        } else {
            cout << "Bottom is longer " << endl;
        }
    }
    bool left;
    if (corner.doesItPointRight()) {
        if (topIsLonger) {
            corner.setSecondaryShape(LEFT_GOAL_L);
            left = true;
        } else {
            corner.setSecondaryShape(RIGHT_GOAL_L);
            left = false;
        }
    } else {
        if (topIsLonger) {
            corner.setSecondaryShape(RIGHT_GOAL_L);
            left = false;
        } else {
            corner.setSecondaryShape(LEFT_GOAL_L);
            left = true;
        }
    }
    if (face == FACING_GOAL) {
        if (left) {
            corner.setSecondaryShape(LEFT_GOAL_L);
        } else {
            corner.setSecondaryShape(RIGHT_GOAL_L);
        }
    }
}

/* Handles case where orientation is between 45 and 135 (absolute value)
   In this case we can't rely on the endpoints of the line to be outside
   the post.  See:  223-11/slarti/leftbluecorner/12.FRM
   Fortunately it isn't that hard.  We just determine if the shorter line
   is up or to the side, that tells us if we are looking from the side
   of the goal box or from the front.  From there it is easy to determine
   the corner.
 */
void Context::classifyOuterLMidAngle(VisualCorner & corner,
                                     boost::shared_ptr<VisualLine> shorty,
                                     boost::shared_ptr<VisualLine> longy) {
    const point<int> top1 = shorty->getTopEndpoint();
    const point<int> top2 = longy->getTopEndpoint();


	if (top1.y > top2.y) {
        // we're at the side of the goal - just check direction
        if (corner.doesItPointRight()) {
            corner.setSecondaryShape(LEFT_GOAL_L);
			// while we're at it, make sure IDs are correct
			if (vision->yglp->getDistance() == 0 && vision->ygrp->getDistance() > 0) {
				if (debugIdentifyCorners) {
					cout << "Bad post ID had been right" << endl;
				}
				//postSwap(vision->yglp, vision->ygrp);
			}
        } else {
            corner.setSecondaryShape(RIGHT_GOAL_L);
			if (vision->ygrp->getDistance() == 0 && vision->yglp->getDistance() > 0) {
				if (debugIdentifyCorners) {
					cout << "Bad post ID had been left or unknown" << endl;
				}
				//postSwap(vision->ygrp, vision->yglp);
			}
        }
    } else {
        // we're staring towards the goal, again check direction
        if (corner.doesItPointRight()) {
            corner.setSecondaryShape(RIGHT_GOAL_L);
        } else {
            corner.setSecondaryShape(LEFT_GOAL_L);
        }
    }
}

/** If we have a single OUTER_L corner we can often glean a lot of information
    about what it might be - especially if it is connected to a field object.
    Our main goal here is to see if we can tell if it is a left or a right
    corner.

    @param first    An unconnected outerl corner
 */
void Context::classifyOuterL(VisualCorner & corner) {
    if (corner.doesItPointDown() && abs(corner.getOrientation()) > 135) {
        classifyInnerL(corner);
        return;
    }
    float l1 = realLineDistance(corner.getLine1());
    float l2 = realLineDistance(corner.getLine2());
	// check distance of t to object if any
	float distToObject = 1000.0f;
	if (objectRightX >= 0) {
		distToObject = realDistance(corner.getX(), corner.getY(),
					 objectRightX, objectRightY);
	}
    if (debugIdentifyCorners) {
        cout << "Lines " << l1 << " " << l2 << " " <<
			corner.getLine1()->getLeftEndpoint().x <<
			" " << corner.getLine1()->getLeftEndpoint().y << endl;
        if (objectDistance > 0) {
            cout << "Object is " << objectDistance << endl;
        }
		cout << "Distance to object " << distToObject << endl;
		cout << "Goalbox info: " << GOALBOX_FUDGE * GOALBOX_DEPTH << endl;
    }

    // watch out for a bad CC identify or bad T
    if (face != FACING_UNKNOWN && objectDistance > 300 &&
        distToObject > 500) {
        // might be a side T
        if (l1 > GOALBOX_FUDGE * GOALBOX_DEPTH &&
            l2 > GOALBOX_FUDGE * GOALBOX_DEPTH &&
            objectDistance > 300) {
            // our "L" is actually a T unfortunately it isn't set up right
            // To Do: use the chageTo method in VisualCorner to get the the
            // set up properly
            if (corner.doesItPointRight()) {
                if (corner.getLine1()->getRightEndpoint().x >
                    corner.getLine2()->getRightEndpoint().x) {
                    corner.changeToT(corner.getLine1());
                    corner.setSecondaryShape(SIDE_T);
                } else {
                    corner.changeToT(corner.getLine2());
                    corner.setSecondaryShape(SIDE_T);
                }
            } else {
                if (corner.getLine1()->getLeftEndpoint().x >
                    corner.getLine2()->getLeftEndpoint().x) {
                    corner.changeToT(corner.getLine2());
                    corner.setSecondaryShape(SIDE_T);
                } else {
                    corner.changeToT(corner.getLine1());
                    corner.setSecondaryShape(SIDE_T);
                }
            }
            return;
        }
        corner.setShape(CIRCLE);
        return;
    }

    // check if it is an obvious field corner
    if (corner.getY() < objectRightY - 20) {
        if (face == FACING_GOAL) {
            // 223-11/slarti/lookingin-bg/NBFRM.0
            if (corner.getX() < objectRightX && corner.doesItPointLeft()) {
                corner.setSecondaryShape(RIGHT_GOAL_CORNER);
                return;
            } else if (corner.getX() > objectRightX &&
                       corner.doesItPointRight()) {
                corner.setSecondaryShape(LEFT_GOAL_CORNER);
                return;
            }
        }
    }
    bool line1IsLonger = l1 > l2;
    bool pointsMostlyUp = abs(corner.getOrientation()) < 135.0;
    if (!pointsMostlyUp) {
        checkLowOuterL(corner, line1IsLonger);
        return;
    } // for now we let compareObjsOuterL handle the else

    const point<int> top = corner.getLine1()->getTopEndpoint();
    const point<int> top2 = corner.getLine2()->getTopEndpoint();

    // if we can definitively determine the correct short line
    if (l1 < GOALBOX_FUDGE * GOALBOX_DEPTH &&
        l2 > GOALBOX_FUDGE * GOALBOX_DEPTH) {
        if (abs(corner.getOrientation())  < 45) {
            // it is l1 so fine its high endpoint
            if (objectRightX > -1) {
                if (top.x > top2.x) {
                    if (face == FACING_GOAL) {
                        corner.setSecondaryShape(LEFT_GOAL_L);
						if (vision->yglp->getDistance() == 0 && vision->ygrp->getDistance() > 0) {
							if (debugIdentifyCorners) {
								cout << "Bad post ID needs setting" << endl;
							}
						}
                    }
                } else {
                    if (face == FACING_GOAL) {
                        corner.setSecondaryShape(RIGHT_GOAL_L);
						if (vision->ygrp->getDistance() == 0 && vision->yglp->getDistance() > 0) {
							if (debugIdentifyCorners) {
								cout << "Bad post ID needs setting" << endl;
							}
						}
                    }
                }
            }
        } else {
            classifyOuterLMidAngle(corner, corner.getLine1(),
                                       corner.getLine2());
        }
    } else if (l1 > GOALBOX_FUDGE * GOALBOX_DEPTH &&
               l2 < GOALBOX_FUDGE * GOALBOX_DEPTH) {
        if (abs(corner.getOrientation()) < 45) {
            // bug:  223-11/slarti/leftbluecorner/NBFRM.12
            // basically on the side of the goal this idea doesn't work
            if (objectRightX > -1) {
                if (top2.x > top.x) {
                    if (face == FACING_GOAL) {
                        corner.setSecondaryShape(LEFT_GOAL_L);
						if (vision->yglp->getDistance() == 0 && vision->ygrp->getDistance() > 0) {
                            if (debugIdentifyCorners) {
                                cout << "Bad post ID" << endl;
                            }
						}
                    }
                } else {
                    if (face == FACING_GOAL) {
                        corner.setSecondaryShape(RIGHT_GOAL_L);
						if (vision->ygrp->getDistance() == 0 && vision->yglp->getDistance() > 0) {
                            if (debugIdentifyCorners) {
                                cout << "Bad post ID" << endl;
                            }
						}
                    }
                }
            }
        } else {
            classifyOuterLMidAngle(corner, corner.getLine2(),
                                   corner.getLine1());
        }
    }
    // eventually we should be able to figure some stuff out anyway
    // but let's get the low-hanging fruit first
    if (l1 < GOALBOX_FUDGE * GOALBOX_DEPTH &&
        l2 < GOALBOX_FUDGE * GOALBOX_DEPTH) {
        return;
    } else if (l1 > GOALBOX_FUDGE * GOALBOX_DEPTH &&
               l2 > GOALBOX_FUDGE * GOALBOX_DEPTH &&
               objectDistance > 400 && distToObject > 300) {
        // our "L" is actually a T unfortunately it isn't set up right
        // To Do: use the chageTo method in VisualCorner to get the the
        // set up properly
        corner.setShape(T);
        corner.setSecondaryShape(SIDE_T);
        return;
    }
}

/** We have identified an innerl as a field corner.  Set it accordingly
 *  based on other information like whether we see goals or not.
 *  @param corner       The innerl corner we want to set
 */
void Context::setFieldCorner(VisualCorner & corner) {
    if (debugIdentifyCorners) {
        cout << "Inner L classified as Field Corner " <<
            objectRightX << endl;
    }
    corner.setSecondaryShape(CORNER_L);
    switch (face) {
    case FACING_GOAL:
        if (objectRightX > -1 && objectRightX < IMAGE_WIDTH) {
            if (corner.getX() < objectRightX) {
                corner.setSecondaryShape(RIGHT_GOAL_CORNER);
            } else {
                corner.setSecondaryShape(LEFT_GOAL_CORNER);
            }
        }
        break;
    default:
        // if we can see goal box lines we can narrow it further
        if (seeGoalBoxLines) {
            // find the goal box line
            float distant = min(corner.getLine1()->getDistance(),
                                corner.getLine2()->getDistance());
            const vector < boost::shared_ptr<VisualLine> > * lines =
                vision->fieldLines->getLines();
            for (vector < boost::shared_ptr<VisualLine> >::const_iterator i =
                     lines->begin();
                 i != lines->end(); ++i) {
                float lineDist = (*i)->getDistance();
                // if we see a closer line, but it is still close to corner
                if (lineDist < distant && distant - lineDist < 100.0f &&
                    distant - lineDist > 10) {
                    if ((*i)->getBottomEndpoint().x < IMAGE_WIDTH / 2) {
                        corner.setSecondaryShape(LEFT_GOAL_CORNER);
                    } else {
                        corner.setSecondaryShape(RIGHT_GOAL_CORNER);
                    }
                    return;
                }
                distant = max((*i)->getDistance(), distant);
            }
        }
        break;
    }
}

/** We have an innerl and a field object.  It still could be a
    field corner or a goal corner.  Look at the relationship
    of the corner to the object to determine which.
    @param corner     an innerl
    @param l1         the length of the corner's line1
    @param l2         the length of the corner's line2
    @param l1IsLeft   whether l1 is the leftmost line or not

 */
void Context::checkGoalCornerWithPost(VisualCorner & corner,
                                      int l1y, int l2y,
                                      bool l1IsLeft, float dist) {
    shape leftCorner = LEFT_GOAL_CORNER;
    shape rightCorner = RIGHT_GOAL_CORNER;
    if (debugIdentifyCorners) {
        cout << "In checkGoalCornerWithPost " << dist << endl;
    }
    // sometimes we see field corners far away
    if (corner.getDistance() > MIDFIELD_X || !sameHalf) {
        if (debugIdentifyCorners) {
            cout << "Changing to far corner" << endl;
        }
        setFieldCorner(corner);
        return;
    }
    float distant = 0;
    // check if this corner is at the edge
    const vector < boost::shared_ptr<VisualLine> > * lines =
        vision->fieldLines->getLines();
    for (vector < boost::shared_ptr<VisualLine> >::const_iterator i =
             lines->begin();
         i != lines->end(); ++i) {
        distant = max((*i)->getDistance(), distant);
    }
    bool isInner = false;
    if (distant > corner.getLine1()->getDistance() &&
        distant > corner.getLine2()->getDistance()) {
        isInner = true;
    }

    // "Right" in this case means in the visual frame
    bool cornerIsRight = false;
    if (objectRightX > -1) {
        if (corner.getX() > objectRightX) {
            cornerIsRight = true;
        }
    } else {
        if (debugIdentifyCorners) {
            cout << "Have an object, but not its location" << endl;
        }
        if (corner.doesItPointLeft()) {
            cornerIsRight = true;
        }
    }


    // sometime to be super-safe we should check where the line intersects
    // the goal post
    // best done by using the post
    if (isInner) {
        if (cornerIsRight) {
            corner.setSecondaryShape(LEFT_GOAL_L);
        } else {
            corner.setSecondaryShape(RIGHT_GOAL_L);
        }
    } else {
        if (cornerIsRight) {
            corner.setSecondaryShape(leftCorner);
        } else {
            corner.setSecondaryShape(rightCorner);
        }
    }
}

/** We have an innerl and no field objects.  On the bright side
    we have seen parallel lines which suggests that this is a
    corner of a goal box.
    @param corner     an innerl
    @param l1         the length of the corner's line1
    @param l2         the length of the corner's line2
    @param l1IsLonger whether line1 is longer than line2 or not
    @param l1IsLeft   whether l1 is the leftmost line or not
 */
void Context::checkUnknownGoalCorner(VisualCorner & corner,
                                     float l1, float l2,
                                     bool l1IsLeft) {
    bool l1IsLonger = l1 > l2;
    // we have one corner and no field objects
    corner.setSecondaryShape(GOAL_L);
    // now see if we can figure out exactly which L
    if (debugIdentifyCorners) {
        cout << "In checkunknown " << l1 << " " << l2 << endl;
        if (l1IsLeft) {
            cout << "L1 is left " << endl;
        } else {
            cout << "L2 is left " << endl;
        }
    }

    if (l1IsLonger) {
        if (l1 > GOALBOX_FUDGE * GOALBOX_DEPTH) {
            // we have enough information - is big line to left or right?
            if (l1IsLeft) {
                corner.setSecondaryShape(RIGHT_GOAL_L);
            } else {
                corner.setSecondaryShape(LEFT_GOAL_L);
            }
        }
    } else if (l2 > GOALBOX_FUDGE * GOALBOX_DEPTH) {
        if (l1IsLeft) {
            corner.setSecondaryShape(RIGHT_GOAL_L);
        } else {
            corner.setSecondaryShape(LEFT_GOAL_L);
        }
    } else {
        // The situation: one of the lines is parallel to the sideline
        // if we can figure out which one, then we know it is the
        // shorter line
        if (corner.getLine1()->isParallel() &&
            !corner.getLine2()->isParallel()) {
            // line 1 is parallel to the sideline, so we can infer corner
            if (l1IsLeft) {
                corner.setSecondaryShape(LEFT_GOAL_L);
            } else {
                corner.setSecondaryShape(RIGHT_GOAL_L);
            }
        } else if (!corner.getLine1()->isParallel() &&
                   corner.getLine2()->isParallel()) {
            if (!l1IsLeft) {
                corner.setSecondaryShape(LEFT_GOAL_L);
            } else {
                corner.setSecondaryShape(RIGHT_GOAL_L);
            }
        }
    }
}

/** This is called when we have an INNER_L and little to suggest
 *  that it is a goal corner (e.g. no long line, no goal box match).
 *  So we look for evidence that it is a field corner.  Among the
 *  the things we look at are the length of its lines - e.g. if
 *  they are both long it is strong evidence - and the distance
 *  to the sideline.
 *  @param corner     the innerl we are examining
 *  @param l1        the (real) length of its line1
 *  @param l2        the (real) length of its line2
 */
void Context::lookForFieldCorner(VisualCorner & corner, float l1, float l2) {
    // we'll want the distance to the field edge
    int horizon = field->horizonAt(corner.getX());
    float dist = realDistance(corner.getX(), corner.getY(),
                              corner.getX(), horizon);
    // we'll want to know the spatial layout of the two lines
    int l1x1 = corner.getLine1()->getLeftEndpoint().x;
    int l2x1 = corner.getLine2()->getLeftEndpoint().x;
    int l1y1, l2y1;
    // there are two ways the two lines can be connected
    bool l1IsLeft = l1x1 < l2x1;
    if (debugIdentifyCorners) {
        cout << "In lookForFieldCorner" << endl;
    }
    // if both lines are long then its a corner
    if (l1 > GOALBOX_DEPTH * GOALBOX_FUDGE &&
        l2 > GOALBOX_DEPTH * GOALBOX_FUDGE) {
        setFieldCorner(corner);
        return;
    } else if (l1 > 150 && dist < 2 * GREEN_PAD_X && face == FACING_UNKNOWN) {
        if (l1IsLeft) {
            corner.setSecondaryShape(RIGHT_GOAL_CORNER);
        } else {
            corner.setSecondaryShape(LEFT_GOAL_CORNER);
        }
    } else if (l2 > 150 && dist < 2 * GREEN_PAD_X && face == FACING_UNKNOWN) {
        if (l1IsLeft) {
            corner.setSecondaryShape(LEFT_GOAL_CORNER);
        } else {
            corner.setSecondaryShape(RIGHT_GOAL_CORNER);
        }
    } else if (dist < 2 * GREEN_PAD_X && field->horizonAt(corner.getX()) > 10) {
        // if we have a long line and it isn't far from edge -> field corner
        int otherX, otherY;
        if (l1 > GOALBOX_DEPTH * GOALBOX_FUDGE) {
            if (l1IsLeft) {
                otherX = corner.getLine1()->getLeftEndpoint().x;
                otherY = corner.getLine1()->getLeftEndpoint().y;
            } else {
                otherX = corner.getLine1()->getRightEndpoint().x;
                otherY = corner.getLine1()->getRightEndpoint().y;
            }
            int horizon2 = field->horizonAt(otherX);
            float dist2 = realDistance(otherX, otherY, otherX, horizon);
            if (dist2 < 2 * GREEN_PAD_X) {
                setFieldCorner(corner);
                return;
            }
        } else if (l2 > GOALBOX_DEPTH * GOALBOX_FUDGE) {
            if (!l1IsLeft) {
                otherX = corner.getLine2()->getLeftEndpoint().x;
                otherY = corner.getLine2()->getLeftEndpoint().y;
            } else {
                otherX = corner.getLine2()->getRightEndpoint().x;
                otherY = corner.getLine2()->getRightEndpoint().y;
            }
            int horizon2 = field->horizonAt(otherX);
            float dist2 = realDistance(otherX, otherY, otherX, horizon);
            if (dist2 < 2 * GREEN_PAD_X) {
                setFieldCorner(corner);
                return;
            }
        } else {
            // alternatively if all three points are close to edge
            int x1 = corner.getLine1()->getLeftEndpoint().x;
            int y1 = corner.getLine1()->getLeftEndpoint().y;
            int x2 = corner.getLine2()->getRightEndpoint().x;
            int y2 = corner.getLine2()->getRightEndpoint().y;
            if (!l1IsLeft) {
                corner.getLine1()->getRightEndpoint().x;
                corner.getLine1()->getRightEndpoint().y;
                corner.getLine2()->getLeftEndpoint().x;
                corner.getLine2()->getLeftEndpoint().y;
            }
            int h1 = field->horizonAt(x1);
            int h2 = field->horizonAt(x2);
            float d1 = realDistance(x1, y1, x1, h1);
            float d2 = realDistance(x2, y2, x2, h2);
            if (d1 < GREEN_PAD_X * 2 && d2 < GREEN_PAD_X * 2) {
                setFieldCorner(corner);
                return;
            }
        }
    }
	// the field edge is very useful in this case
	if (field->findSlant() < 0 && field->getPeak() < IMAGE_WIDTH / 3 &&
		thresh) {
		corner.setSecondaryShape(RIGHT_GOAL_L);
		if (debugIdentifyCorners) {
			cout << "Setting from field slant info" << endl;
		}
	} else if (field->findSlant() > 0 && field->getPeak() > 2 * IMAGE_WIDTH / 3) {
		corner.setSecondaryShape(LEFT_GOAL_L);
		if (debugIdentifyCorners) {
			cout << "Setting from field slant info" << endl;
		}
	} else if (debugIdentifyCorners) {
        cout << "Lone InnerL, Distance to horizon is " << dist << endl;
        cout << "      " << horizon << " " << corner.getY() << endl;
        cout << "Lengths: " << l1 << " " << l2 << endl;
    }
}


/** If we have a single INNER_L corner we can often glean a lot of information
    about what it might be - especially if it is connected to a field object.
    Things we can check:
          Can we see the goal box?
          Can we see a goal post?
          How long are the lines in the corner?
    @param corner     the innerl (not connected to another corner)
 */
void Context::classifyInnerL(VisualCorner & corner) {
    // we'll need the actual lengths of each line
    float l1 = realLineDistance(corner.getLine1());
    float l2 = realLineDistance(corner.getLine2());
    float cornerDist = corner.getDistance();

    // try and throw away bad CC corners - goal is far, corner is close
    if (face != FACING_UNKNOWN && objectDistance > MIDFIELD_X &&
                                   corner.getDistance() < 250) {
        corner.setShape(CIRCLE);
        return;
    }

    // again, another way of checking, corner far from object, closer to robot
    if (face != FACING_UNKNOWN &&
        realDistance(corner.getX(), corner.getY(), objectRightX, objectRightY) >
        150.0f && objectDistance > cornerDist) {
        corner.setShape(CIRCLE);
        return;
    }

    // punt (for now) when we can be sure about what we see
    if (!seeGoalBoxLines && face == FACING_UNKNOWN) {
        lookForFieldCorner(corner, l1, l2);
        return;
    }
    // we'll want the distance to the field edge
    int horizon = field->horizonAt(corner.getX());
    float dist = realDistance(corner.getX(), corner.getY(),
                              corner.getX(), horizon);
    // we'll want to know the spatial layout of the two lines
    int l1x1 = corner.getLine1()->getLeftEndpoint().x;
    int l2x1 = corner.getLine2()->getLeftEndpoint().x;
    int l1y1, l2y1;
    // there are two ways the two lines can be connected
    bool l1IsLeft = l1x1 < l2x1;

    // check if this corner is at the edge
    float distant = 0.0f;
    const vector < boost::shared_ptr<VisualLine> > * lines =
        vision->fieldLines->getLines();
    for (vector < boost::shared_ptr<VisualLine> >::const_iterator i =
             lines->begin();
         i != lines->end(); ++i) {
        distant = max((*i)->getDistance(), distant);
    }
    bool isInner = false;

    if (distant > corner.getLine1()->getDistance() &&
        distant > corner.getLine2()->getDistance()) {
        isInner = true;
    }

    if (face == FACING_UNKNOWN && !isInner && dist < 2.0f * GREEN_PAD_X &&
        horizon > 10) {
        lookForFieldCorner(corner, l1, l2);
    }

    // if we're a long way away and we don't see a goal post
    // it is highly likely that we see a field corner
    if (face == FACING_UNKNOWN && cornerDist > 150.0f) {
        if (l1 > 150 || l2 > 150) {
            lookForFieldCorner(corner, l1, l2);
            return;
        }
    }

    //223-11/scotty/dark_corner_far_ball/NBFRM.32
    if (face != FACING_UNKNOWN && !sameHalf && cornerDist > 300 &&
        objectDistance - cornerDist > 200) {
        // this isn't a corner at all it is part of the center circle
        corner.setShape(CIRCLE);
        corner.setSecondaryShape(CIRCLE);
        return;
    }

    // 223-11/annika/center_circle+varous/NBFRM.49
    if (face != FACING_UNKNOWN && cornerDist > 150 && objectDistance > 150 &&
        realDistance(corner.getX(), corner.getY(), objectRightX, objectRightY) >
        GOALBOX_DEPTH + 20) {
        if (objectRightX > corner.getX() && corner.doesItPointLeft()) {
            setFieldCorner(corner);
            return;
        } else if (objectRightX < corner.getX() && corner.doesItPointRight()) {
            setFieldCorner(corner);
            return;
        }
    }

    // 223-11/annika/center_circle+varous/NBFRM.43
    // but 223-11/field_pictures-dax/NBFRM.195
    if (face != FACING_UNKNOWN && cornerDist > 300) {
        if (objectRightX > corner.getX() && corner.doesItPointLeft()) {
            setFieldCorner(corner);
            return;
        } else if (objectRightX < corner.getX() && corner.doesItPointRight()) {
            setFieldCorner(corner);
            return;
        }
    }

    // Watch out for seeing a corner, but a part of goalbox too, but
    // the T of the goalbox is too close to edge to recognize
    // replace 70 with GREEN_PAD_X
    if (dist < GREEN_PAD_X * 2.0 && cornerDist > 175) {
        // could indicate this is actually a corner
        // we can do this if we see a post
        // punt for now
        if (face == FACING_UNKNOWN) {
            if (debugIdentifyCorners) {
                cout << "Punting on inner L " << dist << endl;
            }
            return;
        }
    }
    // l1 and l2 hold information on points away from the corner
    if (l1IsLeft) {
        l2x1 = corner.getLine2()->getRightEndpoint().x;
        l2y1 = corner.getLine2()->getRightEndpoint().y;
        l1y1 = corner.getLine1()->getLeftEndpoint().y;
    } else {
        l1x1 = corner.getLine1()->getRightEndpoint().x;
        l1y1 = corner.getLine1()->getRightEndpoint().y;
        l2y1 = corner.getLine2()->getLeftEndpoint().y;
    }
    // determine general facing
    if (face == FACING_UNKNOWN) {
        checkUnknownGoalCorner(corner, l1, l2, l1IsLeft);
    } else {
        checkGoalCornerWithPost(corner, l1y1, l2y1, l1IsLeft, dist);
    }
}

/** We see two inner Ls.  This is quite easy to handle, figure out which
    one is inside, then look at whether it is right or left of the other
    one.
    @param inner      the corner inside of the other
    @param outer      the corner outside of the other
 */
void Context::unconnectedInnerLs(VisualCorner & inner, VisualCorner & outer) {
    // inner is a field corner, outer a goal corner
    // determine which ones
    if (inner.getX() < outer.getX()) {
        inner.setSecondaryShape(RIGHT_GOAL_L);
        outer.setSecondaryShape(RIGHT_GOAL_CORNER);
    } else {
        inner.setSecondaryShape(LEFT_GOAL_L);
        outer.setSecondaryShape(LEFT_GOAL_CORNER);
    }
}

/** Given two corners that we know are not connected, explore to find out the
    possible relationships.  This should help us reduce the number of
    possible corners each can be and ultimately help the identification
    process.  Normally this is two innerls (goal corner, field corner).
    @param first       one corner
    @param second      the other
 */
void Context::findUnconnectedCornerRelationship(VisualCorner & first,
                                                VisualCorner & second) {
    VisualCorner* inner = &first;
    VisualCorner* outer = &second;
    if (first.getShape() == INNER_L) {
        if (second.getShape() == OUTER_L) {
            inner = &first;
            outer = &second;
        } else if (second.getShape() == INNER_L) {
            // Should be an easy case - find the "inner" innerl
            if (first.getY() < second.getY()) {
                unconnectedInnerLs(second, first);
            } else {
                unconnectedInnerLs(first, second);
            }
            return;
        } else {
            classifyInnerL(first);
            if (second.getShape() == T) {
                classifyT(second);
            }
            return;
        }
    } else if (second.getShape() == INNER_L) {
        if (first.getShape() == OUTER_L) {
            inner = &second;
            outer = &first;
        } else {
            classifyInnerL(second);
            if (first.getShape() == T) {
                classifyT(first);
            }
            return;
        }
    } else {
        if (first.getShape() == T) {
            classifyT(first);
        }
        if (second.getShape() == T) {
            classifyT(second);
        } else if (second.getShape() == OUTER_L) {
            classifyOuterL(second);
        }
        return;
    }
    // check the orientation of the outer-l
    if (outer->doesItPointRight()) {
        inner->setSecondaryShape(RIGHT_GOAL_CORNER);
        outer->setSecondaryShape(RIGHT_GOAL_L);
        return;
    } else {
        inner->setSecondaryShape(LEFT_GOAL_CORNER);
        outer->setSecondaryShape(LEFT_GOAL_L);
        return;
    }
}

/** We have connected T corners.  As this is impossible
    we should explore it further and see if one is a center circle.
    When this is called the common line should be first.getTStem()
    @param first        a T corner
    @param second       another T corner
*/
void Context::checkConnectedTs(VisualCorner & first, VisualCorner & second) {
    if (realLineDistance(first.getTStem()) > GOALBOX_DEPTH * GOALBOX_FUDGE) {
        classifyT(first);
        if (first.getSecondaryShape() == CENTER_T_TOP) {
            second.setSecondaryShape(CENTER_CIRCLE_TOP);
        } else if (first.getSecondaryShape() == CENTER_T_BOTTOM) {
            second.setSecondaryShape(CENTER_CIRCLE_BOTTOM);
        } else {
            second.setShape(CIRCLE);
        }
        tCorner--;
    }
}

/** We have a T connected to a center circle.  This is a nice easy case.
    See what the T is and potentially use that to narrow the circle.
    @param t       A T corner
    @param center  A center circle corner
 */
void Context::checkTToCenter(VisualCorner & t, VisualCorner & center) {
    classifyT(t);
    if (t.getSecondaryShape() == CENTER_T_TOP) {
        center.setSecondaryShape(CENTER_CIRCLE_TOP);
    } else if (t.getSecondaryShape() == CENTER_T_BOTTOM) {
        center.setSecondaryShape(CENTER_CIRCLE_BOTTOM);
    }
}

/** We have a T connected to a goalbox corner.  Theoretically the
    T could be anything, so try and narrow it down.
    @param   t    A T corner
    @param l1     A corner connected to the T
    @param common The line that the two corners have in common
 */
void Context::checkTToGoal(VisualCorner & t, VisualCorner & l1,
                           boost::shared_ptr<VisualLine> common) {
    t.setSecondaryShape(GOAL_T);
    l1.setSecondaryShape(GOAL_L);
    // looks very good - ultimately we should check line length too
    if (debugIdentifyCorners) {
        cout << "T connect to an L, should be goal box " <<
            realLineDistance(t.getTStem()) << " " << l1.getX() << endl;
    }
	// check distance of t to object if any
	float distToObject = 1000.0f;
	if (objectRightX >= 0) {
		distToObject = realDistance(t.getX(), t.getY(),
					 objectRightX, objectRightY);
	}
	bool objectIsClose = distToObject < GOALBOX_DEPTH * 2;

    // make sure we aren't side to circle
    if (realLineDistance(t.getTStem()) > 130.0f &&
		distToObject > MIDFIELD_X - 150 &&
		!seeGoalBoxLines) {
        t.setSecondaryShape(SIDE_T);
        l1.setShape(CIRCLE);
        return;
    }
    // can we determine which side?
    if (l1.getShape() == OUTER_L) {
        // check if we're right on the side
        if (objectDistance > t.getDistance() + GOALBOX_OVERAGE / 2) {
            if (t.doesItPointRight()) {
                t.setSecondaryShape(RIGHT_GOAL_T);
                l1.setSecondaryShape(RIGHT_GOAL_L);
            } else {
                t.setSecondaryShape(LEFT_GOAL_T);
                l1.setSecondaryShape(LEFT_GOAL_L);
            }
            return;
        }
        // look at the non-common line, figure out which direction it goes
        // if it goes in the same as left T, then it is a RIGHT L
        point<int> left;
        point<int> right;
        point<int> tpoint = t.getTStemEndpoint();
        if (l1.getLine1() == common) {
            left = l1.getLine2()->getLeftEndpoint();
            right = l1.getLine2()->getRightEndpoint();
        } else {
            left = l1.getLine1()->getLeftEndpoint();
            right = l1.getLine1()->getRightEndpoint();
        }
        // unfortunately the t-endpoint doesn't always equal the corner
        int close = abs(left.x - tpoint.x) + abs(left.y - tpoint.y);
        int close2 = abs(right.x - tpoint.x) + abs(right.y - tpoint.y);
        if (close > close2) {
            // Line goes to right of T
            l1.setSecondaryShape(LEFT_GOAL_L);
            t.setSecondaryShape(LEFT_GOAL_T);
        } else {
            l1.setSecondaryShape(RIGHT_GOAL_L);
            t.setSecondaryShape(RIGHT_GOAL_T);
        }
    } else if (l1.getShape() == INNER_L) {
        if (l1.doesItPointLeft()) {
            l1.setSecondaryShape(LEFT_GOAL_L);
            t.setSecondaryShape(LEFT_GOAL_T);
        } else {
            l1.setSecondaryShape(RIGHT_GOAL_L);
            t.setSecondaryShape(RIGHT_GOAL_T);
        }
    }
}

/** We have what has been classified as an innerl connected to an outerl.
    Since this is not normally possible we should check it out.  Generally
    it probably means that a T corner has been miscast as an L.  This can
    occur when looking at a goal from the far goalbox corner when the goal
    is edged out over the goal line.
    @param inner         an innerl corner
    @param outer         an outerl corner connected to the innerl
 */
void Context::checkInnerToOuter(VisualCorner & inner, VisualCorner & outer) {
    // if it is the T, then we should be able to see the nearby goal post
    if (face == FACING_GOAL) {
        boost::shared_ptr<VisualLine> common;
        if (inner.getLine1() == outer.getLine1()) {
            common = inner.getLine1();
        } else if (inner.getLine1() == outer.getLine2()) {
            common = inner.getLine1();
        } else {
            common = inner.getLine2();
        }
        float commonDist = realLineDistance(common);
        if (debugIdentifyCorners) {
            cout << "Checking inner to outer, common is " << commonDist << endl;
        }

        // if the common length is small enough it is a goal T
        if (commonDist < GOALBOX_DEPTH + 20.0f) {
            // one of them is a T corners - should be further away
            float d1, d2;
            if (outer.getLine1() == common) {
                d2 = realLineDistance(outer.getLine2());
            } else {
                d2 = realLineDistance(outer.getLine1());
            }
            if (inner.getLine1() == common) {
                d1 = realLineDistance(inner.getLine2());
            } else {
                d1 = realLineDistance(inner.getLine1());
            }
            if (d1 > d2) {
                // d1 is the T corner
                inner.changeToT(common);
                checkTToGoal(inner, outer, common);
            } else {
                outer.changeToT(common);
                checkTToFieldCorner(outer, inner);
            }
        } else if (commonDist < GOALBOX_WIDTH * 2) {
            // probably the two ends of the goal box - should be an easy case
            if (inner.getY() < outer.getY()) {
                if (inner.doesItPointRight()) {
                    inner.setSecondaryShape(RIGHT_GOAL_L);
                    outer.setSecondaryShape(LEFT_GOAL_L);
                } else {
                    inner.setSecondaryShape(LEFT_GOAL_L);
                    outer.setSecondaryShape(RIGHT_GOAL_L);
                }
            }
        }
    }
}

/** We have two connected outerls.  This is theoretically possible, but
    somewhat unlikely.  There are two major possibilities.  First, we
    are seeing both goalbox corners at once.  Second, we are misclassifying
    one or both of these (e.g. a T as an L, an innerl as an outerl).
 */
void Context::checkOuterToOuter(VisualCorner & first, VisualCorner & second) {
    boost::shared_ptr<VisualLine> common;
    if (first.getLine1() == second.getLine1()) {
        common = first.getLine1();
    } else if (first.getLine1() == second.getLine2()) {
        common = first.getLine1();
    } else {
        common = first.getLine2();
    }
    float commonDist = realLineDistance(common);
    if (debugIdentifyCorners) {
        cout << "Checking two outer Ls " << commonDist << endl;
    }

	// check distance of t to object if any
	float distToObject = 1000.0f;
	if (objectRightX >= 0) {
		distToObject = realDistance(first.getX(), first.getY(),
					 objectRightX, objectRightY);
		distToObject = min(distToObject, realDistance(second.getX(), second.getY(),
													  objectRightX, objectRightY));
	}
    if (debugIdentifyCorners) {
        cout << "Distance " << distToObject << endl;
    }

    // if the common length is small enough it is a goal T
    if (commonDist < GOALBOX_DEPTH + 20.0f || distToObject < 300) {
        // one of them is a T corners - should be further away
        float d1, d2;
        if (second.getLine1() == common) {
            d2 = realLineDistance(second.getLine2());
        } else {
            d2 = realLineDistance(second.getLine1());
        }
        if (first.getLine1() == common) {
            d1 = realLineDistance(first.getLine2());
        } else {
            d1 = realLineDistance(first.getLine1());
        }
        if (d1 > d2) {
            // d1 is the T corner
            first.changeToT(common);
            checkTToGoal(first, second, common);
        } else {
            second.changeToT(common);
            checkTToGoal(second, first, common);
        }
    } else {
        // probably the two ends of the goal box - should be an easy case
        if (debugIdentifyCorners) {
            cout << "REally are two outer Ls" << endl;
        }
        if (first.getY() < second.getY()) {
            if (first.doesItPointRight()) {
                first.setSecondaryShape(RIGHT_GOAL_L);
                second.setSecondaryShape(LEFT_GOAL_L);
            } else {
                first.setSecondaryShape(LEFT_GOAL_L);
                second.setSecondaryShape(RIGHT_GOAL_L);
            }
        } else {
            if (second.doesItPointRight()) {
                first.setSecondaryShape(RIGHT_GOAL_L);
                second.setSecondaryShape(LEFT_GOAL_L);
            } else {
                first.setSecondaryShape(LEFT_GOAL_L);
                second.setSecondaryShape(RIGHT_GOAL_L);
            }
        }
    }
}

/* We have a corner connected to another - but one of them is a CC or
   something.  We have reason to believe that maybe it is a T and and
   field corner.
 */
void Context::checkForBadTID(VisualCorner & first, VisualCorner & second,
                             boost::shared_ptr<VisualLine> common) {
    if (face != FACING_UNKNOWN) {
        // good chance that first is a T
        if (abs(first.getDistance() - objectDistance) < GOALBOX_OVERAGE * 2){
            // we need to figure out which line is the stem
            if (first.getLine1() == common) {
                first.changeToT(first.getLine2());
            } else {
                first.changeToT(first.getLine1());
            }
            // check if it is a goal or field corner
            if (realLineDistance(common) < GOALBOX_DEPTH + 20.0f) {
                checkTToGoal(first, second, common);
            } else {
                checkTToFieldCorner(first, second);
            }
        }
    } else {
        // we have a CC and something else, let's make sure we don't use it
        if (first.getShape() == CIRCLE) {
            float line1 = realLineDistance(second.getLine1());
            float line2 = realLineDistance(second.getLine2());
            //cout << "Dists " << line1 << " " << line2 << endl;
            if (line1 > 100.0f && line2 && 100.0f) {
                second.setShape(T);
                second.setSecondaryShape(SIDE_T);
            } else {
                second.setShape(CIRCLE);
            }
        } else {
            float line1 = realLineDistance(first.getLine1());
            float line2 = realLineDistance(first.getLine2());
            //cout << "Dists " << line1 << " " << line2 << endl;
            if (line1 > 100.0f && line2 && 100.0f) {
                first.setShape(T);
                first.setSecondaryShape(SIDE_T);
            } else {
                first.setShape(CIRCLE);
            }
        }
    }
}

/** We have a T and (apparently) a field corner. The T could be
    a side or a goal, so try and use length, # of corners and
    other things to narrow it down.
    @param t    The T corner
    @param l1   A corner connected to the T (innerl)
 */
void Context::checkTToFieldCorner(VisualCorner & t, VisualCorner & l1) {
    if (debugIdentifyCorners) {
        cout << "T connect to an L should be goal line to corner " <<
            realLineDistance(t.getTBar()) << endl;
    }
    l1.setSecondaryShape(CORNER_L);
    if (lCorner > 1) {
        // we should be able to narrow the choices down to 2
        // compare the location of the L corner to the T corner
        if (l1.getX() < t.getX()) {
            t.setSecondaryShape(RIGHT_GOAL_T);
            l1.setSecondaryShape(RIGHT_GOAL_CORNER);
        } else {
            l1.setSecondaryShape(LEFT_GOAL_CORNER);
            t.setSecondaryShape(LEFT_GOAL_T);
        }
    } else {
        // check length  -- it its REALLY long to corner then we know
        // its a sideline T
        float dist = realDistance(t.getX(), t.getY(), l1.getX(), l1.getY());
        // figure out if lcorner is left or right of T
        bool left = false;
        if (t.getLocation().x < l1.getLocation().x) {
            left = true;
        }
        if (dist * 2 < FIELD_WHITE_WIDTH) {
            // seems to be a goalline - evaluate further
            if (left) {
                t.setSecondaryShape(LEFT_GOAL_T);
                l1.setSecondaryShape(LEFT_GOAL_CORNER);
            } else {
                t.setSecondaryShape(RIGHT_GOAL_T);
                l1.setSecondaryShape(RIGHT_GOAL_CORNER);
            }
        } else {
            face = FACING_SIDELINE;
            t.setSecondaryShape(SIDE_T);
            if (left) {
                l1.setSecondaryShape(RIGHT_GOAL_CORNER);
            } else {
                l1.setSecondaryShape(LEFT_GOAL_CORNER);
            }
        }
    }
}

/** Given two corners that we know are connected, explore to find out the
    possible relationships.  This should help us reduce the number of
    possible corners each can be and ultimately help the identification
    process.  Since every corner on the field is connected to Ts (except
    the top two goal box corners) that is what we focus on.
    @param first    a connected corner
    @param second   the corner the first corner was connected to
 */
void Context::findCornerRelationship(VisualCorner & first,
                                     VisualCorner & second) {
    boost::shared_ptr<VisualLine> common;
    if (first.getLine1() == second.getLine1()) {
        common = first.getLine1();
    } else if (first.getLine1() == second.getLine2()) {
        common = first.getLine1();
    } else {
        common = first.getLine2();
    }
    if (first.getShape() == T) {
        if (second.getShape() == OUTER_L ||
            second.getShape() == INNER_L) {
            if (first.getTStem() == common) {
                checkTToGoal(first, second, common);
            } else {
                checkTToFieldCorner(first, second);
            }
        } else {
            // T to CIRCLE or T to T
            if (second.getShape() == T) {
                if (common == first.getTStem()) {
                    checkConnectedTs(first, second);
                } else {
                    checkConnectedTs(second, first);
                }
            } else if (second.getShape() == CIRCLE) {
				if (common == first.getLine2()) {
					if (debugIdentifyCorners) {
						cout << "Looks like a fake T" << endl;
					}
					first.setShape(CIRCLE);
					return;
				}
                checkTToCenter(first, second);
            }
        }
    } else if (second.getShape() == T) {
        if (first.getShape() == OUTER_L ||
            first.getShape() == INNER_L) {
            if (second.getTStem() == common) {
                checkTToGoal(second, first, common);
            } else {
                checkTToFieldCorner(second, first);
            }
        } else {
			if (common == second.getLine2()) {
				if (debugIdentifyCorners) {
					cout << "Looks like a fake T" << endl;
				}
				second.setShape(CIRCLE);
				return;
			}
            checkTToCenter(second, first);
        }
    } else {
        float commonDist = realLineDistance(common);
        if (debugIdentifyCorners) {
            cout << "Two non T corners with common length " <<
                commonDist << endl;
        }
        if (first.getShape() == INNER_L && second.getShape() == OUTER_L) {
            checkInnerToOuter(first, second);
        } else if (first.getShape() == OUTER_L && second.getShape() == INNER_L) {
            checkInnerToOuter(second, first);
        }
        // it is likely that one of the corners is actually a T
        // it is possible (never seen it) that it could be the two goal corners
        if (first.getShape() == OUTER_L && second.getShape() == OUTER_L) {
            checkOuterToOuter(first, second);
        }
        if (first.getShape() == CIRCLE && second.getShape() == CIRCLE) {
            return;
        }
        // false corner in the center
        if (objectDistance > 350.0f) {
            // TO DO:  instead of resetting, these should be removed
            if (first.getShape() == CIRCLE) {
                second.setShape(CIRCLE);
                return;
            } else {
                first.setShape(CIRCLE);
                return;
            }
            return;
        }
        // T + CC + ?  usually that ? is a false corner
        if ((face == FACING_UNKNOWN  || face == FACING_SIDELINE) &&
            getTCorner() > 0 && (first.getShape() == CIRCLE ||
                                 second.getShape() == CIRCLE)) {
            // TO DO:  instead of resetting, these should be removed
            if (first.getShape() == CIRCLE) {
                second.setShape(CIRCLE);
            } else {
                first.setShape(CIRCLE);
            }
            return;
        }
        // sometimes we see a goal T as a CC
        if (commonDist < BLUE_GOALBOX_BOTTOM_Y + 20 && (first.getShape() == CIRCLE ||
                                                        second.getShape() == CIRCLE)) {
            if (first.getDistance() < second.getDistance()) {
                checkForBadTID(first, second, common);
            } else {
                checkForBadTID(second, first, common);
            }
        }
        if (commonDist > 100.0f) {
            if (first.getShape() == CIRCLE) {
                float line1 = realLineDistance(second.getLine1());
                float line2 = realLineDistance(second.getLine2());
                //cout << "Dists " << line1 << " " << line2 << endl;
                if (line1 > 100.0f && line2 && 100.0f) {
                    second.setShape(T);
                    second.setSecondaryShape(SIDE_T);
                } else {
                    second.setShape(CIRCLE);
                }
            } else {
                float line1 = realLineDistance(first.getLine1());
                float line2 = realLineDistance(first.getLine2());
                //cout << "Dists " << line1 << " " << line2 << endl;
                if (line1 > 100.0f && line2 && 100.0f) {
                    first.setShape(T);
                    first.setSecondaryShape(SIDE_T);
                } else {
                    first.setShape(CIRCLE);
                }
            }
        }
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
        ConcreteCorner::getPossibleCorners(corner.getShape(),
                                           corner.getSecondaryShape());

    if (debugIdentifyCorners) {
        cout << endl
             << "Beginning to get possible classifications for corner given "
             << visibleObjects.size() << " visible objects and "
             << possibleCorners.size() << " corner possibilities" << endl
             << endl;
    }

    list<const ConcreteCorner*> classification;
    vector<const ConcreteCorner*>::const_iterator j =
        possibleCorners.begin();
    for (; j != possibleCorners.end(); ++j) {
        classification.push_back(*j);
    }
    return classification;
}

/**
 * Compares the given estimated distance against the actual distance between
 * the given concrete corner and the field object's true field position. Returns
 * true if the estimated distance is acceptable.
 *
 * @param estimatedDistance Distance between VisualCorner and VisualFieldObject
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
/* An artifact of the old field lines code.  May not even be used anymore
   except for corners we're really clueless about.
   @param obj      Object we want to test distance against
   @return         how much error will we tolerate
 */
float Context::getAllowedDistanceError(const VisualFieldObject * obj) const
{
    return obj->getDistanceSD() * 2;
}


/* We misidentified a post.	 Now that we've figured that out we need to
 *	switch it to the correct post.	Just transfer the information and reinit the
 *	previously IDd post.
 * @param p1	the correct post
 * @param p2	the wrong one
 */
// TODO: Use a copy constructor...
void Context::postSwap(VisualFieldObject * p1, VisualFieldObject * p2){
    p1->setLeftTopX(p2->getLeftTopX());
    p1->setLeftTopY(p2->getLeftTopY());
    p1->setLeftBottomX(p2->getLeftBottomX());
    p1->setLeftBottomY(p2->getLeftBottomY());
    p1->setRightTopX(p2->getRightTopX());
    p1->setRightTopY(p2->getRightTopY());
    p1->setRightBottomX(p2->getRightBottomX());
    p1->setRightBottomY(p2->getRightBottomY());
    p1->setX(p2->getLeftTopX());
    p1->setY(p2->getLeftTopY());
    p1->setWidth(p2->getWidth());
    p1->setHeight(p2->getHeight());
    p1->setCenterX(p2->getCenterX());
    p1->setCenterY(p2->getCenterY());
    p1->setIDCertainty(_SURE);
    p1->setDistanceCertainty(p2->getDistanceCertainty());
    p1->setDistance(1);
    p2->init();
}

/* This method should only be called with a VisualFieldObject object that has a
   concrete location on the field (YGLP, YGRP, BGLP, BGRP, BY, and YB).
   Calculates the length of the straight line between the two objects on the
   field.  The key here is that we only deal with actual objects as we are
   looking at the true physical distances, not the camera distances.
   @param c       A concrete (real) corner
   @param obj     A goal post
   @param which   Which goal post (if it is abstract)
   @return        floating point distance estimate
*/
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

/* Debugging method to print all of the current corner possibilities
   @param list       list of possibilities
 */
void Context::printPossibilities(const list <const ConcreteCorner*> &_list)
    const
{
    cout << "Possibilities: " << endl;
    for (list<const ConcreteCorner*>::const_iterator i = _list.begin();
         i != _list.end(); ++i) {
        cout << (*i)->toString() << endl;
    }
}

/* Creates a vector of all those field objects that are visible in the frame
   and have SURE certainty.
   @return   vector of goal posts that we have positively IDd.
*/
vector <const VisualFieldObject*> Context::getVisibleFieldObjects()
{
    vector <const VisualFieldObject*> visibleObjects;
    for (int i = 0; i < NUM_FIELD_OBJECTS_WITH_DIST_INFO; ++i) {
        if (allFieldObjects[i]->getDistance() > 0) {
            // We don't want to identify corners based on posts that aren't sure
            //allFieldObjects[i]->getIDCertainty() == _SURE) {
            // set field half information
            if (debugIdentifyCorners) {
                cout << "checking distance to field object " <<
                    allFieldObjects[i]->getDistance() << " " <<
                    MIDFIELD_X << endl;
                cout << "Object is at " << allFieldObjects[i]->getRightBottomX()
                     << " " << allFieldObjects[i]->getRightBottomY() << endl;
            }
            if (allFieldObjects[i]->getDistance() > 0) {
                if (objectDistance > 0) {
                    objectDistance = min(allFieldObjects[i]->getDistance(),
                                         objectDistance);
                } else {
                    objectDistance = allFieldObjects[i]->getDistance();
                }
            }
            objectRightX = allFieldObjects[i]->getRightBottomX();
            objectRightY = allFieldObjects[i]->getRightBottomY();
            // With the Nao we need to make sure that the goal posts are near
            // the green of the field in order to use them for distance
            if (ConcreteFieldObject::isGoal(allFieldObjects[i]->getID()) &&
                !(goalSuitableForPixEstimate(allFieldObjects[i]))) {
                // We can't use it.
            }
            else {
                visibleObjects.push_back(allFieldObjects[i]);
            }
        } else if (allFieldObjects[i]->getDistance() == 0 &&
                   allFieldObjects[i]->getIDCertainty() == _SURE) {
            // we see a post, but it is probably too close to get
            // a good distance (occluded on two sides)
            // we may not want to use the object too much, but it can help
            if (objectRightX < 1 && allFieldObjects[i]->getRightBottomX() >
                0) {
                objectRightX = allFieldObjects[i]->getRightBottomX();
                objectRightY = allFieldObjects[i]->getRightBottomY();
            }
        } else {
            if (allFieldObjects[i]->getDistance() > 0) {
                if (objectDistance > 0) {
                    objectDistance = min(allFieldObjects[i]->getDistance(),
                                         objectDistance);
                } else {
                    objectDistance = allFieldObjects[i]->getDistance();
                }
            }
            /*if (objectRightX < 1) {
              if (debugIdentifyCorners) {
              cout << "checking distance to weird field object " <<
              allFieldObjects[i]->getDistance() << " " <<
              MIDFIELD_X << endl;
              cout << "Object is at " <<
              allFieldObjects[i]->getRightBottomX()
              << " " << allFieldObjects[i]->getRightBottomY() <<
              endl;
              }
              objectRightX = allFieldObjects[i]->getRightBottomX();
              objectRightY = allFieldObjects[i]->getRightBottomY();
              }*/
        }
    }
    return visibleObjects;
}

/* Returns all of the field objects, regardless of their certainty.
   @return   a vector of all goal posts
 */
vector<const VisualFieldObject*> Context::getAllVisibleFieldObjects() const
{
    vector <const VisualFieldObject*> visibleObjects;
    visibleObjects.clear();
    for (int i = 0; i < NUM_FIELD_OBJECTS_WITH_DIST_INFO; ++i) {
        if (allFieldObjects[i]->getDistance() > 0){
                visibleObjects.push_back(allFieldObjects[i]);
        }
    }
    return visibleObjects;
}

/* This is called when we have identified a robot and a ball. We'll
   check how likely it is that the robot might kick the ball and try
   and determine a level of danger and a possible direction.
 */
void Context::checkForKickDanger(VisualRobot *robot) {
    const float ROBOT_TO_BALL = 30.0f;
    const int ROBOT_OFFSET = IMAGE_WIDTH / 4;
    int robotX = robot->getCenterX();
    int robotY = robot->getCenterY();
    int ballX = vision->ball->getCenterX();
    int ballY = vision->ball->getCenterY();
    // sometimes we see ourselves when looking straight down
    if (robotY > ballY) {
        return;
    }
    float heat = 0.0f;
    float distance = abs(robot->getDistance() - vision->ball->getDistance());
    float dist = realDistance(robotX, ballY, ballX, ballY);
    if (dist < 13.0f) {
        heat += 10.0f;
    } else if (dist < 16.0f) {
        heat += 7.5f;
    } else if (dist < 21.0f) {
        heat += 2.5f;
    }
    if (distance < 20.0f) {
        heat += 10.0f;
    } else if (distance < 30.0f) {
        heat += 7.5f;
    } else if (distance < 40.0f) {
        heat += 2.5f;
    }
    if (debugDangerousBall) {
        cout << "Dangerous ball check: " << heat << endl;
    }
    float currentHeat = vision->ball->getHeat();
    if (heat > currentHeat) {
        vision->ball->setHeat(heat);
    }
}

/* Much like the last method we are checking if a robot might kick
   the ball.  The need for this method is that sometimes we don't
   id a robot when it is in the frame (and near the ball). So try a
   little harder if we haven't id'd any.
 */
void Context::checkForKickDangerNoRobots() {
    int ballX = vision->ball->getX();
    int ballY = vision->ball->getY();
    int width = static_cast<int>(vision->ball->getWidth());
    int height = static_cast<int>(vision->ball->getHeight());
    float topDist = thresh->getPixDistance(ballY) + 30.0f;
    int count = 0, total = 0;
    float heat = 0.0f;
    int row = ballY - 1;
    // zone 1 - right above the ball
    for (int i = ballY - 1; i >= 0 && thresh->getPixDistance(i) < topDist; i--) {
        for (int j = ballX; j < ballX + width; j++) {
            unsigned char pixel = thresh->getThresholded(i, j);
            if (Utility::isWhite(pixel)) {
                count++;
            }
            total++;
        }
        row = i;
    }
    int gap = ballY - row;
    row = row - gap - gap / 2;
    if (count * 2 > total) {
        heat += 5.0f;
    } else if (count * 3 > total) {
        heat += 2.5f;
    }
    // zone 2 to the left and above the ball
    count = 0;
    total = 0;
    for (int i = ballY + height / 2; i >= 0 && thresh->getPixDistance(i) <
             topDist; i--) {
        for (int j = max(0, ballX - width); j < ballX; j++) {
            unsigned char pixel = thresh->getThresholded(i, j);
            if (Utility::isWhite(pixel)) {
                count++;
            }
            total++;
        }
    }
    if (count * 2 > total) {
        heat += 5.0f;
    } else if (count * 3 > total) {
        heat += 2.5f;
    }
    // zone 3 to the right and above the ball
    count = 0;
    total = 0;
    for (int i = ballY + height / 2; i >= 0 && thresh->getPixDistance(i) <
             topDist; i--) {
        for (int j = ballX + width; j < ballX + 2 * width && j < IMAGE_WIDTH;
             j++) {
            unsigned char pixel = thresh->getThresholded(i, j);
            if (Utility::isWhite(pixel)) {
                count++;
            }
            total++;
        }
    }
    if (count * 2 > total) {
        heat += 5.0f;
    } else if (count * 3 > total) {
        heat += 2.5f;
    }
    if (heat >= 5.0f && vision->ball->getDistance() > 50 &&
        vision->ball->getDistance() <  350) {
        // do some extra scanning for uniforms
        count = 0;
        total = 0;
        for (int i = row; i >=0 && i > row - 20; i--) {
            for (int col = max(0, ballX - width / 2); col < min(IMAGE_WIDTH - 1,
                                                                ballX + 2 * width);
                 col++) {
                unsigned char pixel = thresh->getThresholded(i, col);
                if (Utility::isRed(pixel) || Utility::isNavy(pixel)) {
                    count++;
                }
                total++;
            }
        }
        int target = width * height / 5;
        if (count > target) {
            heat += 7.5f;
        } else if (count * 2 > target) {
            heat += 5.0f;
        } else if (count * 3 > target) {
            heat += 2.5f;
        }
        if (debugDangerousBall) {
            cout << "Uniform count " << count << " " << target << endl;
        }
    }
    vision->ball->setHeat(heat);
    if (debugDangerousBall) {
        cout << "Dangerous Ball no robot check: " << heat << " " <<
            count << " " << total << endl;
    }
}

/* In some Nao frames, robots obscure part of the goal and the bottom is not
   visible.  We can only use pix estimates of goals whose bottoms are visible
   @param goal       a goal we'd like to check distances to
   @return           whether or not it is a good idea
*/
const bool Context::goalSuitableForPixEstimate(const VisualFieldObject * goal)
    const
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

/*  Calculate the actual distance between two points.  Uses functions
    from NaoPose.cpp in Noggin
    @param x1   x coord of object 1
    @param y1   y coord of object 1
    @param x2   x coord of object 2
    @param y2   y coord of object 2
    @return     the distance in centimeters
 */

float Context::realDistance(int x1, int y1, int x2, int y2)  const {
    estimate r = vision->pose->pixEstimate(x1, y1, 0.0);
    estimate l = vision->pose->pixEstimate(x2, y2, 0.0);
    return vision->pose->getDistanceBetweenTwoObjects(l, r);
}


/* Get the actual length of a line.  VisualLine actually just calculates
   the length on the screen.  Here we need the real thing.
   @param line      a line we would like to have the length of
   @return          the length of the line in centimeters
 */
float Context::realLineDistance(boost::shared_ptr<VisualLine> line) {
    const point<int> end1 = line->getEndpoint();
    const point<int> end2 = line->getStartpoint();
    return realDistance(end1.x, end1.y, end2.x, end2.y);
}


/* Set facing information.  Initially this is based on whether we see a goal
   post or not.  If we do then we are facing one.  Eventually we may realize
   that we are facing a sideline (e.g. if we see a side T and no goal).
   Down the road we may want to make it more fine grained (e.g. facing a
   particular field corner).
 */
void Context::setFacing() {
    if (seePost) {
        face = FACING_GOAL;
    } else {
        face = FACING_UNKNOWN;
    }
}

/** Set the field half if possible by looking at the distance to any field
    objects.
 */
void Context::setFieldHalf() {
    if (face == FACING_UNKNOWN) {
        fieldHalf = HALF_UNKNOWN;
        return;
    }
}

/**
   Print lots of useful debugging information.
*/
void Context::printContext() {
    cout << "Begining identify Corners." << endl;
    cout << "   Facing: ";
    switch (face) {
    case FACING_GOAL:
        cout << "Goals " << endl;
        if (rightPost) {
            cout << "     We see a right post" << endl;
        }
        if (leftPost) {
            cout << "     We see a left post" << endl;
        }
        if (unknownPost) {
            cout << "     We see an unidentified post" << endl;
        }
        break;
    case FACING_SIDELINE:
        cout << "Sideline " << endl;
        break;
    case FACING_UNKNOWN:
        cout << "Unknown " << endl;
        break;
    }
    cout << "   We can see:" << endl;
    cout << "                " << tCorner << " T corners " << endl;
    cout << "                " << lCorner << " L corners " << endl;
    cout << "                " << cCorner << " CC corners" << endl;
    if (cCorner < 1 && seeCenterCircle) {
        cout << "    We see the center circle" << endl;
    }
    if (seeGoalBoxLines) {
        cout << "    We see goal box lines" << endl;
    }
}


}
}
