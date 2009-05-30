import PositionConstants as constants

####### POSITIONING STUFF ##############

def shouldTurnToBall_fromAtBallPosition(player):
    ball = player.brain.ball
    if (ball.on and
        abs(ball.bearing) > constants.BALL_SPIN_POSITION_THRESH + 10):
        return True
    return False
