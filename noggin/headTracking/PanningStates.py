import man.motion.HeadMoves as HeadMoves
from . import TrackingConstants as constants

def findLandmark(tracker):
    #if we've definitely found the target, track it
    if tracker.target.framesOn >= constants.TRACKER_FRAMES_ON_TRACK_THRESH:
        tracker.activeLocOn = False
        return tracker.goNow('landmarkTracking')

    if not tracker.brain.motion.isHeadActive():
        #first, look to our best guess of where target is based on loc
        if tracker.counter < constants.LOOK_TO_TIME_TO_FIND:
            tracker.lookToLandmark(tracker.target)
        else:
            tracker.helper.executeHeadMove(HeadMoves.MID_UP_SCAN_BALL)
        #second, start looking to box around object?
        '''e.g.
        for i = 20 to 100, increment 20
        tracker.lookToPoint(target.x+i, target.y+i, 0)
        tracker.lookToPoint(target.x-i, target.y+i, 0)
        tracker.lookToPoint(target.x-i, target.y-i, 0)
        tracker.lookToPoint(target.x-i, target.y+i, 0)
        #separate state?
        '''

    return tracker.stay()

def scanBall(tracker):
    ball = tracker.brain.ball

    if tracker.target == ball and \
            tracker.target.framesOn >= constants.TRACKER_FRAMES_ON_TRACK_THRESH:
        tracker.activeLocOn = False
        return tracker.goNow('ballTracking')


    if not tracker.brain.motion.isHeadActive():
        ballDist = ball.locDist

        if ballDist > HeadMoves.HIGH_SCAN_CLOSE_BOUND:
            tracker.helper.executeHeadMove(HeadMoves.HIGH_SCAN_BALL)

        elif ballDist > HeadMoves.MID_SCAN_CLOSE_BOUND and \
                ballDist < HeadMoves.MID_SCAN_FAR_BOUND:
            tracker.helper.executeHeadMove(HeadMoves.MID_UP_SCAN_BALL)
        else:
            tracker.helper.executeHeadMove(HeadMoves.LOW_SCAN_BALL)
    return tracker.stay()

def scanning(tracker):
    if (tracker.firstFrame() or not
        tracker.brain.motion.isHeadActive()):
        tracker.activeLocOn = False
        tracker.helper.executeHeadMove(tracker.currentHeadScan)
    return tracker.stay()

def locPans(tracker):
    if tracker.firstFrame() \
            or not tracker.brain.motion.isHeadActive():
        tracker.activeLocOn = False
        tracker.helper.executeHeadMove(HeadMoves.QUICK_PANS)
    return tracker.stay()

def panLeftOnce(tracker):
    if tracker.firstFrame():
        tracker.brain.motion.stopHeadMoves()
        tracker.helper.panTo(HeadMoves.PAN_LEFT_HEADS)
        return tracker.stay()

    if not tracker.brain.motion.isHeadActive():
        return tracker.goLater(tracker.lastDiffState)
    return tracker.stay()

def panRightOnce(tracker):
    if tracker.firstFrame():
        tracker.brain.motion.stopHeadMoves()
        tracker.helper.panTo(HeadMoves.PAN_RIGHT_HEADS)
        return tracker.stay()

    if not tracker.brain.motion.isHeadActive():
        return tracker.goLater(tracker.lastDiffState)
    return tracker.stay()

def panUpOnce(tracker):
    if tracker.firstFrame():
        tracker.brain.motion.stopHeadMoves()
        tracker.helper.panTo(HeadMoves.PAN_UP_HEADS)
        return tracker.stay()

    if not tracker.brain.motion.isHeadActive():
        return tracker.goLater(tracker.lastDiffState)
    return tracker.stay()

def postScan(tracker):
    if tracker.firstFrame() \
            or not tracker.brain.motion.isHeadActive():
        tracker.activeLocOn = False
        tracker.helper.executeHeadMove(HeadMoves.POST_SCAN)
    return tracker.stay()

def activeLocScan(tracker):
    if tracker.target.on:
        return tracker.goLater('activeTracking')

    if tracker.firstFrame() \
            or not tracker.brain.motion.isHeadActive():
        if tracker.brain.player.squatting:
            tracker.helper.executeHeadMove(HeadMoves.SQUAT_LOW_SCAN_BALL)
        else:
            tracker.helper.executeHeadMove(HeadMoves.MID_UP_SCAN_BALL)

    return tracker.stay()

def returnHeadsPan(tracker):
    """
    Return the heads to pre-active pan position
    """
    if tracker.firstFrame():
        tracker.brain.motion.stopHeadMoves()
        tracker.helper.panTo(tracker.preActivePanHeads)
        return tracker.stay()

    if not tracker.brain.motion.isHeadActive() or \
            tracker.target.on:
        tracker.helper.trackObject()
        return tracker.goLater(tracker.lastDiffState)
    return tracker.stay()

def look(tracker):
    """down, right, up, left"""
    if tracker.firstFrame():
        heads = HeadMoves.LOOK_HEADS[tracker.lookDirection]
        tracker.helper.panTo(heads)
        return tracker.stay()
    if tracker.brain.ball.on:
        return tracker.goNow('ballTracking')
    return tracker.stay()
