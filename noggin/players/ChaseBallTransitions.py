import man.motion.SweetMoves as SweetMoves
import man.motion.HeadMoves as HeadMoves
import ChaseBallConstants as constants

####### CHASING STUFF ##############

def shouldTurnToBall_FoundBall(player):
    """
    Should we turn to the ball heading after searching for the ball
    """
    return( player.brain.ball.framesOn > constants.BALL_ON_THRESH)

def shouldTurnToBall_ApproachBall(player):
    """
    Should turn to the ball, if we are currently approaching it
    """
    ball = player.brain.ball
    return (ball.on and
            abs(ball.bearing) > constants.BALL_APPROACH_BEARING_OFF_THRESH)

def shouldApproachBall(player):
    """
    Begin walking to the ball if it is close to straight in front of us
    """
    ball = player.brain.ball
    return ( ball.on and
             abs(ball.bearing) < constants.BALL_APPROACH_BEARING_THRESH )

def shouldApproachFromPositionForKick(player):
    """
    Walk to the ball if its too far away
    """
    ball = player.brain.ball
    return ( ball.on and
             ball.relX > constants.BALL_POS_KICK_DIST_THRESH +
             constants.POSITION_FOR_KICK_DIST_THRESH)

def shouldTurnToBallFromPositionForKick(player):
    """
    Walk to the ball if its too far away
    """
    ball = player.brain.ball
    return (ball.on and
            abs(ball.bearing) > constants.BALL_APPROACH_BEARING_OFF_THRESH +
            constants.POSITION_FOR_KICK_BEARING_THRESH)

def shouldPositionForKick(player):
    """
    Should begin aligning on the ball for a kick when close
    """
    ball = player.brain.ball
    return (ball.on and
            abs(ball.bearing) < constants.BALL_POS_KICK_BEARING_THRESH and
            ball.dist < constants.BALL_POS_KICK_DIST_THRESH )

def shouldRepositionForKick(player):
    """
    Stop waiting for kick and realign on the ball instead
    """
    ball = player.brain.ball
    return False

def shouldApproachForKick(player):
    """
    While positioning for kick, we need to walk forward
    """
    ball = player.brain.ball
    return (ball.on and
            ball.relX > constants.BALL_KICK_LEFT_X_FAR )

def shouldKick(player):
    """
    Ball is in the correct foot position to kick
    """
    ball = player.brain.ball
    return (ball.framesOff < 10 and
            ball.locRelY > constants.BALL_KICK_LEFT_Y_R and
            ball.locRelY < constants.BALL_KICK_LEFT_Y_L and
            ball.locRelX > constants.BALL_KICK_LEFT_X_CLOSE and
            ball.locRelX < constants.BALL_KICK_LEFT_X_FAR )

####### AVOIDANCE STUFF ##############

def shouldAvoidObstacleLeft(player):
    """
    Need to avoid an obstacle on our left side
    """
    sonar = player.brain.sonar
    return (#(sonar.LLdist != sonar.UNKNOWN_VALUE and
        # sonar.LLdist < constants.AVOID_OBSTACLE_DIST) or
        (sonar.LRdist != sonar.UNKNOWN_VALUE and
         sonar.LRdist < constants.AVOID_OBSTACLE_DIST))

def shouldAvoidObstacleRight(player):
    """
    Need to avoid an obstacle on our right side
    """
    sonar = player.brain.sonar
    return (#(sonar.RRdist != sonar.UNKNOWN_VALUE and
        # sonar.RRdist < constants.AVOID_OBSTACLE_DIST) or
        (sonar.RLdist != sonar.UNKNOWN_VALUE and
         sonar.RLdist < constants.AVOID_OBSTACLE_DIST))

def shouldAvoidObstacle(player):
    """
    Should avoid an obstacle
    """
    return (shouldAvoidObstacleLeft(player) or
            shouldAvoidObstacleRight(player))

####### FIND BALL STUFF ##############

def shouldScanFindBall(player):
    """
    We lost the ball, scan to find it
    """
    return (player.brain.ball.framesOff > constants.BALL_OFF_THRESH)

def shouldSpinFindBall(player):
    """
    Should spin if we already tried searching
    """
    return (player.stateTime >= SweetMoves.getMoveTime(HeadMoves.HIGH_SCAN_BALL))

def shouldntStopChasing(player):
    """
    Dont switch out of chaser in certain circumstances
    """
    return (player.currentState == 'decideKick' or
            player.currentState == 'kickBallStraight' or
            player.currentState == 'kickBallLeft' or
            player.currentState == 'kickBallRight' or
            player.currentState == 'kickBallLeftExecute')
