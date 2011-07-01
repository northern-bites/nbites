import man.motion.HeadMoves as HeadMoves
from . import TrackingConstants as constants
from man.motion import MotionConstants

# ** # old method
def scanBall(tracker):
    """
    While ball is not in view, execute naive pans.
    If ball is in view, go to last diff state.
    """
    ball = tracker.brain.ball
    if tracker.target == ball and \
            tracker.target.framesOn >= constants.TRACKER_FRAMES_ON_TRACK_THRESH:
        tracker.activeLocOn = False
        return tracker.goNow(tracker.lastDiffState)
    #Here we choose where to look for the ball first
    if not tracker.brain.motion.isHeadActive():
        if ball.dist > HeadMoves.HIGH_SCAN_CLOSE_BOUND:
            tracker.helper.executeHeadMove(HeadMoves.HIGH_WIDE_SCAN_BALL)
        elif ball.dist > HeadMoves.MID_SCAN_CLOSE_BOUND and \
                ball.dist < HeadMoves.MID_SCAN_FAR_BOUND:
            tracker.helper.executeHeadMove(HeadMoves.MID_DOWN_WIDE_SCAN_BALL)
        else:
            tracker.helper.executeHeadMove(HeadMoves.FULL_SCAN_BALL)

    # # Instead because our ball information is poor, lets just do one
    # # scan and make sure we don't miss it.  If our ball EKF is better
    # # and trustworthy than we can put the above code back in
    # if not tracker.brain.motion.isHeadActive():
    #     tracker.helper.executeHeadMove(HeadMoves.FULL_SCAN_BALL)
    return tracker.stay()

# ** # new method
def panScanForLoc(tracker):
    """
    Perform full scans until a corner or post is seen. Then, stare at that
    corner or post.
    """
    # Make sure head is stopped
    if tracker.firstFrame():
        tracker.brain.motion.stopHeadMoves()

    # If any visual object is sighted, stare at it
    for obj in tracker.locObjectList:
        if obj.on:
            print "target found:",obj.visionId
            tracker.target = obj
            return tracker.goLater('stareLoc')

    # Nothing on frame? Look around
    if not tracker.brain.motion.isHeadActive():
        tracker.helper.executeHeadMove(HeadMoves.HIGH_WIDE_SCAN_BALL)

    return tracker.stay()

# ** # new method
def scanForPost(tracker):
    """
    Perform high wide scans until a post is seen. Then, stare at that post.
    """
    # Make sure that head is inactive
    if tracker.firstFrame():
        tracker.brain.motion.stopHeadMoves()

    # Do a high wide scan for a post
    if not tracker.brain.motion.isHeadActive():
        tracker.helper.executeHeadMove(HeadMoves.HIGH_WIDE_SCAN_BALL)

    # Check for posts.
    for obj in tracker.locObjectList:
        if obj.on and obj.visionId > 19:
            tracker.target = obj
            print "found a post:",obj.visionId
            return tracker.goLater('stareLoc')

    # Check search threshold
    if tracker.counter > constants.TRACKER_FRAMES_STARE_THRESH:
        print "Past search thresh, switching to new target"
        return tracker.goLater(tracker.decisionState)

    return tracker.stay()

# ** # old method
def spinScanBall(tracker):
    """
    If ball is in view, goes to state cycle decider.
    If ball is not in view, execute pans based on direction robot is spinning.
    """
    ball = tracker.brain.ball
    nav = tracker.brain.nav

    if tracker.target == ball and \
            tracker.target.framesOn >= constants.TRACKER_FRAMES_ON_TRACK_THRESH:
        tracker.activeLocOn = False
        return tracker.goNow('ballSpinTracking')

    if nav.walkTheta > 0:
        tracker.headMove = HeadMoves.LEFT_EDGE_SCAN_BALL
    else:
        tracker.headMove = HeadMoves.RIGHT_EDGE_SCAN_BALL

    if not tracker.brain.motion.isHeadActive():
        tracker.helper.executeHeadMove(tracker.headMove)

    return tracker.stay()

# ** # new method
def spinningPan(tracker):
    nav = tracker.brain.nav

    if nav.walkTheta > 0:
        tracker.headMove = HeadMoves.SPIN_LEFT_SCAN_BALL
    else:
        tracker.headMove = HeadMoves.SPIN_RIGHT_SCAN_BALL

    if not tracker.brain.motion.isHeadActive():
        tracker.helper.executeHeadMove(tracker.headMove)

    return tracker.stay()

