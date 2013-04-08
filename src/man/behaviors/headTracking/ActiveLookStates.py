from . import TrackingConstants as constants
import HeadMoves as HeadMoves

TIME_TO_LOOK_TO_TARGET = 1.0

def lookToPoint(tracker):
    """Look to the localization coords of the stored target."""
    tracker.helper.lookToPoint(tracker.target)
    return tracker.stay()

def lookToTarget(tracker):
    """
    Look to localization coords of target.
    If too much time passes, perform naive pans.
    If ball is seen, go to 'targetTracking' or 'activeTracking'.
    """
    if tracker.target.vis.frames_on > constants.TRACKER_FRAMES_ON_TRACK_THRESH:
        request = tracker.brain.interface.motionRequest
        request.type = request.RequestType.STOP_HEAD
        request.processed_by_motion = False
        if tracker.activeLocOn:
            return tracker.goNow('activeTracking')
        else:
            return tracker.goNow('targetTracking')

    elif tracker.stateTime >= TIME_TO_LOOK_TO_TARGET:
        request = tracker.brain.interface.motionRequest
        request.type = request.RequestType.STOP_HEAD
        request.processed_by_motion = False
        return tracker.goLater('scanForTarget')

    tracker.helper.lookToPoint(tracker.target)

    return tracker.stay()

def scanForTarget(tracker):
    """
    Performs naive scan for target.
    If ball is seen, go to 'targetTracking' or 'activeTracking'.
    """
    if tracker.target.vis.frames_on > constants.TRACKER_FRAMES_ON_TRACK_THRESH:
        if tracker.activeLocOn:
            request = tracker.brain.interface.motionRequest
            request.type = request.RequestType.STOP_HEAD
            request.processed_by_motion = False
            return tracker.goNow('activeTracking')
        else:
            request = tracker.brain.interface.motionRequest
            request.type = request.RequestType.STOP_HEAD
            request.processed_by_motion = False
            return tracker.goNow('targetTracking')

    if not tracker.brain.motion.head_is_active:
        targetDist = tracker.target.distance

        if targetDist > HeadMoves.HIGH_SCAN_CLOSE_BOUND:
            tracker.helper.executeHeadMove(HeadMoves.HIGH_SCAN_BALL)

        elif ( targetDist > HeadMoves.MID_SCAN_CLOSE_BOUND and
               targetDist < HeadMoves.MID_SCAN_FAR_BOUND ):
            tracker.helper.executeHeadMove(HeadMoves.MID_UP_SCAN_BALL)

        else:
            tracker.helper.executeHeadMove(HeadMoves.LOW_SCAN_BALL)

    return tracker.stay()

def targetTracking(tracker):
    """
    Track the target via vision values.
    If target is lost, look to localization values.
    If that fails, use naive scans.
    """
    if tracker.firstFrame():
        request = tracker.brain.interface.motionRequest
        request.type = request.RequestType.STOP_HEAD
        request.processed_by_motion = False
        tracker.activeLocOn = False

    if tracker.target.vis.frames_off > constants.TRACKER_FRAMES_OFF_REFIND_THRESH:
        return tracker.goLater('lookToTarget')

    tracker.helper.trackObject()

    return tracker.stay()

# Fixed Pitch
# Super State
def trackBallFixedPitch(tracker):
    """
    Track the ball using vision values.
    If ball is lost, execute pans until found.
    """
    tracker.target = tracker.brain.ball

    if tracker.target.vis.frames_off <= \
            constants.TRACKER_FRAMES_OFF_REFIND_THRESH:
        return tracker.goNow('trackingFixedPitch')
    else:
        return tracker.goNow('fullPanFixedPitch')

# Fixed Pitch
def lookStraightThenTrackFixedPitch(tracker):
    """
    Perform a 'look straight' head move.
    Once ball is seen enough, track it.
    """
    if tracker.firstFrame():
        request = tracker.brain.interface.motionRequest
        request.type = request.RequestType.STOP_HEAD
        request.processed_by_motion = False
        tracker.helper.executeHeadMove(HeadMoves.FIXED_PITCH_LOOK_STRAIGHT)
        tracker.target = tracker.brain.ball

    if tracker.target.vis.frames_on > constants.TRACKER_FRAMES_ON_TRACK_THRESH:
        tracker.trackBallFixedPitch()

    return tracker.stay()
