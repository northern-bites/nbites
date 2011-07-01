from man.motion import MotionConstants
from . import TrackingConstants as constants
from ..typeDefs.Landmarks import FieldObject
from ..typeDefs.Landmarks import FieldCorner

DEBUG = False

# ** # old method
def ballTracking(tracker):
    '''Super state which handles following/refinding the ball'''
    if tracker.target.vis.framesOff <= constants.TRACKER_FRAMES_OFF_REFIND_THRESH:
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
    if not tracker.target.vis.on:
        if DEBUG : tracker.printf("Missing object this frame",'cyan')
        if tracker.target.vis.framesOff > constants.TRACKER_FRAMES_OFF_REFIND_THRESH:
            return tracker.goLater(tracker.lastDiffState)
        return tracker.stay()

    return tracker.stay()

# ** # old method
def ballSpinTracking(tracker):
    '''Super state which handles following/refinding the ball'''
    if tracker.target.vis.framesOff <= constants.TRACKER_FRAMES_OFF_REFIND_THRESH:
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
            tracker.target.vis.framesOn > 2:
        return tracker.goLater('tracking')

    if tracker.target.vis.framesOff > constants.TRACKER_FRAMES_OFF_REFIND_THRESH \
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
