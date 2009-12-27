from man.motion import MotionConstants
from . import TrackingConstants as constants
DEBUG = False

def stopped(tracker):
    '''default state where the tracker does nothing'''
    tracker.activeLocOn = False
    return tracker.stay()

def stop(tracker):
    ''' stop all head moves '''
    if tracker.firstFrame():
        tracker.activeLocOn = False
        tracker.brain.motion.stopHeadMoves()

    if not tracker.brain.motion.isHeadActive():
        return tracker.goLater('stopped')

    return tracker.stay()

def ballTracking(tracker):
    '''Super state which handles following/refinding the ball'''
    if tracker.target.framesOff <= constants.TRACKER_FRAMES_OFF_REFIND_THRESH:
        return tracker.goNow('tracking')
    else:
        return tracker.goNow('scanBall')

def landmarkTracking(tracker):
    '''Super state which handles following/refinding the landmark'''
    if tracker.target.framesOff <= constants.TRACKER_FRAMES_OFF_REFIND_THRESH:
        return tracker.goNow('tracking')
    else:
        return tracker.goNow('findLandmark')


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


def activeTracking(tracker):
    """
    Method to perform tracking
    """
    if tracker.firstFrame():
        tracker.activeLocOn = True

    if tracker.target.framesOff > constants.TRACKER_FRAMES_OFF_REFIND_THRESH \
            and not tracker.brain.motion.isHeadActive() \
            and not (tracker.activePanUp or tracker.activePanOut):
        return tracker.goLater('activeLocScan')

    elif not (tracker.activePanOut or tracker.activePanUp) and \
            tracker.counter <= constants.ACTIVE_LOC_STARE_THRESH:
        tracker.helper.trackObject()
        tracker.activePanOut = tracker.activePanUp = False
        return tracker.stay()

    elif tracker.activePanOut and tracker.activePanUp:
        tracker.activePanUp = tracker.activePanOut = False
        return tracker.goLater('returnHeadsPan')

    elif not tracker.activePanUp:
        tracker.activePanUp = True
        motionAngles = tracker.brain.sensors.motionAngles
        tracker.preActivePanHeads = (
            motionAngles[MotionConstants.HeadYaw],
            motionAngles[MotionConstants.HeadPitch])
        return tracker.goLater('panUpOnce')

    elif not tracker.activePanOut:
        tracker.activePanOut = True
        tracker.activePanDir = (tracker.activePanDir + 1) % \
            constants.NUM_ACTIVE_PANS

        if tracker.activePanDir == constants.PAN_RIGHT:
            return tracker.goLater('panRightOnce')
        elif tracker.activePanDir == constants.PAN_LEFT:
            return tracker.goLater('panLeftOnce')
    return tracker.stay()
