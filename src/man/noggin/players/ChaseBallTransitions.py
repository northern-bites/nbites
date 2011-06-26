import man.motion.SweetMoves as SweetMoves
import man.motion.HeadMoves as HeadMoves
import ChaseBallConstants as constants
from .. import NogginConstants
from math import fabs

####### CHASING STUFF ##############

def shouldChaseBall(player):
    """
    We see the ball. So go get it.
    """
    ball = player.brain.ball
    return (ball.framesOn > constants.BALL_ON_THRESH)

def shouldChaseFromPositionForKick(player):
    """
    Exit PFK if the ball is too far away. This should be
    like shouldPFK but with a slightly larger range to avoid
    oscillations between chase and PFK.
    """
    ball = player.brain.ball
    return (shouldChaseBall(player) and
            (ball.dist > constants.BALL_PFK_DIST+5 or
             fabs(ball.relY) > constants.BALL_PFK_LEFT_Y))

def shouldChaseFromSpinToBall(player):
    """
    Exit spinToBall if the ball is now in front of us or suddenly
    far away.
    """
    ball = player.brain.ball
    return (shouldChaseBall(player) and
            (ball.relX > constants.SHOULD_SPIN_TO_KICK_X and
             (shouldPositionForKick(player) or
              (fabs(ball.relY) > constants.BALL_PFK_LEFT_Y and
               ball.dist > constants.SHOULD_STOP_BEFORE_KICK_DIST + 5))))

def shouldChaseFromClaimBall(player):
    """
    Exit claimBall if the ball is no longer too close to us.
    """
    ball = player.brain.ball
    return (shouldChaseBall(player) and
            (shouldChaseFromPositionForKick(player) or
             ball.dist > constants.SHOULD_STOP_BEFORE_KICK_DIST or
             ball.relX < constants.SHOULD_SPIN_TO_KICK_X))

def shouldPositionForKick(player):
    """
    Should begin aligning on the ball for a kick when close
    """
    ball = player.brain.ball
    return (shouldChaseBall(player) and
            ball.dist < constants.BALL_PFK_DIST and
            (constants.BALL_PFK_LEFT_Y > ball.relY >
             constants.BALL_PFK_RIGHT_Y))

def shouldClaimBall(player):
    """
    Ball is right in front of us but we would kick it away if we tried
    to decide the kick on the move. So go claim it first.
    """
    ball = player.brain.ball
    return (shouldPositionForKick(player) and
            ball.dist < constants.SHOULD_STOP_BEFORE_KICK_DIST)

def shouldSpinToBall(player):
    """
    Ball is close and we should spin before we decide our kick
    """
    ball = player.brain.ball
    return (shouldChaseBall(player) and
            ((fabs(ball.relY) > constants.BALL_PFK_LEFT_Y and
             ball.dist < constants.SHOULD_STOP_BEFORE_KICK_DIST) or
             ball.relX <= 9.5))

def ballInPosition(player):
    """
    Make sure ball is somewhere we will kick it
    """
    ball = player.brain.ball
    kick = player.brain.kickDecider.getKick()
    #Get the current kick sweet spot information
    (x_offset, y_offset, heading) = kick.getPosition()

    #Get the difference
    diff_x = fabs(x_offset - ball.relX)
    diff_y = fabs(y_offset - ball.relY)

    print "diff_x:"
    print diff_x
    print "diff_y:"
    print diff_y

    #Compare the sweet spot with the actual values and make sure they
    #are within the threshold
    return (diff_x < constants.X_POS_THRESH and
            diff_y < constants.Y_POS_THRESH)

def ballNearPosition(player):

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

def shouldDribble(player):
    """
    Ball is in between us and the opp goal, let's dribble for a while
    """
    if constants.USE_DRIBBLE:
        my = player.brain.my
        # helpers is no longer used. Find a different way.
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
    # helpers is no longer used. Find a different way.
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
    return (not player.hasKickedOff)

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
    return (player.brain.ball.framesOff > constants.BALL_OFF_THRESH)

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