# ** # old method
def scanning(tracker):
    if tracker.firstFrame():
        tracker.brain.motion.stopHeadMoves()
        tracker.helper.executeHeadMove(tracker.currentHeadScan)

    if not tracker.brain.motion.isHeadActive():
        tracker.activeLocOn = False
        tracker.helper.executeHeadMove(tracker.currentHeadScan)

    return tracker.stay()

# ** # old method
def locPans(tracker):
    if tracker.firstFrame():
        tracker.brain.motion.stopHeadMoves()
        tracker.helper.executeHeadMove(HeadMoves.QUICK_PANS)

    if not tracker.brain.motion.isHeadActive():
        tracker.activeLocOn = False
        tracker.helper.executeHeadMove(HeadMoves.QUICK_PANS)

    return tracker.stay()

# ** # new method
def checkBall(tracker):
    """
    If ball not on frame, look to current ball coordinates.
    If ball is on frame, track it via angles.
    """
    # Stop head moves and set target.
    if tracker.firstFrame():
        tracker.brain.motion.stopHeadMoves()
        tracker.target = tracker.brain.ball

    # Check for time in state.
    if tracker.counter > TRACKER_BALL_STARE_THRESH:
        return tracker.goLater(tracker.decisionState)

    # Look to ball.
    if tracker.target.framesOn > constants.TRACKER_FRAMES_ON_TRACK_THRESH:
        tracker.helper.lookToTargetAngles()
    elif tracker.target.framesOff > constants.TRACKER_FRAMES_OFF_REFIND_THRESH:
        tracker.helper.lookToTargetCoords()
    return tracker.stay()

def kickWhiffCheck(tracker):
    """
    After a kick was executed and the ball was lost,
    look down and check for a whiff.
    """
    if tracker.firstFrame():
        tracker.brain.motion.stopHeadMoves()
        tracker.helper.executeHeadMove(HeadMoves.LOOK_DOWN)
        tracker.target = tracker.brain.ball
        return tracker.stay()

    if tracker.target.framesOn() > constants.TRACKER_FRAMES_ON_TRACK_THRESH:
        # Ball located, stare at it.
        tracker.stareBall()
        return tracker.stay()

    if not tracker.brain.motion.isHeadActive():
        # Ball was not whiffed. Pan in kick direction.
        return tracker.goLater(followKickPan)

def followKickPan(tracker):
    """
    Pan in direction of kick. For side kicks, start at feet and pan out
    from there. For straight kicks, ball was probably lost because it was
    moving too fast, so start far out and pan back. For back kicks, don't
    enter this state at all.
    If ball is located, stare at it.
    NOTE: Set tracker.kickDirection to constants.LOOK_UP, etc.
    """
    if tracker.firstFrame():
        tracker.target = tracker.brain.ball
        tracker.brain.motion.stopHeadMoves()
        if tracker.kickDirection == constants.LOOK_LEFT:
            pass # ** # Do left pan # ** #
        elif tracker.kickDirection == constants.LOOK_RIGHT:
            pass # ** # Do right pan # ** #
        elif tracker.kickDirection == constants.LOOK_UP:
            pass # ** # Do forward pan # ** #

    # Check if ball was seen.
    if tracker.target.framesOn > constants.TRACKER_FRAMES_ON_TRACK_THRESH:
        tracker.stareBall()

    return tracker.stay()

# ** # old method
def panLeftOnce(tracker):
    if tracker.firstFrame():
        tracker.brain.motion.stopHeadMoves()
        tracker.helper.panTo(HeadMoves.PAN_LEFT_HEADS)
        return tracker.stay()

    if not tracker.brain.motion.isHeadActive():
        return tracker.goLater(tracker.lastDiffState)
    return tracker.stay()

# ** # old method
def panRightOnce(tracker):
    if tracker.firstFrame():
        tracker.brain.motion.stopHeadMoves()
        tracker.helper.panTo(HeadMoves.PAN_RIGHT_HEADS)
        return tracker.stay()

    if not tracker.brain.motion.isHeadActive():
        return tracker.goLater(tracker.lastDiffState)
    return tracker.stay()

# ** # old method
def panUpOnce(tracker):
    if tracker.firstFrame():
        tracker.brain.motion.stopHeadMoves()
        tracker.helper.panTo(HeadMoves.PAN_UP_HEADS)
        return tracker.stay()

    elif not tracker.brain.motion.isHeadActive():
        return tracker.goLater(tracker.lastDiffState)
    return tracker.stay()

# ** # old method
def postScan(tracker):
    if tracker.firstFrame() \
            or not tracker.brain.motion.isHeadActive():
        tracker.activeLocOn = False
        tracker.helper.executeHeadMove(HeadMoves.POST_SCAN)
    return tracker.stay()

