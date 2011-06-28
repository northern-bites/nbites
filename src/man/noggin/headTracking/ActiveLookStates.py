from . import TrackingConstants as constants
from .. import NogginConstants
import man.motion.HeadMoves as HeadMoves
from math import (fabs)
from ..typeDefs.Landmarks import FieldCorner # ** # imported for debugging
from ..typeDefs.Landmarks import FieldObject # ** # imported for debugging

TIME_TO_LOOK_TO_TARGET = 1.0

# ** # old method
def lookToPoint(tracker):
    tracker.helper.lookToPoint(tracker.target)
    return tracker.stay()

# ** # old method
def lookToTarget(tracker):
    """looks to best guess of where target is"""

    if tracker.target.framesOn > constants.TRACKER_FRAMES_ON_TRACK_THRESH:
        tracker.brain.motion.stopHeadMoves()
        if tracker.activeLocOn:
            tracker.brain.motion.stopHeadMoves()
            return tracker.goNow('activeTracking')
        else:
            tracker.brain.motion.stopHeadMoves()
            return tracker.goNow('targetTracking')

    elif tracker.stateTime >= TIME_TO_LOOK_TO_TARGET:
        tracker.brain.motion.stopHeadMoves()
        return tracker.goLater('scanForTarget')

    tracker.helper.lookToPoint(tracker.target)

    return tracker.stay()

# ** # old method
def scanForTarget(tracker):
    """performs naive scan for target"""
    if tracker.target.framesOn > constants.TRACKER_FRAMES_ON_TRACK_THRESH:
        print "target on"
        if tracker.activeLocOn:
            tracker.brain.motion.stopHeadMoves()
            return tracker.goNow('activeTracking')
        else:
            tracker.brain.motion.stopHeadMoves()
            return tracker.goNow('targetTracking')

    if not tracker.brain.motion.isHeadActive():
        print "head not active"
        targetDist = tracker.target.locDist

        if targetDist > HeadMoves.HIGH_SCAN_CLOSE_BOUND:
            tracker.helper.executeHeadMove(HeadMoves.HIGH_SCAN_BALL)

        elif ( targetDist > HeadMoves.MID_SCAN_CLOSE_BOUND and
               targetDist < HeadMoves.MID_SCAN_FAR_BOUND ):
            tracker.helper.executeHeadMove(HeadMoves.MID_UP_SCAN_BALL)

        else:
            tracker.helper.executeHeadMove(HeadMoves.LOW_SCAN_BALL)

    return tracker.stay()

# ** # old method
def targetTracking(tracker):
    """
    state askes it's parent (the tracker) for an object or angles to track
    while the object is on screen, or angles are passed, we track it.
    Otherwise, we continually write the current values into motion via setHeads.

    If a sweet move is begun while we are tracking, the current setup is to let
    the sweet move conclude and then resume tracking afterward.
    """

    if tracker.firstFrame():
        tracker.brain.motion.stopHeadMoves()
        tracker.activeLocOn = False

    if tracker.target.framesOff > constants.TRACKER_FRAMES_OFF_REFIND_THRESH:
        return tracker.goLater('lookToTarget')

    tracker.helper.trackObject()

    return tracker.stay()

# ** # new method
def trackLandmarks(tracker):
    """
    Looks at nearby landmarks for localization.
    """
    # Update tracking fitness of locObjects
    for obj in tracker.locObjectList:
        tracker.helper.updateGeneralTrackingFitness(obj)
    # Sort list of locObjects
    newlist = sorted(tracker.locObjectList)

    if not newlist == tracker.locObjectList:
        #Landmarks have changed fitness ranking. Track most fit.
        tracker.locObjectList = newlist
        tracker.target = tracker.locObjectList[0]
        print "fitness list changed"# ** #debugging
        return tracker.goLater('trackLoc')

    # Assert: no change in list order

    # Check for no target (or ball target)
    if tracker.target is None or tracker.target == tracker.brain.ball:
        print "ALS: target is None"# ** #debugging
        tracker.target = tracker.locObjectList[0]

    # Track next most fit locObject
    oldIndex = tracker.locObjectList.index(tracker.target)
    tracker.target = tracker.locObjectList[oldIndex+1]
    # Check for unfit trackingFitness
    if tracker.target.trackingFitness > constants.FITNESS_THRESHOLD:
        # Cycle to most fit locObject
        tracker.target = tracker.locObjectList[0]

    print "going to: track target"# ** #debugging

    # Track target
    return tracker.goLater('trackLoc')

