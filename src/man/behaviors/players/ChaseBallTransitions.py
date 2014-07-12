from .. import SweetMoves
from ..headTracker import HeadMoves
import ChaseBallConstants as constants
import noggin_constants as NogginConstants
import ClaimTransitions as claimTrans
from math import fabs, degrees

####### CHASING STUFF ##############

def shouldChaseBall(player):
    """
    We see the ball. So go get it.
    """
    ball = player.brain.ball
    return (ball.vis.frames_on > constants.BALL_ON_THRESH)

def shouldReturnHome(player):
    """
    The ball is no longer our responsibility. Go home.
    player.buffBoxFiltered is a CountTransition, see approachBall.

    If the ball IS in our box, check the claims for a higher priority claim
    """
    if player.buffBoxFiltered.checkCondition(player):
        player.claimedBall = False
        return True;

    return claimTrans.shouldCedeClaim(player)

def shouldSupport(player):
    """
    when in positionAndKickBall don't care if the ball is in our box but do
    want to check who has higher priority claim
    """
    return claimTrans.shouldCedeClaim(player)

def shouldPrepareForKick(player):
    """
    We're close enough to prepare for a kick
    """
    ball = player.brain.ball
    return (ball.vis.frames_on > 0 and
            ball.distance < constants.PREPARE_FOR_KICK_DIST)

def shouldSpinToBall(player):
    """
    We're not facing the ball well enough
    """
    return fabs(degrees(player.brain.ball.bearing)) > constants.SHOULD_SPIN_TO_BALL_BEARING

def shouldApproachBallAgain(player):
    """
    The ball got really far away somehow
    """
    ball = player.brain.ball
    return ball.vis.on and ball.distance > constants.APPROACH_BALL_AGAIN_DIST

def shouldRedecideKick(player):
    """
    We've been in position for kick too long
    """
    return player.counter > 200

def ballInPosition(player, kickPose):
    """
    Make sure ball is somewhere we will kick it. Also makes sure we're looking
    at the ball.
    """
    if not player.brain.ball.vis.on:
        return False

    # NOTE don't take the absolute value of kickPose.relX because being too
    # close to the ball is not a problem for kicking
    return (kickPose.relX < constants.BALL_X_OFFSET and
            fabs(kickPose.relY) < constants.BALL_Y_OFFSET and
            fabs(kickPose.relH) < constants.GOOD_ENOUGH_H)

def ballNearPosition(player):
    """
    Ball is around our feet. Maybe we wiffed?
    """
    ball = player.brain.ball
    return ((constants.SHOULD_KICK_AGAIN_CLOSE_X < ball.rel_x <
              constants.SHOULD_KICK_AGAIN_FAR_X) and
             fabs(ball.rel_y) < constants.SHOULD_KICK_AGAIN_Y)

def shouldKick(player):
    """
    Ball is in correct position to kick
    """
    return player.brain.nav.isAtPosition() and player.counter > 1

def shouldKickAgain(player):
    """
    Ball hasn't changed enough to warrant new kick decision.
    """
    return player.brain.ball.vis.on and ballNearPosition(player)

def shouldOrbit(player):
    """
    We are lost (no kick) but are chaser and are at the ball.
    """
    return player.brain.kickDecider.getSweetMove() is None

def orbitBallTooFar(player):
    """
    Ball is far away. Don't want to finish slow orbit.
    """
    return (player.brain.ball.vis.frames_on > 4 and
            player.brain.ball.distance > constants.SHOULD_CANCEL_ORBIT_BALL_DIST)

def orbitTooLong(player):
    """
    We have been in orbit too long, try again.
    """
    return (player.stateTime > constants.ORBIT_TOO_LONG_THR)

####### PENALTY KICK STUFF ###########

def inPenaltyKickStrikezone(player):
    """
    If we are in a good place to kick
    """
    return (NogginConstants.OPP_GOALBOX_LEFT_X + 75. < player.brain.loc.x)


####### FIND BALL STUFF ##############

def shouldFindBall(player):
    """
    We lost the ball, scan to find it
    """
    return (player.brain.ball.vis.frames_off > constants.BALL_OFF_THRESH)

def shouldFindBallKick(player):
    """
    We have been in a kicking state too long, try again.
    """
    return (player.stateTime > constants.BALL_OFF_KICK_THRESH)

def shouldFindBallPosition(player):
    """
    We lost the ball while playbook positioning. We should have a good heading,
    so wait a while before spinning.
    """
    return (player.brain.ball.vis.frames_off > 30 * 3* constants.SPUN_ONCE_TIME_THRESH
            and player.brain.nav.isAtPosition() and player.brain.nav.stateTime >
            3 * constants.SPUN_ONCE_TIME_THRESH)

def ballMoved(player):
    """
    Ball has moved away from where it was seen when positioning and we haven't
    yet kicked it (we kick at counter == 30). We probably walked into it.
    """
    ball = player.brain.ball
    ballBefore = player.ballBeforeKick
    return (player.counter < 30 and (ball.vis.frames_off > 15 or
            fabs(ball.rel_x - ballBefore.rel_x) > constants.BALL_MOVED_THR or
            fabs(ball.rel_y - ballBefore.rel_y) > constants.BALL_MOVED_THR))

def shouldSpinFindBall(player):
    """
    Should spin if we already tried scanning
    """
    return (player.stateTime >=
            SweetMoves.getMoveTime(HeadMoves.HIGH_SCAN_BALL))

def spunOnce(player):
    """
    Did we spin once?
    """
    return player.stateTime > constants.SPUN_ONCE_TIME_THRESH

def shouldWalkFindBall(player):
    """
    If we've been spinFindBall-ing too long we should walk
    """
    return player.stateTime > constants.WALK_FIND_BALL_TIME_THRESH
