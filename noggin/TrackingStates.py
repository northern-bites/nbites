from man.motion import MotionConstants

DEBUG = False
TRACKER_FRAMES_OFF_REFIND_THRESH = 10 #num frms after which to switch to scanfindbl
ACTIVE_LOC_STARE_THRESH = 30
ACTIVE_LOC_OFF_REFIND_THRESH = 40

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

def ballTracking(tracker): #Super state which handles following/refinding the ball
    if tracker.target.framesOff <= TRACKER_FRAMES_OFF_REFIND_THRESH:
        return tracker.goNow('tracking')
    else:
        return tracker.goNow('scanBall')

def tracking(tracker):
    ''' state askes it's parent (the tracker) for an object or angles to track
    while the object is on screen, or angles are passed, we track it.
    Otherwise, we continually write the current values into motion via setHeads.

    If a sweet move is begun while we are tracking, the current setup is to let
    the sweet move conclude and then resume tracking afterward.'''

    if tracker.firstFrame():
        tracker.activeLocOn = False

    tracker.trackObject()
    if not tracker.target.on:
        if DEBUG : tracker.printf("Missing object this frame",'cyan')
        if tracker.target.framesOff > TRACKER_FRAMES_OFF_REFIND_THRESH:
            return tracker.goLater(tracker.lastDiffState)
        return tracker.stay()

    return tracker.stay()


def activeTracking(tracker):
    """
    Method to perform tracking
    """
    tracker.trackObject()
    tracker.activeLocOn = True

    if tracker.activePanOut:
        return tracker.goLater('returnHeadsPan')

    if tracker.target.framesOff > TRACKER_FRAMES_OFF_REFIND_THRESH and \
            not tracker.brain.motion.isHeadActive():
        return tracker.goLater('activeLocScan')

    if not tracker.activePanOut and \
            tracker.counter > ACTIVE_LOC_STARE_THRESH:

        tracker.activePanOut = True
        tracker.activePanDir = not tracker.activePanDir

        motionAngles = tracker.brain.sensors.motionAngles
        tracker.preActivePanHeads = (
            motionAngles[MotionConstants.HeadYaw],
            motionAngles[MotionConstants.HeadPitch])

        if tracker.activePanDir:
            return tracker.goLater('panRightOnce')
        else:
            return tracker.goLater('panLeftOnce')
    return tracker.stay()

