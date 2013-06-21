from .. import SweetMoves
from ..headTracker import HeadMoves
import ChaseBallConstants as constants
import noggin_constants as NogginConstants
from math import fabs

####### CHASING STUFF ##############

def shouldChaseBall(player):
    """
    We see the ball. So go get it.
    """
    ball = player.brain.ball
    return (ball.vis.frames_on > constants.BALL_ON_THRESH)

def shouldPrepareForKick(player):
    """
    We're close enough to prepare for a kick
    """
    ball = player.brain.ball
    return (ball.vis.frames_on > 4 and
            ball.distance < constants.PREPARE_FOR_KICK_DIST)

def shouldSpinToBall(player):
    """
    We're not facing the ball well enough yet
    """
    ball = player.brain.ball
    return (ball.vis.on and
            fabs(ball.rel_y) > constants.SHOULD_SPIN_TO_BALL_Y and
            not (ball.distance > constants.SHOULD_SPIN_TO_BALL_DIST and
                 fabs(ball.bearing_deg) < constants.SHOULD_SPIN_TO_BALL_BEAR))

def shouldStopSpinningToBall(player):
    """
    We're done spinning
    """
    ball = player.brain.ball
    return (ball.vis.on and
            fabs(ball.rel_y) < constants.STOP_SPINNING_TO_BALL_Y)

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
    if player.brain.ball.vis.frames_on < 4:
        return False

    #Get the current kick sweet spot information

    return (fabs(kickPose.relX) < constants.BALL_X_OFFSET and
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

def shouldCancelOrbit(player):
    """
    Ball is far away. Don't want to finish slow orbit.
    """
    return (player.brain.ball.vis.frames_on > 4 and
            player.brain.ball.distance > constants.SHOULD_CANCEL_ORBIT_BALL_DIST)

####### PENALTY KICK STUFF ###########

def shouldStopPenaltyKickDribbling(player):
    """
    While dribbling we should stop
    """
    my = player.brain.loc
    # helpers is no longer used. Find a different way.
    dribbleAimPoint = helpers.getShotCloseAimPoint(player)
    goalBearing = my.getRelativeBearing(dribbleAimPoint)
    return (inPenaltyKickStrikezone(player) or
            player.brain.ball.rel_x > constants.STOP_DRIBBLE_X or
            fabs(player.brain.ball.rel_y) > constants.STOP_DRIBBLE_Y or
            fabs(goalBearing) > constants.STOP_DRIBBLE_BEARING or
            player.counter > constants.STOP_PENALTY_DRIBBLE_COUNT)

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
    We lost the ball while in a kicking state, be more generous before looking
    """
    return (player.brain.ball.vis.frames_off > constants.BALL_OFF_KICK_THRESH)

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
    Ball has moved away from where it was seen when positioning. We probably
    dribbled through it.
    """
    ball = player.brain.ball
    ballBefore = player.ballBeforeKick
    return (fabs(ball.rel_x - ballBefore.rel_x) > constants.BALL_MOVED_THR or
            fabs(ball.rel_y - ballBefore.rel_y) > constants.BALL_MOVED_THR)

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
    return player.stateTime > constants.WALK_FIND_BALL_FRAMES_THRESH
