from . import TrackingConstants as constants
import HeadMoves
from objects import RelLocation
import noggin_constants as NogginConstants
from ..playbook import PBConstants
from ..players import GoalieConstants

DEBUG = False

def tracking(tracker):
    """
    While the target is visible, track it via vision values.
    If the target is lost, switches to fullPan.
    """
    # If the target is not in vision, trackObjectFixedPitch will track via loc.
    tracker.helper.trackObject()

    if not tracker.target.vis.on and tracker.counter > 15:
        if DEBUG : tracker.printf("Missing object this frame",'cyan')
        if (tracker.target.vis.frames_off >
            constants.TRACKER_FRAMES_OFF_REFIND_THRESH):
            return tracker.goLater('fullPan')

    return tracker.stay()

def trackingFieldObject(tracker):
    tracker.helper.trackStationaryObject()
    if not tracker.target.on and tracker.counter > 15:
        if (tracker.target.frames_off >
            constants.TRACKER_FRAMES_OFF_REFIND_THRESH):
            return tracker.goLater('fullPan')

    return tracker.stay()

# Not currently used, but would be good functionality to have in the future.
def lookAtTarget(tracker):
    """Look to the localization coords of the stored target."""
    tracker.helper.lookAtTarget(tracker.target)
    return tracker.stay()

def lookStraightThenTrack(tracker):
    """
    Perform a 'look straight' head move.
    Once ball is seen enough, track it.

    Usually, this is used to lock the robot's
    head forward while it spins and searches
    for the ball.
    """
    if tracker.firstFrame():
        # Send the motion request message to stop
        request = tracker.brain.interface.motionRequest
        request.stop_head = True
        request.timestamp = int(tracker.brain.time * 1000)
        # Perform the head move to look straight ahead
        tracker.helper.executeHeadMove(HeadMoves.FIXED_PITCH_LOOK_STRAIGHT)
        # Make sure target is set right
        tracker.target = tracker.brain.ball

    if tracker.target.vis.frames_on > constants.TRACKER_FRAMES_ON_TRACK_THRESH:
        tracker.trackBall()

    return tracker.stay()

def fullPan(tracker):
    """
    Repeatedly executes the headMove FIXED_PITCH_PAN.
    Once the ball is located, switches to tracking.
    """
    if tracker.firstFrame():
        # Send the motion request message to stop
        request = tracker.brain.interface.motionRequest
        request.stop_head = True
        request.timestamp = int(tracker.brain.time * 1000)
        # Smartly start the pan
        tracker.helper.startingPan(HeadMoves.FIXED_PITCH_PAN)

    if not tracker.brain.motion.head_is_active:
        # Repeat the pan
        tracker.helper.executeHeadMove(HeadMoves.FIXED_PITCH_PAN)

    if tracker.target.on:
        return tracker.goLater('trackingFieldObject')

    if (tracker.target is tracker.brain.ball and
        tracker.brain.ball.vis.frames_on > constants.TRACKER_FRAMES_ON_TRACK_THRESH):
        return tracker.goLater('tracking')

    return tracker.stay()

def afterKickScan(tracker):
    """
    Looks in the direction the ball was kicked in.
    If the ball is seen, go to state 'ballTracking'.
    """
    if tracker.firstFrame():
        tracker.performHeadMove(constants.KICK_DICT[tracker.kickName])

    return tracker.stay()