# ** # new method
def trackingBallLoc(tracker):
    """
    Periodically looks directly at ball.
    Otherwise, looks for nearest landmark, without losing sight
    of the ball if boolean is set in headTracking.
    """
    # Make sure that head is inactive
    if tracker.firstFrame():
        tracker.brain.motion.stopHeadMoves()

    tracker.decisionState = 'trackingBallLoc'

    # If we didn't arrive here from trackingBall, go there now.
    if tracker.lastDiffState != 'trackingBall':
        return tracker.goLater('trackingBall')

    # Update landmark fitness by angular distance from ball
    for obj in tracker.locObjectList:
        tracker.helper.updateAngularTrackingFitness(obj,tracker.target)
    # Sort list of locObjects
    newlist = sorted(tracker.locObjectList)

    # Check for no target (first pass in method)
    if tracker.target is None:
        tracker.target = tracker.locObjectList[0]
    elif not newlist == tracker.locObjectList:
        # Landmarks have changed fitness ranking. Track most fit.
        tracker.locObjectList = newlist
        tracker.target = tracker.locObjectList[0]
        return tracker.goLater('trackLoc')
    else:
        # Track next most fit locObject
        oldIndex = tracker.locObjectList.index(tracker.target)
        tracker.target = tracker.locObjectList[oldIndex+1]

    # Set values for sanity check.
    if (tracker.safeBallTracking):
        elevationCheck = fabs(tracker.brain.ball.elevation - \
                                  tracker.target.elevation) < \
                                  constants.ELEVATION_OFFSET_LIMIT
        bearingCheck = fabs(tracker.brain.ball.bearing - \
                                tracker.target.bearing) < \
                                constant.BEARING_OFFSET_LIMIT
    else:
        # Elevation rarly inhibits looking towards an object
        elevationCheck = True
        bearingCheck = fabs(tracker.target.bearing) < \
            constants.BEARING_THRESHOLD

    # Sanity check on target
    while not elevationCheck or not bearingCheck:
        # If this is most fit target, there are no viable landmarks
        if tracker.target == tracker.locObjectList[0]:
            print "No viable landmark to check."
            return tracker.goLater('trackingBall')
        else:
            # Cycle to most fit landmark, then sanity check it.
            tracker.target = tracker.locObjectList[0]

    # Track target
    return tracker.goLater('trackLoc')

# ** # new method
def passiveLoc(tracker):
    """
    Looks at nearby landmarks until sufficiently localized,
    Then looks to center of field for ball.
    """
    my = tracker.brain.my
    if my.locScoreXY == NogginConstants.OK_LOC or \
            my.locScoreXY == NogginConstants.GOOD_LOC and \
            my.locScoreTheta == NogginConstants.OK_LOC or \
            my.locScoreTheta == NogginConstants.GOOD_LOC:
        print "I am sure about my position. Now looking to the ball."
        # Set ball position to center of field.
        tracker.target = tracker.brain.ball
        tracker.target.x = 370
        tracker.target.y = 270
        tracker.brain.ball.updateLoc(tracker.brain.loc,tracker.brain.my)
        tracker.helper.lookToTargetCoords(tracker.target)
        return tracker.stay()

    # Not confident enough about position:
    # check next landmark.
    return tracker.goLater('trackLandmarks')

# ** # new method
def activeLoc(tracker):
    """
    Intended for field players who are not chaser.
    Check nearby landmarks until localization is good enough,
    then look at the ball while localization remains good.
    """
    # Make sure head is stopped
    if tracker.firstFrame():
        tracker.brain.motion.stopHeadMoves()

    my = tracker.brain.my
    if my.locScoreXY == NogginConstants.GOOD_LOC and \
            my.locScoreTheta == NogginConstants.GOOD_LOC:
        # If localization is good, look to the ball
        print "I am sure about my position. Now checking ball."
        return tracker.goLater('checkBall')

    # If localization not good enough, check landmarks
    return tracker.goLater('trackLandmarks')
