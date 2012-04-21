import man.motion.SweetMoves as SweetMoves
import man.motion.HeadMoves as HeadMoves
import ChaseBallConstants as constants
import noggin_constants as NogginConstants
from math import fabs

####### CHASING STUFF ##############

def shouldChaseBall(player):
    """
    We see the ball. So go get it.
    """
    ball = player.brain.ball
    return (ball.vis.framesOn > constants.BALL_ON_THRESH)

def shouldPrepareForKick(player):
    """
    We're close enough to prepare for a kick
    """
    ball = player.brain.ball.loc
    return ball.dist < constants.PREPARE_FOR_KICK_DIST
    

def ballInPosition(player):
    """
    Make sure ball is somewhere we will kick it. Also makes sure we're looking
    at the ball.
    """
    if player.brain.ball.vis.framesOn < 4:
        return False

    #Get the current kick sweet spot information
    position = player.brain.kickDecider.getIdealKickPosition()

    return (0 < position.relX < constants.BALL_X_OFFSET and
                position.relY < constants.BALL_Y_OFFSET and
                position.relH < constants.GOOD_ENOUGH_H)

def ballNearPosition(player):
    """
    Ball is around our feet. Maybe we wiffed?
    """
    ball = player.brain.ball
    return ((constants.SHOULD_KICK_AGAIN_CLOSE_X < ball.loc.relX <
              constants.SHOULD_KICK_AGAIN_FAR_X) and
             fabs(ball.loc.relY) < constants.SHOULD_KICK_AGAIN_Y)

def shouldKick(player):
    """
    Ball is in correct position to kick
    """
    return player.brain.nav.isAtPosition() and player.counter > 1

def shouldKickAgain(player):
    """
    Ball hasn't changed enough to warrant new kick decision.
    """
    return (shouldKick(player) and ballNearPosition(player))

def shouldOrbit(player):
    """
    We are lost (no kick) but are chaser and are at the ball.
    """
    return player.brain.kickDecider.getSweetMove() is None

####### PENALTY KICK STUFF ###########

def shouldStopPenaltyKickDribbling(player):
    """
    While dribbling we should stop
    """
    my = player.brain.my
    # helpers is no longer used. Find a different way.
    dribbleAimPoint = helpers.getShotCloseAimPoint(player)
    goalBearing = my.getRelativeBearing(dribbleAimPoint)
    return (inPenaltyKickStrikezone(player) or
            player.brain.ball.loc.relX > constants.STOP_DRIBBLE_X or
            fabs(player.brain.ball.loc.relY) > constants.STOP_DRIBBLE_Y or
            fabs(goalBearing) > constants.STOP_DRIBBLE_BEARING or
            player.counter > constants.STOP_PENALTY_DRIBBLE_COUNT)

def inPenaltyKickStrikezone(player):
    """
    If we are in a good place to kick
    """
    return (NogginConstants.OPP_GOALBOX_LEFT_X + 75. < player.brain.my.x)


####### FIND BALL STUFF ##############

def shouldFindBall(player):
    """
    We lost the ball, scan to find it
    """
    return (player.brain.ball.vis.framesOff > constants.BALL_OFF_THRESH)

def shouldFindBallKick(player):
    """
    We lost the ball while in a kicking state, be more generous before looking
    """
    return (player.brain.ball.vis.framesOff > constants.BALL_OFF_KICK_THRESH)

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
