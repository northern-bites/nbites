import ChaseBallConstants as constants
import ChaseBallTransitions as transitions
from objects import Location

def findBall(player):
    """
    State to stop all activity and begin finding the ball
    """
    if transitions.shouldChaseBall(player):
        if not player.brain.play.isChaser():
            return player.goLater('playbookPosition')
        else:
            return player.goLater('chase')

    if player.firstFrame():
        player.stand()
        player.brain.tracker.stopHeadMoves()

    if player.brain.nav.isStopped():
        return player.goLater('spinFindBall')

    return player.stay()

def spinFindBall(player):
    """
    State to spin to find the ball. If we find the ball, we
    move to align on it. If we don't find it, we walk to look for it
    """
    if transitions.shouldChaseBall(player):
        player.stopWalking()
        player.brain.tracker.trackBall()
        return player.goNow('findBall')

    if player.firstFrame():
        player.brain.tracker.stopHeadMoves()

    if player.brain.nav.isStopped() and player.brain.tracker.isStopped():
        my = player.brain.loc
        ball = Location(player.brain.ball.x, player.brain.ball.y)
        spinDir = my.spinDirToPoint(ball)
        player.setWalk(0, 0, spinDir*constants.FIND_BALL_SPIN_SPEED)

        player.brain.tracker.spinPan()

    if transitions.shouldWalkFindBall(player):
        return player.goLater('walkFindBall')

    return player.stay()

def walkFindBall(player):
    """
    State to walk to find the ball. If we find the ball we chase it.
    """
    if player.firstFrame():
        player.stopWalking()
        # Do a slow pan
        player.brain.tracker.repeatWidePan()

    if transitions.shouldChaseBall(player):
        player.stopWalking()
        player.brain.tracker.trackBall()
        return player.goNow('findBall')

    if player.brain.nav.isStopped():
        player.brain.nav.chaseBall()

    if transitions.shouldSpinFindBallAgain(player):
        return player.goLater('spinFindBall')

    return player.stay()
