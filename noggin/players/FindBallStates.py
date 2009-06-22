import ChaseBallConstants as constants
import ChaseBallTransitions as transitions
from ..util import MyMath

def scanFindBall(player):
    """
    State to move the head to find the ball. If we find the ball, we
    move to align on it. If we don't find it, we spin to keep looking
    """
    if player.firstFrame():
        player.stopWalking()
        player.brain.tracker.trackBall()

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
        my = player.brain.my
        ball = player.brain.ball
        spinDir = MyMath.getSpinDir(my.h,
                                    ball.bearing + my.h)

    player.setSpeed(0, 0, spinDir*constants.FIND_BALL_SPIN_SPEED)

    # Determine if we should leave this state
    if transitions.shouldApproachBallWithLoc(player):
        player.brain.tracker.trackBall()
        return player.goLater('approachBallWithLoc')
    elif transitions.shouldTurnToBall_FoundBall(player):
        return player.goLater('turnToBall')

    return player.stay()
