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

def ballInPosition(player):
    """
    Make sure ball is somewhere we will kick it
    """
    ball = player.brain.ball
    kick = player.brain.kickDecider.getKick()
    #Get the current kick sweet spot information
    (x_offset, y_offset, heading) = kick.getPosition()

    #Get the difference
    diff_x = x_offset - ball.relX # not absolute value, if ball is closer kick anyway
    diff_y = fabs(y_offset - ball.relY)

    if diff_x < constants.BALL_X_OFFSET:
        print "Ball X OK"
    if diff_y < constants.BALL_Y_OFFSET:
        print "Ball Y OK"

    #Compare the sweet spot with the actual values and make sure they
    #are within the threshold
    return (diff_x < constants.BALL_X_OFFSET and
            diff_y < constants.BALL_Y_OFFSET)

def ballNearPosition(player):
    """
    Ball is around our feet. Maybe we wiffed?
    """
    ball = player.brain.ball
    return ((constants.SHOULD_KICK_AGAIN_CLOSE_X < ball.relX <
              constants.SHOULD_KICK_AGAIN_FAR_X) and
             fabs(ball.relY) < constants.SHOULD_KICK_AGAIN_Y)

def shouldKick(player):
    """
    Ball is in correct position to kick
    """
    return player.brain.nav.isStopped() and player.counter > 1

def shouldKickAgain(player):
    """
    Ball hasn't changed enough to warrant new kick decision.
    """
    return (shouldKick(player) and ballNearPosition(player))

def ballTooFar(player):
    """
    Navigator is almost at its destination, but we're still far away from
    the ball
    """
    if player.brain.nav.nearDestination and player.brain.ball.dist > 30:
        print "ballTooFar"
        return True
    return False

def shouldSwitchPFKModes(player):
    """
    True if we're near to the ball and using setSpeed, or far away and using setDest
    """
    ball = player.brain.ball

    # using setSpeed
    if player.brain.nav.currentState == 'goToPosition':
        # forces the vector to update in case the ball moved
        if player.counter > constants.BALL_APPROACH_REFRESH_FRAMES:
            return True

        return ball.dist <= constants.BALL_SET_DEST_CUTOFF

    # using setDestination
    return ball.dist > constants.BALL_SET_DEST_CUTOFF


def shouldOrbit(player):
    """
    We are lost but are chaser and are at the ball.
    """
    return player.shouldOrbit

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
            player.brain.ball.relX > constants.STOP_DRIBBLE_X or
            fabs(player.brain.ball.relY) > constants.STOP_DRIBBLE_Y or
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
