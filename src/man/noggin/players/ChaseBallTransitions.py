import man.motion.SweetMoves as SweetMoves
import man.motion.HeadMoves as HeadMoves
import ChaseBallConstants as constants
import KickingHelpers as helpers
from .. import NogginConstants
from ..playbook.PBConstants import GOALIE

from math import fabs

####### CHASING STUFF ##############

def shouldChaseBall(player):
    """
    We see the ball. So go get it.
    """
    ball = player.brain.ball
    return (ball.framesOn > constants.BALL_ON_THRESH)

def shouldApproachBall(player):
    """
    Approach the ball if it is far away.
    """
    ball = player.brain.ball
    return (ball.on and not shouldPositionForKick(player))

def shouldChaseFromPositionForKick(player):
    """
    Exit PFK if the ball is too far away.
    """
    ball = player.brain.ball
    return shouldChaseBall(player) and \
        not shouldPositionForKick(player) and \
        ball.dist > constants.BALL_PFK_MAX_X+10

def shouldPositionForKick(player):
    """
    Should begin aligning on the ball for a kick when close
    """
    ball = player.brain.ball
    return (constants.BALL_PFK_LEFT_Y > ball.relY > \
            constants.BALL_PFK_RIGHT_Y and \
            constants.BALL_PFK_MAX_X > ball.relX > \
            constants.BALL_PFK_MIN_X and \
            fabs(ball.bearing) < constants.BALL_PFK_BEARING_THRESH)

def shouldSpinToBallClose(player):
    ball = player.brain.ball
    return ball.on and \
        ball.dist < constants.SHOULD_STOP_DIST and \
        fabs(ball.relY) > constants.SHOULD_STOP_Y

def shouldSpinToKick(player):
    ball = player.brain.ball
    return (ball.relX < constants.SHOULD_SPIN_TO_KICK_X
            and ball.relX > 0)

def shouldStopBeforeKick(player):
    """
    Ball is right in front of us but we aren't stopped
    Used before we have a kick decided (more general)
    """
    ball = player.brain.ball
    return ball.on and \
        ball.relX > constants.SHOULD_KICK_CLOSE_X and \
        ball.relX < constants.SHOULD_KICK_FAR_X and \
        fabs(ball.relY) < constants.SHOULD_KICK_Y

def shouldStopAndKick(player):
    """
    Ball is in correct position to kick but we aren't stopped
    Used after we have a kick decided (more specific)
    """
    ball = player.brain.ball
    kick = player.brain.kickDecider.getKick()
    (targetX, targetY, heading) = kick.getPosition()
    return (ball.on and \
                (fabs(ball.relX - targetX) <= constants.KICK_CLOSE_ENOUGH_X) and \
                (fabs(ball.relY - targetY) <= constants.KICK_CLOSE_ENOUGH_Y))

def shouldKickNow(player):
    """
    Ball is in the correct position to kick and we are stopped
    Used after we have a kick decided (more specific)
    """
    ball = player.brain.ball
    kick = player.brain.kickDecider.getKick()
    (targetX, targetY, heading) = kick.getPosition()
    return (player.brain.nav.isStopped() and \
                ball.on and \
                (fabs(ball.relX - targetX) <= constants.KICK_CLOSE_ENOUGH_X) and \
                (fabs(ball.relY - targetY) <= constants.KICK_CLOSE_ENOUGH_Y))
                # and player.counter < 1 ??

def shouldDribble(player):
    """
    Ball is in between us and the opp goal, let's dribble for a while
    """
    if constants.USE_DRIBBLE:
        my = player.brain.my
        dribbleAimPoint = helpers.getShotCloseAimPoint(player)
        goalBearing = my.getRelativeBearing(dribbleAimPoint)
        return  (not player.penaltyKicking and
                 0 < player.brain.ball.relX < constants.SHOULD_DRIBBLE_X and
                 0 < fabs(player.brain.ball.relY) < constants.SHOULD_DRIBBLE_Y and
                 fabs(goalBearing) < constants.SHOULD_DRIBBLE_BEARING and
                 not player.brain.my.inOppGoalbox() and
                 player.brain.my.x > (
                     NogginConstants.FIELD_WHITE_WIDTH / 3.0 +
                     NogginConstants.GREEN_PAD_X) )

