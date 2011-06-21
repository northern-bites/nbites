from . import TrackingConstants as constants
import man.motion.HeadMoves as HeadMoves

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
def readyLoc(tracker):
    """
    Assumes the robot is in the "ready" state.
    Looks at nearby landmarks for localization.
    """
    tracker.decisionState = 'readyLoc'
    #update tracking fitness of locObjects
    for obj in tracker.locObjectList:
        tracker.helper.updateGeneralTrackingFitness(obj)
    #sort list of locObjects
    newlist = sorted(tracker.locObjectList)

    if not newlist == tracker.locObjectList:
        #landmarks have changed fitness ranking. Track most fit
        tracker.locObjectList = newlist
        tracker.target = tracker.locObjectList[0]
        print "target Id:",tracker.target.visionId# ** #debugging
        return tracker.goLater('trackLoc')

    #Assert: no change in list order

    #check for no target (first pass in readyLoc state)
    if tracker.target is None:
        tracker.target = tracker.locObjectList[0]

    #track next most fit locObject
    oldIndex = tracker.locObjectList.index(tracker.target)
    tracker.target = tracker.locObjectList[oldIndex+1]
    #check for unfit trackingFitness
    if tracker.target.trackingFitness > constants.FITNESS_THRESHOLD:
        #cycle to most fit locObject
        tracker.target = tracker.locObjectList[0]

    print "target Id:",tracker.target.visionId# ** #debugging

    #track target
    return tracker.goLater('trackLoc')

# ** # new method
def trackingBallLoc(tracker):
    """
    Periodically looks directly at ball.
    Otherwise, looks for nearest landmark, without losing sight
    of the ball.
    """
    # Make sure that head is inactive
    if tracker.firstFrame():
        tracker.brain.motion.stopHeadMoves()

    # When finished, states called by trackingBallLoc will return to
    # trackingBall state, which will then return here.
    tracker.decisionState = 'trackingBall'

    # Update landmark fitness by angular distance from ball
    for obj in tracker.locObjectList:
        tracker.helper.updateAngularTrackingFitness(obj,tracker.target)
    # Sort list of locObjects, set target to most fit
    tracker.locObjectList = sorted(tracker.locObjectList)
    tracker.target = tracker.locObjectList[0]

    # Sanity check most fit locObject
    if fabs(tracker.brain.ball.elevation - tracker.target.elevation) > \
            constants.ELEVATION_OFFSET_LIMIT or \
            fabs(tracker.brain.ball.bearing - tracker.target.bearing) > \
            constants.BEARING_OFFSET_LIMIT:
        print "No viable landmark to check."
        return tracker.goLater('trackingBall')

    # Track target
    return tracker.goLater('trackLoc')
