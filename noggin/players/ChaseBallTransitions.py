import man.motion.SweetMoves as SweetMoves
import ChaseBallConstants as constants


def shouldTurnToBall_FoundBall(player):
    if player.brain.ball.framesOn > constants.FRAMES_ON_THRESH:
        return True
    return False

def shouldTurnToBall_ApproachBall(player):
    ball = player.brain.ball
    if (ball.on and
        abs(ball.bearing) > constants.BALL_BEARING_OFF_THRESH):
        return True
    return False

def shouldCantFindBall(player):
    if player.stateTime >= constants.GIVE_UP_THRESH:
        return True
    return False

def shouldSpinFindBall(player):
    if player.stateTime >= SweetMoves.getMoveTime(SweetMoves.SCAN_BALL):
        return True
    return False

def shouldSpinFindBallPosition(player):
    if player.brain.ball.framesOff >= 50:
        return True
    return False

def shouldScanFindBall(player):
    if player.brain.ball.framesOff > constants.FRAMES_OFF_THRESH:
        return True
    return False

def shouldApproachBall(player):
    ball = player.brain.ball
    if ( ball.on and
         abs(ball.bearing) < constants.BALL_BEARING_THRESH ):
        return True
    return False

def shouldApproachBallClose(player):
    ball = player.brain.ball
    if ball.dist < constants.BALL_CLOSE_DIST_THRESH and \
            abs(ball.bearing) < constants.BALL_CLOSE_APPROACH_BEARING_THRESH and\
            ball.on :
        return True
    return False

def shouldTurnToBallClose(player):
    ball = player.brain.ball
    if ( ball.on and
         abs(ball.bearing) > constants.BALL_CLOSE_TURN_BEARING_THRESH and
         ball.dist < constants.BALL_CLOSE_DIST_THRESH):
        return True

def shouldPositionForKick(player):
    ball = player.brain.ball
    if (ball.on and
        abs(ball.bearing) < constants.BALL_POS_KICK_BEARING_THRESH and
        ball.dist < constants.BALL_POS_KICK_DIST_THRESH ):
        return True
    return False

def shouldApproachForKick(player):
    ball = player.brain.ball
    if (ball.on and
        ball.relX > constants.BALL_KICK_LEFT_X_FAR ):
        return True
    return False

def shouldTurnForKick(player):
    ball = player.brain.ball
    if ( ball.on and
          abs(ball.bearing) > constants.BALL_TURN_KICK_BEARING_THRESH and
         ball.dist < constants.BALL_POS_KICK_DIST_THRESH ):
        return True
    return False

def shouldKick(player):
    ball = player.brain.ball
    if (ball.framesOff < 10 and
        ball.locRelY > constants.BALL_KICK_LEFT_Y_R and
        ball.locRelY < constants.BALL_KICK_LEFT_Y_L and
         ball.locRelX > constants.BALL_KICK_LEFT_X_CLOSE and
         ball.locRelX < constants.BALL_KICK_LEFT_X_FAR ):
        return True
    return False

def shouldTurnToBall_fromAtBallPosition(player):
    ball = player.brain.ball
    if (ball.on and
        abs(ball.bearing) > constants.BALL_SPIN_POSITION_THRESH + 10):
        return True
    return False

def shouldBeAtSpinDir(player):
    ball = player.brain.ball
    if ( ball.on and
         abs(ball.bearing) < constants.BALL_SPIN_POSITION_THRESH ):
        return True
    return False

def shouldAvoidObstacleLeft(player):
    sonar = player.brain.sonar
    if (#(sonar.LLdist != sonar.UNKNOWN_VALUE and
        # sonar.LLdist < constants.AVOID_OBSTACLE_DIST) or
        (sonar.LRdist != sonar.UNKNOWN_VALUE and
         sonar.LRdist < constants.AVOID_OBSTACLE_DIST)):
        return True

    return False

def shouldAvoidObstacleRight(player):
    sonar = player.brain.sonar
    if (#(sonar.RRdist != sonar.UNKNOWN_VALUE and
        # sonar.RRdist < constants.AVOID_OBSTACLE_DIST) or
        (sonar.RLdist != sonar.UNKNOWN_VALUE and
         sonar.RLdist < constants.AVOID_OBSTACLE_DIST)):
        return True

    return False

def shouldAvoidObstacle(player):
    return (shouldAvoidObstacleLeft(player) or
            shouldAvoidObstacleRight(player))