def shouldStopDribbling(player):
    """
    While dribbling we should stop
    """
    my = player.brain.my
    dribbleAimPoint = helpers.getShotCloseAimPoint(player)
    goalBearing = my.getRelativeBearing(dribbleAimPoint)
    return (player.penaltyKicking or
            player.brain.my.inOppGoalbox() or
            player.brain.ball.relX > constants.STOP_DRIBBLE_X or
            fabs(player.brain.ball.relY) > constants.STOP_DRIBBLE_Y or
            fabs(goalBearing) > constants.STOP_DRIBBLE_BEARING or
            player.brain.my.x < ( NogginConstants.FIELD_WHITE_WIDTH / 3.0 +
                                  NogginConstants.GREEN_PAD_X))

def shouldKickOff(player):
    """
    Determines whether we should do our KickOff play as chaser
    """
    return (player.brain.gameController.ownKickOff and
            not player.hasKickedOffKick)

####### FIND BALL STUFF ##############

def shouldFindBall(player):
    """
    We lost the ball, scan to find it
    """
    return (player.brain.ball.framesOff > constants.BALL_OFF_THRESH)

def shouldFindBallActiveLoc(player):
    """
    We lost the ball, scan to find it
    """
    return not (player.brain.tracker.activePanUp or
                player.brain.tracker.activePanOut) and \
        (player.brain.ball.framesOff > constants.BALL_OFF_ACTIVE_LOC_THRESH)

def shouldFindBallKick(player):
    """
    We lost the ball while in a kicking state, be more generous before looking
    """
    return (player.brain.ball.framesOff > constants.BALL_OFF_KICK_THRESH)

def shouldSpinFindBall(player):
    """
    Should spin if we already tried scanning
    """
    return (player.stateTime >=
            SweetMoves.getMoveTime(HeadMoves.HIGH_SCAN_BALL))

def shouldSpinFindBallAgain(player):
    """
    If we have been walkFindBall-ing too long we should spin.
    """
    return player.stateTime > constants.WALK_FIND_BALL_FRAMES_THRESH

def shouldWalkFindBall(player):
    """
    If we've been spinFindBall-ing too long we should walk
    """
    return player.counter > constants.WALK_FIND_BALL_FRAMES_THRESH

def shouldntStopChasing(player):
    """
    Dont switch out of chaser in certain circumstances
    """
    return player.inKickingState

def shouldPreKickScan(player):
    if player.brain.ball.on:
        return (constants.PRE_KICK_SCAN_MIN_DIST < player.brain.ball.dist
                < constants.PRE_KICK_SCAN_MAX_DIST and
                abs(player.brain.ball.bearing) <
                constants.APPROACH_ACTIVE_LOC_BEARING)
    return False

def shouldActiveLoc(player):
    if player.brain.ball.on:
        return (player.brain.ball.dist > constants.APPROACH_ACTIVE_LOC_DIST
                and fabs(player.brain.ball.bearing) <
                constants.APPROACH_ACTIVE_LOC_BEARING)

    else:
        return player.brain.ball.dist > constants.APPROACH_ACTIVE_LOC_DIST

def shouldStopPenaltyKickDribbling(player):
    """
    While dribbling we should stop
    """
    my = player.brain.my
    dribbleAimPoint = helpers.getShotCloseAimPoint(player)
    goalBearing = my.getRelativeBearing(dribbleAimPoint)
    return (inPenaltyKickStrikezone(player) or
            player.brain.ball.relX > constants.STOP_DRIBBLE_X or
            fabs(player.brain.ball.relY) > constants.STOP_DRIBBLE_Y or
            fabs(goalBearing) > constants.STOP_DRIBBLE_BEARING or
            player.counter > constants.STOP_PENALTY_DRIBBLE_COUNT)

def inPenaltyKickStrikezone(player):
    """
    If we are in a good place to kick
    """
    return (NogginConstants.OPP_GOALBOX_LEFT_X + 75. < player.brain.my.x)
