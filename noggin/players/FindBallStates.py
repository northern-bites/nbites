import ChaseBallConstants as constants
import ChaseBallTransitions as transitions

def scanFindBall(player):
    """
    State to move the head to find the ball. If we find the ball, we
    move to align on it. If we don't find it, we spin to keep looking
    """
    if player.firstFrame():
        player.stopWalking()
        player.brain.tracker.trackBall()

    if player.brain.ball.on and constants.USE_LOC_CHASE:
        player.brain.tracker.trackBall()
        return player.goLater('positionOnBall')
    elif transitions.shouldTurnToBall_FoundBall(player):
        return player.goLater('turnToBall')
    elif transitions.shouldSpinFindBall(player):
        return player.goLater('spinFindBall')

    return player.stay()

def spinFindBall(player):
    """
    State to spin to find the ball. If we find the ball, we
    move to align on it. If we don't find it, we go to a garbage state
    """
    if player.firstFrame():
        player.brain.tracker.trackBall()
        # Stop walking if we need to switch gaits

    player.setSpeed(0, 0, constants.FIND_BALL_SPIN_SPEED)

    # Determine if we should leave this state
    if player.brain.ball.on and constants.USE_LOC_CHASE:
        player.brain.tracker.trackBall()
        return player.goLater('positionOnBall')
    elif transitions.shouldTurnToBall_FoundBall(player):
        return player.goLater('turnToBall')

    return player.stay()
