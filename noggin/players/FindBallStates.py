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
        if player.currentGait != constants.FAST_GAIT:
            player.stopWalking()
            player.stoppedWalk = False
        else:
            player.stoppedWalk = True

    # Determine if we have stopped walking
    if player.brain.nav.isStopped():
        player.stoppedWalk = True

    # Walk if we have stopped walking
    if player.stoppedWalk:
        # Switch to the fast gait for better spinning
        if (player.brain.nav.isStopped() and
            player.currentGait != constants.FAST_GAIT):
            player.brain.CoA.setRobotGait(player.brain.motion)
            player.currentGait = constants.FAST_GAIT

        player.setSpeed(0, 0, constants.FIND_BALL_SPIN_SPEED)

    # Determine if we should leave this state
    if player.brain.ball.on and constants.USE_LOC_CHASE:
        player.brain.tracker.trackBall()
        return player.goLater('positionOnBall')
    elif transitions.shouldTurnToBall_FoundBall(player):
        return player.goLater('turnToBall')

    return player.stay()
