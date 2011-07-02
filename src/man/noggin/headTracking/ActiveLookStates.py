from . import TrackingConstants as constants
from .. import NogginConstants
import man.motion.HeadMoves as HeadMoves
from math import (fabs)

TIME_TO_LOOK_TO_TARGET = 1.0

# ** # old method
def lookToPoint(tracker):
    """Look to the localization coords of the stored target."""
    tracker.helper.lookToPoint(tracker.target)
    return tracker.stay()

# ** # old method
def lookToTarget(tracker):
    """
    Look to localization coords of target.
    If too much time passes, perform naive pans.
    If ball is seen, go to 'targetTracking' or 'activeTracking'.
    """
    if tracker.target.vis.framesOn > constants.TRACKER_FRAMES_ON_TRACK_THRESH:
        tracker.brain.motion.stopHeadMoves()
        if tracker.activeLocOn:
            return tracker.goNow('activeTracking')
        else:
            return tracker.goNow('targetTracking')

    elif tracker.stateTime >= TIME_TO_LOOK_TO_TARGET:
        tracker.brain.motion.stopHeadMoves()
        return tracker.goLater('scanForTarget')

    tracker.helper.lookToPoint(tracker.target)

    return tracker.stay()

# ** # old method
def scanForTarget(tracker):
    """
    Performs naive scan for target.
    If ball is seen, go to 'targetTracking' or 'activeTracking'.
    """
    if tracker.target.vis.framesOn > constants.TRACKER_FRAMES_ON_TRACK_THRESH:
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
    Track the target via vision values.
    If target is lost, look to localization values.
    If that fails, use naive scans.
    """

    if tracker.firstFrame():
        tracker.brain.motion.stopHeadMoves()
        tracker.activeLocOn = False

    if tracker.target.vis.framesOff > constants.TRACKER_FRAMES_OFF_REFIND_THRESH:
        return tracker.goLater('lookToTarget')

    tracker.helper.trackObject()

    return tracker.stay()
