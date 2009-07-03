import man.motion.HeadMoves as HeadMoves
from . import TrackingConstants as constants


def scanBall(tracker):
    ball = tracker.brain.ball

    if tracker.target == ball and \
            tracker.target.framesOn >= constants.TRACKER_FRAMES_ON_TRACK_THRESH:
        tracker.activeLocOn = False
        return tracker.goNow('ballTracking')


    if not tracker.brain.motion.isHeadActive():
        ballDist = ball.locDist

        if ballDist > HeadMoves.HIGH_SCAN_CLOSE_BOUND:
            tracker.execute(HeadMoves.HIGH_SCAN_BALL)

        elif ballDist > HeadMoves.MID_SCAN_CLOSE_BOUND and \
                ballDist < HeadMoves.MID_SCAN_FAR_BOUND:
            tracker.execute(HeadMoves.MID_UP_SCAN_BALL)
        else:
            tracker.execute(HeadMoves.LOW_SCAN_BALL)
    return tracker.stay()

def scanning(tracker):
    if (tracker.firstFrame() or not
        tracker.brain.motion.isHeadActive()):
        tracker.activeLocOn = False
        tracker.execute(tracker.currentHeadScan)
    return tracker.stay()

def locPans(tracker):
    if tracker.firstFrame() \
            or not tracker.brain.motion.isHeadActive():
        tracker.activeLocOn = False
        tracker.execute(HeadMoves.QUICK_PANS)
    return tracker.stay()

def ballScan(tracker):
    if tracker.firstFrame() \
            or not tracker.brain.motion.isHeadActive():
        tracker.activeLocOn = False
        tracker.execute(HeadMoves.HIGH_SCAN_BALL)
    return tracker.stay()

def panLeftOnce(tracker):
    if tracker.firstFrame():
        tracker.brain.motion.stopHeadMoves()
        tracker.panTo(HeadMoves.PAN_LEFT_HEADS)
        return tracker.stay()

    if not tracker.brain.motion.isHeadActive():
        return tracker.goLater(tracker.lastDiffState)
    return tracker.stay()

def panRightOnce(tracker):
    if tracker.firstFrame():
        tracker.brain.motion.stopHeadMoves()
        tracker.panTo(HeadMoves.PAN_RIGHT_HEADS)
        return tracker.stay()

    if not tracker.brain.motion.isHeadActive():
        return tracker.goLater(tracker.lastDiffState)
    return tracker.stay()

def panUpOnce(tracker):
    if tracker.firstFrame():
        tracker.brain.motion.stopHeadMoves()
        tracker.panTo(HeadMoves.PAN_UP_HEADS)
        return tracker.stay()

    if not tracker.brain.motion.isHeadActive():
        return tracker.goLater(tracker.lastDiffState)
    return tracker.stay()

def postScan(tracker):
    if tracker.firstFrame() \
            or not tracker.brain.motion.isHeadActive():
        tracker.activeLocOn = False
        tracker.execute(HeadMoves.POST_SCAN)
    return tracker.stay()

def activeLocScan(tracker):
    if tracker.target.on:
        return tracker.goLater('activeTracking')

    if tracker.firstFrame() \
            or not tracker.brain.motion.isHeadActive():
        if tracker.brain.player.squatting:
            tracker.execute(HeadMoves.SQUAT_LOW_SCAN_BALL)
        else:
            tracker.execute(HeadMoves.MID_UP_SCAN_BALL)

    return tracker.stay()

def returnHeadsPan(tracker):
    """
    Return the heads to pre-active pan position
    """
    if tracker.firstFrame():
        tracker.brain.motion.stopHeadMoves()
        tracker.panTo(tracker.preActivePanHeads)
        return tracker.stay()

    if not tracker.brain.motion.isHeadActive() or \
            tracker.target.on:
        tracker.trackObject()
        return tracker.goLater(tracker.lastDiffState)
    return tracker.stay()

def look(tracker):
    if tracker.firstFrame():
        heads = HeadMoves.LOOK_HEADS[tracker.lookDirection]
        tracker.panTo(heads)
        return tracker.stay()
    if tracker.brain.ball.on:
        return tracker.goNow('ballTracking')
    return tracker.stay()
