import noggin_constants as nogginConstants
from math import fabs

### THE BALL
def ballOnForNFrames(player, atleastThisManyFrames):
    """
    We have seen the ball for N frames.
    """
    ball = player.brain.ball
    return (ball.vis.frames_on > atleastThisManyFrames)

def ballOffForNFrames(player, atleastThisManyFrames):
    """
    We have not seen the ball for N frames.
    """
    ball = player.brain.ball
    return (ball.vis.frames_off > atleastThisManyFrames)

def ballForNFramesAndMClose(player, atleastThisManyFrames, atleastThisClose):
    """
    We have seen the ball for N frames and it is M centimeters away or closer.

    NOTE: This is not a CountTransition, see util/Transitions.py if you want
    a rolling fiter of predicate checks.
    """
    ball = player.brain.ball
    return (ballOnForNFrames(player, atleastThisManyFrames) and
            ball.distance < atleastThisClose)

def ballForNFramesAndMFar(player, atleastThisManyFrames, atleastThisFar):
    """
    We have seen the ball for N frames and it is M centimeters away or further.

    NOTE: This is not a CountTransition, see util/Transitions.py if you want
    a rolling fiter of predicate checks.
    """
    ball = player.brain.ball
    return (ballOnForNFrames(player, atleastThisManyFrames) and
            ball.distance > atleastThisFar)

def ballMovedNCMsFromWhereSeenLast(player, ballMovedTolerance):
    """
    The ball has moved from where it was last seen by at least N centimeters.

    @requires player.ballBeforeCheck to be set for comparision
    """
    ball = player.brain.ball
    ballBefore = player.ballBeforeCheck
    return (fabs(ball.rel_x - ballBefore.rel_x) > ballMovedTolerance or
            fabs(ball.rel_y - ballBefore.rel_y) > ballMovedTolerance)

### STATE-TIME/FRAME-COUNT
def sameStateForNFrames(player, numFrames):
    """
    We have been in the same state for N frames.

    NOTE: ~30 frames = 1 second, probably better to use sameStateForNSeconds
    """
    return (player.counter > numFrames)

def sameStateForNSeconds(player, numSeconds):
    """
    We have been in the same state for N seconds.
    """
    return (player.stateTime > numSeconds)

def noTimeLeft(player, seconds):
    """
    There are not more than N seconds left in the game.
    """
    return (player.brain.game.secs_remaining > seconds)

### ROBOT DETECTION
def centerLaneOpenForNCMs(player, atleastThisMuchGreen):
    """
    We have an open lane for N cms where I am looking.
    """
    return (player.brain.interface.visionObstacle.mid_dist > 
            atleastThisMuchGreen or
            player.brain.interface.visionObstacle.block_mid == 0)

def seesZeroRobots(player):
    """
    Vision sees zero robots in this frame.
    """
    vr = player.brain.interface.visionRobot
    return (not vr.red1.on and not vr.navy1.on)

### FIELD POSITION
def betweenCrosses(player):
    """
    We are between the two field crosses.
    """
    return (player.brain.loc.x > nogginConstants.LANDMARK_BLUE_GOAL_CROSS_X and
            player.brain.loc.x < nogginConstants.LANDMARK_YELLOW_GOAL_CROSS_X)

def middleThird(player):
    """
    We are in the middle third of the field.
    """
    field_len = nogginConstants.FIELD_WHITE_RIGHT_SIDELINE_X
    return (player.brain.loc.x > field_len / 3. and
            player.brain.loc.x < 2. * field_len / 3.)

def firstHalf(player):
    """
    We are between our field cross and midfield.
    """
    field_len = nogginConstants.FIELD_WHITE_RIGHT_SIDELINE_X
    return (player.brain.loc.x > nogginConstants.LANDMARK_BLUE_GOAL_CROSS_X and
            player.brain.loc.x < field_len / 2.)

def secondHalf(player):
    """
    We are between midfield and our opponent's field cross.
    """
    field_len = nogginConstants.FIELD_WHITE_RIGHT_SIDELINE_X
    return (player.brain.loc.x > field_len / 2. and
            player.brain.loc.x < nogginConstants.LANDMARK_YELLOW_GOAL_CROSS_X)

def navAtPosition(player):
    """
    Nav says we are at position
    """
    return player.brain.nav.isAtPosition()
