import ChaseBallConstants as constants
import ChaseBallTransitions as transitions
from ..playbook import PBConstants as pbc

def scanFindBall(player):
    """
    State to move the head to find the ball. If we find the ball, we
    move to align on it. If we don't find it, we spin to keep looking
    """
    if player.firstFrame():
        player.stopWalking()
        player.brain.tracker.trackBall()

    if player.currentRole == pbc.GOALIE:
        if transitions.shouldTurnToBall_FoundBall(player):
            return player.goLater('turnToBall')
        elif transitions.shouldSpinFindBall(player):
            return player.goLater('spinFindBall')
    else:
        if transitions.shouldApproachBallWithLoc(player):
            player.brain.tracker.trackBall()
            return player.goLater('approachBallWithLoc')
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
    if player.justKicked:
        spinDir = player.getSpinDirAfterKick()
    else:
        spinDir = constants.TURN_LEFT

    player.setSpeed(0, 0, spinDir*constants.FIND_BALL_SPIN_SPEED)

    # Determine if we should leave this state
    if player.currentRole == pbc.GOALIE:
        if transitions.shouldTurnToBall_FoundBall(player):
            return player.goLater('turnToBall')
    else:
        if transitions.shouldApproachBallWithLoc(player):
            player.brain.tracker.trackBall()
            return player.goLater('approachBallWithLoc')
        elif transitions.shouldTurnToBall_FoundBall(player):
            return player.goLater('turnToBall')

    return player.stay()
