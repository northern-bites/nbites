from man.motion import MotionConstants
from . import TrackingConstants as constants

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
    state askes it's parent (the tracker) for an object or angles to track
    while the object is on screen, or angles are passed, we track it.
    Otherwise, we continually write the current values into motion via setHeads.

    If a sweet move is begun while we are tracking, the current setup is to let
    the sweet move conclude and then resume tracking afterward.
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
    Method to perform tracking
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
            tracker.counter <= constants.ACTIVE_LOC_STARE_THRESH:
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

    # safety check that currentLocObject was set to a post
    if tracker.target is None or tracker.target == tracker.brain.ball:
        print "target is None, or the ball"
        return tracker.goLater(tracker.decisionState)

    tracker.helper.lookToTargetCoords(tracker.target)

    # if close enough to target, switch to stareLoc
    if tracker.target.framesOn > constants.TRACKER_FRAMES_ON_TRACK_THRESH:
        print "found target on frame, now staring"
        return tracker.goLater('stareLoc')

    # debugging
    #print "bglp on?:",tracker.target.on
    #print "target on?:",target.on


    if tracker.counter > constants.TRACKER_FRAMES_SEARCH_THRESH:
        print "$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$"
        print "Past search thresh, switching to new target"
        return tracker.goLater(tracker.decisionState)

    return tracker.stay()

# ** # new state
def stareLoc(tracker):
    """
    Dynamically stare at the target, then return to decisionState.
    """
    # make sure head is inactive first
    if tracker.firstFrame():
        tracker.brain.motion.stopHeadMoves()

    # find the real post in vision frame
    target = tracker.helper.findPostInVision(tracker.target, tracker.brain)

    # second safety check that any post is on the frame
    if target is None:
        print "no post of correct color on frame"
        return tracker.stay()

    tracker.helper.lookToTargetAngles(target)

    if tracker.counter > constants.TRACKER_FRAMES_STARE_THRESH:
        print "$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$"
        print "Past stare thresh, switching to new target"
        return tracker.goLater(tracker.decisionState)

    return tracker.stay()

# ** # new state
def trackingBall(tracker):
    """
    Look directly at ball for a short time.
    Then, check for nearby landmarks without losing sight of the ball.
    """
    # make sure head is inactive first
    if tracker.firstFrame():
        tracker.brain.motion.stopHeadMoves()

    # Look to the ball for TRACKER_BALL_STARE_THRESH frames
    # If ball is lost on frame, do nothing.
    if tracker.brain.ball.on:
        tracker.helper.lookToTargetAngles(tracker.brain.ball)

    if tracker.counter > constants.TRACKER_BALL_STARE_THRESH:
        print "Past stare thresh for ball, looking for landmarks"
        return tracker.goLater('trackingBallLoc')

    return tracker.stay()

