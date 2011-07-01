from man.motion import MotionConstants
from . import TrackingConstants as constants
from ..typeDefs.Landmarks import FieldObject
from ..typeDefs.Landmarks import FieldCorner

DEBUG = False

# ** # old method
def ballTracking(tracker):
    '''Super state which handles following/refinding the ball'''
    if tracker.target.framesOff <= constants.TRACKER_FRAMES_OFF_REFIND_THRESH:
        return tracker.goNow('tracking')
    else:
        return tracker.goNow('scanBall')

# ** # old method
def tracking(tracker):
    """
    While the target is visible, track it via vision values.
    If the ball is lost, go to last diff state.
    """

    if tracker.firstFrame():
        tracker.activeLocOn = False

    tracker.helper.trackObject()
    if not tracker.target.on:
        if DEBUG : tracker.printf("Missing object this frame",'cyan')
        if tracker.target.framesOff > constants.TRACKER_FRAMES_OFF_REFIND_THRESH:
            return tracker.goLater(tracker.lastDiffState)
        return tracker.stay()

    return tracker.stay()

# ** # old method
def ballSpinTracking(tracker):
    '''Super state which handles following/refinding the ball'''
    if tracker.target.framesOff <= constants.TRACKER_FRAMES_OFF_REFIND_THRESH:
        return tracker.goNow('tracking')
    else:
        return tracker.goNow('spinScanBall')


# ** # old method
def activeTracking(tracker):
    """
    If ball is visible and close, track it via vision values.
    If ball is not visible, execute naive pans.
    If state counter is low enough, track ball via vision values.
    If state counter is high enough, perform triangle pans
    and return to last head angles.
    """
    if tracker.firstFrame():
        tracker.activeLocOn = True

    if tracker.target.locDist < constants.MAX_ACTIVE_TRACKING_DIST and \
            tracker.target.framesOn > 2:
        return tracker.goLater('tracking')

    if tracker.target.framesOff > constants.TRACKER_FRAMES_OFF_REFIND_THRESH \
            and not tracker.brain.motion.isHeadActive() \
            and not (tracker.activePanOut):
        return tracker.goLater('activeLocScan')

    elif not tracker.activePanOut and \
            tracker.counter <= constants.ACTIVE_LOC_STARE_THRESH \
            and not tracker.goalieActiveLoc:
        tracker.helper.trackObject()
        tracker.activePanOut = False
        return tracker.stay()

    elif not tracker.activePanOut and \
            tracker.counter <= constants.ACTIVE_LOC_STARE_GOALIE_THRESH \
            and tracker.goalieActiveLoc:
        tracker.helper.trackObject()
        tracker.activePanOut = False
        return tracker.stay()

    elif tracker.activePanOut:
        tracker.activePanOut = False
        return tracker.goLater('returnHeadsPan')
    else :
        tracker.activePanOut = True
        motionAngles = tracker.brain.sensors.motionAngles
        tracker.preActivePanHeads = (
            motionAngles[MotionConstants.HeadYaw],
            motionAngles[MotionConstants.HeadPitch])

        return tracker.goLater('trianglePan')

    return tracker.stay()

# ** # new method
def trackLoc(tracker):
    """
    Look towards the current target, then stare at it for
    TRACKER_FRAMES_STARE_THRESH frames.
    """
    # make sure head is inactive first
    if tracker.firstFrame():
        tracker.brain.motion.stopHeadMoves()

    # ** # debugging
    #if isinstance(tracker.target, FieldCorner):
    #    print "target is corner:",tracker.target.visionId
    #elif isinstance(tracker.target, FieldObject):
    #    print "target is post:",tracker.target.visionId

    # safety check that target was not set to a ball
    if tracker.target is None or tracker.target == tracker.brain.ball:
        print "target is None, or the ball"
        return tracker.goLater(tracker.decisionState)

    tracker.helper.lookToTargetCoords(tracker.target)

    """
    # if close enough to target, switch to stareLoc
    if tracker.helper.inView(tracker.target):
        print "found target on frame, now staring"
        return tracker.goLater('stareLoc')
        """

    # Once any visual object is in frame, stare at it
    for obj in tracker.locObjectList:
        if obj.on:
            tracker.target = obj
            return tracker.goLater('stareLoc')

    if tracker.counter > constants.TRACKER_FRAMES_SEARCH_THRESH:
        print "Past search thresh, switching to new target"
        return tracker.goLater(tracker.decisionState)

    return tracker.stay()

# ** # new state
def stareLoc(tracker):
    """
    Dynamically stare at the target, then return to decisionState.
    Note: Does not alter tracker.target
    """
    # Make sure head is inactive first
    if tracker.firstFrame():
        print "stare target is:",tracker.target.visionId
        tracker.brain.motion.stopHeadMoves()

    if tracker.counter > constants.TRACKER_FRAMES_STARE_THRESH:
        print "Past stare thresh, switching to new target"
        return tracker.goLater(tracker.decisionState)

    # Find the real post in vision frame
    if tracker.target is FieldObject:
        stareTarget = tracker.helper.findPostInVision(tracker.target, tracker.brain)
    else: # Target is a corner.
        stareTarget = tracker.helper.findCornerInVision(tracker.target, tracker.brain)

    # Second safety check that something was on frame
    if stareTarget is None:
        print "stare target not in vision"
        return tracker.stay()

    print "target visible"
    tracker.helper.lookToTargetAngles(stareTarget)
    print "target at d/b:",tracker.target.visDist,tracker.target.visBearing

    return tracker.stay()

# ** # new state
def trackingBall(tracker):
    """
    Look directly at ball for a short time.
    """
    # make sure head is inactive first
    if tracker.firstFrame():
        tracker.brain.motion.stopHeadMoves()
    # ** # debugging
    print "tracking ball"

    # Look to the ball for TRACKER_BALL_STARE_THRESH frames
    # If ball is lost on frame, look towards coordinates.
    if tracker.brain.ball.on:
        tracker.helper.lookToTargetAngles(tracker.brain.ball)
    else:
        tracker.helper.lookToTargetCoords(tracker.brain.ball)
        """
    # If we haven't seen the ball in some time, switch to panning
    if tracker.brain.ball.framesOff > constants.TRACKER_BALL_STARE_THRESH/2:
        print "lost ball for some time, panning"
        return tracker.goLater('scanBall')
        """
    if tracker.counter > constants.TRACKER_BALL_STARE_THRESH:
        print "Past stare thresh for ball"
        return tracker.goLater(tracker.decisionState)

    return tracker.stay()

