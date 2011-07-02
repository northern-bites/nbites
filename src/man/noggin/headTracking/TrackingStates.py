from man.motion import MotionConstants
from . import TrackingConstants as constants

DEBUG = False

def ballTracking(tracker):
    '''Super state which handles following/refinding the ball'''
    if tracker.target.vis.framesOff <= \
            constants.TRACKER_FRAMES_OFF_REFIND_THRESH:
        return tracker.goNow('tracking')
    else:
        return tracker.goNow('scanBall')

def tracking(tracker):
    """
    While the target is visible, track it via vision values.
    If the ball is lost, go to last diff state.
    """

    if tracker.firstFrame():
        tracker.activeLocOn = False

    if tracker.target.dist > constants.ACTIVE_TRACK_DIST:
        return tracker.goLater('activeTracking')

    tracker.helper.trackObject()

    if not tracker.target.vis.on:
        if DEBUG : tracker.printf("Missing object this frame",'cyan')
        if tracker.target.vis.framesOff > \
                constants.TRACKER_FRAMES_OFF_REFIND_THRESH:
            return tracker.goLater(tracker.lastDiffState)
        return tracker.stay()

    return tracker.stay()

def ballSpinTracking(tracker):
    '''Super state which handles following/refinding the ball'''
    if tracker.target.vis.framesOff <= \
            constants.TRACKER_FRAMES_OFF_REFIND_THRESH:
        return tracker.goNow('tracking')
    else:
        return tracker.goNow('spinScanBall')

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

    tracker.helper.trackObject()

    # If we are close to the ball and have seen it consistently
    if tracker.target.dist < constants.STARE_TRACK_DIST:
        return tracker.goLater('tracking')

    if tracker.target.vis.framesOff > \
            constants.TRACKER_FRAMES_OFF_REFIND_THRESH:
        return tracker.goLater('activeLocScan')

    elif tracker.counter >= constants.BALL_ON_ACTIVE_PAN_THRESH and \
            tracker.target.vis.on:
        return tracker.goLater('trianglePan')

    return tracker.stay()
