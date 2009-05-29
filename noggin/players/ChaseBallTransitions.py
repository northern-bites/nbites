import man.motion.SweetMoves as SweetMoves
import ChaseBallConstants as constants

def shouldTurnToBall_FoundBall(player):
    """
    Should we turn to the ball heading after searching for the ball
    """
    if player.brain.ball.framesOn > constants.BALL_ON_THRESH:
        return True
    return False

def shouldTurnToBall_ApproachBall(player):
    """
    Should turn to the ball, if we are currently approaching it
    """
    ball = player.brain.ball
    if (ball.on and
        abs(ball.bearing) > constants.BALL_APPROACH_BEARING_OFF_THRESH):
        return True
    return False

def shouldScanFindBall(player):
    """
    We lost the ball, scan to find it
    """
    if player.brain.ball.framesOff > constants.BALL_OFF_THRESH:
        return True
    return False

def shouldSpinFindBall(player):
    """
    Should spin if we already tried searching
    """
    if player.stateTime >= SweetMoves.getMoveTime(SweetMoves.SCAN_BALL):
        return True
    return False

def shouldApproachBall(player):
    """
    Begin walking to the ball if it is close to straight in front of us
    """
    ball = player.brain.ball
    if ( ball.on and
         abs(ball.bearing) < constants.BALL_APPROACH_BEARING_THRESH ):
        return True
    return False

def shouldPositionForKick(player):
    """
    Should begin aligning on the ball for a kick when close
    """
    ball = player.brain.ball
    if (ball.on and
        abs(ball.bearing) < constants.BALL_POS_KICK_BEARING_THRESH and
        ball.dist < constants.BALL_POS_KICK_DIST_THRESH ):
        return True
    return False

def shouldApproachForKick(player):
    """
    While positioning for kick, we need to walk forward
    """
    ball = player.brain.ball
    if (ball.on and
        ball.relX > constants.BALL_KICK_LEFT_X_FAR ):
        return True
    return False

def shouldKick(player):
    """
    Ball is in the correct foot position to kick
    """
    ball = player.brain.ball
    if (ball.framesOff < 10 and
        ball.locRelY > constants.BALL_KICK_LEFT_Y_R and
        ball.locRelY < constants.BALL_KICK_LEFT_Y_L and
         ball.locRelX > constants.BALL_KICK_LEFT_X_CLOSE and
         ball.locRelX < constants.BALL_KICK_LEFT_X_FAR ):
        return True
    return False

def shouldAvoidObstacleLeft(player):
    """
    Need to avoid an obstacle on our left side
    """
    sonar = player.brain.sonar
    if (#(sonar.LLdist != sonar.UNKNOWN_VALUE and
        # sonar.LLdist < constants.AVOID_OBSTACLE_DIST) or
        (sonar.LRdist != sonar.UNKNOWN_VALUE and
         sonar.LRdist < constants.AVOID_OBSTACLE_DIST)):
        return True

    return False

def shouldAvoidObstacleRight(player):
    """
    Need to avoid an obstacle on our right side
    """
    sonar = player.brain.sonar
    if (#(sonar.RRdist != sonar.UNKNOWN_VALUE and
        # sonar.RRdist < constants.AVOID_OBSTACLE_DIST) or
        (sonar.RLdist != sonar.UNKNOWN_VALUE and
         sonar.RLdist < constants.AVOID_OBSTACLE_DIST)):
        return True

    return False

def shouldAvoidObstacle(player):
    """
    Should avoid an obstacle
    """
    return (shouldAvoidObstacleLeft(player) or
            shouldAvoidObstacleRight(player))

