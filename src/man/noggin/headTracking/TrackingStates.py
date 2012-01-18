from . import TrackingConstants as constants
from objects import RelLocation
import noggin_constants as NogginConstants
from ..playbook import PBConstants
from ..players import GoalieConstants

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

    if tracker.brain.play.isRole(PBConstants.GOALIE):
        minActiveDist = GoalieConstants.ACTIVE_LOC_THRESH
    else:
        minActiveDist = constants.ACTIVE_TRACK_DIST

    # I think that not having the right distance
    # might be the problem on tracking
    # TODO: Check ball information problem
    #       tracker.printf(tracker.target.loc.dist);
    if tracker.target.loc.dist > minActiveDist:
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
    If state counter is low enough (< 45), track ball via vision values.
    If state counter is high enough, perform triangle pans
    and return to last head angles.
    """
    if tracker.firstFrame():
        tracker.shouldStareAtBall = 0
        tracker.activeLocOn = True

    tracker.helper.trackObject()

    # If we are close to the ball and have seen it consistently
    if tracker.target.loc.dist < constants.STARE_TRACK_DIST:
        tracker.shouldStareAtBall += 1

        if tracker.shouldStareAtBall > constants.STARE_TRACK_THRESH:
            return tracker.goLater('tracking')
    else:
        tracker.shouldStareAtBall = 0

    if tracker.target.vis.framesOff > \
            constants.TRACKER_FRAMES_OFF_REFIND_THRESH and \
            tracker.counter > constants.TRACKER_FRAMES_OFF_REFIND_THRESH:
        return tracker.goLater('activeLocScan')

    elif tracker.counter >= constants.BALL_ON_ACTIVE_PAN_THRESH and \
            tracker.target.vis.on:
        if tracker.brain.play.isRole(PBConstants.GOALIE):
            return tracker.goLater('trianglePan')
        if tracker.brain.my.locScore == NogginConstants.locScore.GOOD_LOC:
            return tracker.goLater('panToFieldObject')
        else:
            return tracker.goLater('trianglePan')

    return tracker.stay()

def panToFieldObject(tracker):
    """
    Calculate which goalpost is easiest to look at and look to it.
    After we look at it for a bit, look back at target.
    """
    # Calculate closest field object
    if tracker.firstFrame():
        tracker.shouldStareAtBall = 0 # Reuse this counter

        closest = tracker.helper.calculateClosestLandmark()

        # For some reason, we aren't going to look at anything, so go
        # back to tracking
        if closest is None:
            return tracker.goLater('activeTracking')

        if hasattr(closest, "loc"):
            closest = closest.loc

        target = RelLocation(tracker.brain.my, closest.x, closest.y, 0)
        target.height = 45      # stare at the center of the post
                                # rather than the bottom

        tracker.lastMove = tracker.helper.lookToPoint(target)

    elif tracker.lastMove.isDone():
        tracker.shouldStareAtBall += 1

        if tracker.shouldStareAtBall > constants.LOOK_FIELD_OBJ_FRAMES:
            return tracker.goLater('activeTracking')

    return tracker.stay()
