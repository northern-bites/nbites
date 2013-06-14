import DribbleConstants as constants
# import noggin_constants as NogginConstants
from math import fabs

def seesBall(player):
    """
    We see the ball. So go get it.
    """
    ball = player.brain.ball
    return (ball.vis.frames_on > constants.BALL_ON_THRESH)

def ballClose(player):
    """
    We're close to the ball.
    """
    ball = player.brain.ball
    return (ball.vis.frames_on > 4 and
            ball.distance < constants.PREPARE_FOR_KICK_DIST)

def shouldSpinToBall(player):
    """
    We're not facing the ball well enough yet.
    """
    ball = player.brain.ball
    return (ball.vis.on and
            fabs(ball.rel_y) > constants.SHOULD_SPIN_TO_BALL_Y and
            not (ball.distance > constants.SHOULD_SPIN_TO_BALL_DIST and
                 fabs(ball.bearing_deg) < constants.SHOULD_SPIN_TO_BALL_BEAR))

def shouldStopSpinning(player):
    """
    We're done spinning.
    """
    ball = player.brain.ball
    return (ball.vis.on and
            fabs(ball.rel_y) < constants.STOP_SPINNING_TO_BALL_Y)

def ballGotFarAway(player):
    """
    The ball got far away somehow.
    """
    ball = player.brain.ball
    return ball.vis.on and ball.distance > constants.BALL_FAR_AWAY

def itsBeenTooLong(player):
    """
    We've been in state too long.
    """
    return player.counter > 200

def ballNearUs(player):
    """
    Ball is around our feet.
    """
    ball = player.brain.ball
    return ((constants.SHOULD_KICK_AGAIN_CLOSE_X < ball.rel_x <
              constants.SHOULD_KICK_AGAIN_FAR_X) and
             fabs(ball.rel_y) < constants.SHOULD_KICK_AGAIN_Y)

def ballMoved(player):
    """
    Ball has moved away from where it was seen last.
    """
    ball = player.brain.ball
    ballBefore = player.ballBeforeApproach
    return (fabs(ball.x - ballBefore.x) > constants.BALL_MOVED_THR or
            fabs(ball.y - ballBefore.y) > constants.BALL_MOVED_THR)

def navDone(player):
    """
    Nav is done.
    """
    return player.brain.nav.isAtPosition()

def ballLost(player):
    """
    We lost the ball.
    """
    return (player.brain.ball.vis.frames_off > constants.BALL_OFF_THRESH)