# ** # old method
def activeLocScan(tracker):
    """
    Execute naive mid-height scans.
    If ball is visible, return to state 'activeTracking'.
    """
    if tracker.target.on:
        return tracker.goLater('activeTracking')

    if tracker.firstFrame() \
            or not tracker.brain.motion.isHeadActive():
        tracker.helper.executeHeadMove(HeadMoves.MID_UP_SCAN_BALL)

    return tracker.stay()

# ** # old method
def returnHeadsPan(tracker):
    """
    Return the head angles to pre-active pan position.
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

# ** # old method
def look(tracker):
    """down, right, up, left"""
    if tracker.firstFrame():
        tracker.brain.motion.stopHeadMoves()
        heads = HeadMoves.LOOK_HEADS[tracker.lookDirection]
        tracker.helper.panTo(heads)
        return tracker.stay()
    if tracker.brain.ball.on:
        return tracker.goNow('ballTracking')
    return tracker.stay()

# ** # old method
def scanQuickUp(tracker):
    if tracker.firstFrame():
        tracker.isPreKickScanning = True

        # always true...
        if tracker.lastDiffState != 'panUpOnce' or \
                tracker.lastDiffState != 'returnHeadsPan':
            tracker.activePanUp = False
            tracker.scanningUp = False
            motionAngles = tracker.brain.sensors.motionAngles
            tracker.preActivePanHeads = (
                motionAngles[MotionConstants.HeadYaw],
                motionAngles[MotionConstants.HeadPitch])


    if tracker.activePanUp and \
            not tracker.scanningUp:
        tracker.activePanUp = False
        tracker.isPreKickScanning = False
        return tracker.goLater('stop')

    elif tracker.activePanUp and \
            tracker.scanningUp:
        tracker.scanningUp = False
        return tracker.goNow('returnHeadsPan')

    else:
        tracker.scanningUp = True
        tracker.activePanUp = True
        return tracker.goNow('panUpOnce')


MOTION_START_BUFFER = 2

# ** # old method
# anything that calls this should make sure that
# goalieActiveLoc is set to proper value ( most likely false)
def trianglePan(tracker):
    """
    Execute either a left or right scan. Then, return to original
    head angles.
    """
    motionAngles = tracker.brain.sensors.motionAngles
    tracker.preTriPanHeads = (
        motionAngles[MotionConstants.HeadYaw],
        motionAngles[MotionConstants.HeadPitch]
        )

    if tracker.brain.sensors.motionAngles[MotionConstants.HeadYaw] > 0:
        return tracker.goNow('trianglePanLeft')
    else :
        return tracker.goNow('trianglePanRight')

# ** # old method
def trianglePanLeft(tracker):
    if tracker.firstFrame():
        if tracker.goalieActiveLoc:
            tracker.helper.panTo(HeadMoves.PAN_LEFT_SHOULDER_HEADS)
        else:
            tracker.helper.executeHeadMove(HeadMoves.POST_LEFT_SCAN)

    elif not tracker.brain.motion.isHeadActive() and \
            tracker.counter > MOTION_START_BUFFER:
        return tracker.goLater('trianglePanReturn')

    return tracker.stay()

# ** # old method
def trianglePanRight(tracker):
    if tracker.firstFrame():
        if tracker.goalieActiveLoc:
            tracker.helper.panTo(HeadMoves.PAN_RIGHT_SHOULDER_HEADS)
        else:
            tracker.helper.executeHeadMove(HeadMoves.POST_RIGHT_SCAN)

    elif not tracker.brain.motion.isHeadActive() and \
            tracker.counter > MOTION_START_BUFFER:
        return tracker.goLater('trianglePanReturn')
    return tracker.stay()

# ** # old method
def trianglePanReturn(tracker):
    if tracker.firstFrame():
        # TODO! Should be look to ball.
        tracker.helper.panTo(tracker.preTriPanHeads)
    elif (not tracker.brain.motion.isHeadActive() and
          tracker.counter > MOTION_START_BUFFER)  or \
          tracker.target.on:
        return tracker.goLater('ballTracking')
    return tracker.stay()

# ** # old method
def bounceUp(tracker):
    if tracker.firstFrame():
        tracker.helper.panTo(HeadMoves.PAN_UP_HEADS)
    elif not tracker.brain.motion.isHeadActive():
        return tracker.goLater('bounceDown')
    return tracker.stay()

# ** # old method
def bounceDown(tracker):
    if tracker.firstFrame():
        tracker.helper.panTo(HeadMoves.PAN_DOWN_HEADS)
    elif not tracker.brain.motion.isHeadActive():
        return tracker.goLater('bounceUp')
    return tracker.stay()

