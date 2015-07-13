from . import TrackingConstants as constants
import HeadMoves
import noggin_constants as NogginConstants
import Vision_proto as Vision
import math

#TODO: if targets are messed up, insert 'target = tracker.brain.ball'
#in every function

DEBUG = False

def tracking(tracker):
    """
    While the target is visible, track it via vision values.
    If the target is lost, switches to altTrackSharedBallAndPan if shared ball
    is reliable and fullPan if shared ball is not realiable.
    """
    # makes sure ball is default target when entering tracking
    tracker.target = tracker.brain.ball

    # If the target is not in vision, trackObjectFixedPitch will track via loc.
    tracker.helper.trackObject()

    # If cannot see ball and shared ball is reliable, go to altTrackSharedBallAndPan
    # if (tracker.target.vis.frames_off > 15  and tracker.brain.sharedBall.ball_on and
    #     tracker.brain.sharedBall.reliability >= 2 and tracker.counter > 15):
    #     return tracker.goLater('altTrackSharedBallAndPan')

    if not tracker.target.vis.on and tracker.counter > 15:
        if DEBUG : tracker.printf("Missing object this frame",'cyan')
        if (tracker.target.vis.frames_off >
            constants.TRACKER_FRAMES_OFF_REFIND_THRESH):
            return tracker.goLater('fullPan')

    return tracker.stay()

def bounceTracking(tracker):
    """
    Just like the above, but using a different tracking method.
    """
    tracker.helper.bounceTrackBall()

    if tracker.target.vis.frames_off > constants.TRACKER_FRAMES_OFF_REFIND_THRESH:
        return tracker.goLater('bounceFullPan')

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

#TODO use constants
def snapToCorner(tracker):
    """ Snap to a corner to help localization """
    lookTime = 5

    if snapToCorner.count >= 0 and snapToCorner.count < lookTime:
        tracker.target.x = NogginConstants.FIELD_WHITE_WIDTH
        tracker.target.y = 0.0
    if snapToCorner.count >= lookTime and snapToCorner.count < 2*lookTime:
        tracker.target.x = 0.0
        tracker.target.y = NogginConstants.FIELD_WHITE_HEIGHT
    if snapToCorner.count >= 2*lookTime and snapToCorner.count < 3*lookTime:
        tracker.target.x = NogginConstants.FIELD_WHITE_WIDTH
        tracker.target.y = NogginConstants.FIELD_WHITE_HEIGHT
    if snapToCorner.count >= 3*lookTime and snapToCorner.count < 4*lookTime:
        tracker.target.x = 0.0
        tracker.target.y = 0.0

    heading = tracker.brain.loc.h
    robx = tracker.brain.loc.x
    roby = tracker.brain.loc.y
    yaw = math.degrees(math.atan((tracker.target.y-roby)/(tracker.target.x-robx)))
    if robx > tracker.target.x:
        if roby < tracker.target.y:
            yaw = 180 + yaw
        else:
            yaw = -180 + yaw
    yaw = yaw - heading

    # only look to corner if head is finished moving and corner is not behind robot
    if not tracker.brain.motion.head_is_active and yaw < 110 and yaw > -110:
        tracker.helper.executeHeadMove(lookToAngle(yaw))

    if not tracker.brain.motion.head_is_active:
        snapToCorner.count = snapToCorner.count + 1
        if snapToCorner.count > 4*lookTime:
            snapToCorner.count = 1

    return tracker.stay()

snapToCorner.count = 0

def altTrackSharedBallAndPan(tracker):
    # Alternates between track shared ball and a full pan
    altTrackSharedBallAndPan.currentlyOn # 0 for look at shared ball, 1 for full pan
    altTrackSharedBallAndPan.timeCounter
    tracker.target = tracker.brain.sharedBall

    # If shared ball is on, look to shared ball for amount of time determined by 'lookTime'
    if altTrackSharedBallAndPan.currentlyOn == 0:
        if not tracker.brain.motion.head_is_active:
            if tracker.target.ball_on:
                tracker.helper.lookToXY(tracker.target.x, tracker.target.y)
            altTrackSharedBallAndPan.timeCounter = altTrackSharedBallAndPan.timeCounter + 1

    # Automatically pan after a certain amount of time looking at shared ball, and also
    # if shared ball is not on, for set amount of time determined by 'panTime'
    if altTrackSharedBallAndPan.currentlyOn == 1 or (altTrackSharedBallAndPan.currentlyOn == 0
                                                     and not tracker.target.ball_on):
        if not tracker.brain.motion.head_is_active:
            tracker.helper.executeHeadMove(HeadMoves.FIXED_PITCH_PAN)
            altTrackSharedBallAndPan.timeCounter = altTrackSharedBallAndPan.timeCounter + 1

    if altTrackSharedBallAndPan.timeCounter >= 0 and altTrackSharedBallAndPan.timeCounter < constants.LOOKTIME:
        altTrackSharedBallAndPan.currentlyOn = 0
    if altTrackSharedBallAndPan.timeCounter >= constants.LOOKTIME:
        altTrackSharedBallAndPan.currentlyOn = 1
    if altTrackSharedBallAndPan.timeCounter >= constants.PANTIME + constants.LOOKTIME:
        altTrackSharedBallAndPan.timeCounter = 0

    #If ever see the ball, immediately go to back to tracking
    if tracker.brain.ball.vis.on:
        return tracker.goLater('tracking')

    return tracker.stay()

altTrackSharedBallAndPan.currentlyOn = 0
altTrackSharedBallAndPan.timeCounter = 0

def trackSharedBall(tracker):
    tracker.target = tracker.brain.sharedBall

    if tracker.target.ball_on:
        tracker.helper.lookToXY(tracker.target.x, tracker.target.y)

    if tracker.brain.ball.vis.on:
        return tracker.goLater('tracking')
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

    if not isinstance(tracker.target, Vision.messages.FilteredBall):
        if tracker.target.on:
            return tracker.goLater('trackingFieldObject')

    if (isinstance(tracker.target, Vision.messages.FilteredBall) and
        tracker.brain.ball.vis.frames_on > constants.TRACKER_FRAMES_ON_TRACK_THRESH):
        return tracker.goLater('tracking')

    return tracker.stay()

def bounceFullPan(tracker):
    """
    Just like above, but matches with the bounceTracking state instead.
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

    if not isinstance(tracker.target, Vision.messages.FilteredBall):
        if tracker.target.on:
            return tracker.goLater('trackingFieldObject')

    if (isinstance(tracker.target, Vision.messages.FilteredBall) and
        tracker.brain.ball.vis.frames_on > constants.TRACKER_FRAMES_ON_TRACK_THRESH):
        return tracker.goLater('bounceTracking')

    return tracker.stay()

def afterKickScan(tracker):
    """
    Looks in the direction the ball was kicked in.
    If the ball is seen, go to state 'ballTracking'.
    """
    if tracker.firstFrame():
        tracker.performHeadMove(constants.KICK_DICT[tracker.kickName])

    return tracker.stay()

def trackObstacle(tracker):
    """
    Looks in the direction the ball was kicked in.
    If the ball is seen, go to state 'ballTracking'.
    """
    if tracker.firstFrame():
        tracker.performHeadMove(constants.OBSTACLE_DICT[tracker.obstacleDir])

    if tracker.brain.ball.vis.frames_on >= 1:
        return tracker.goLater('trackBall')

    return tracker.stay()
