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

    return player.goNow('spinFindBall')

def spinFindBall(player):
    """
    State to spin to find the ball. If we find the ball, we
    move to align on it. If we don't find it, we walk to look for it
    """
    if transitions.shouldChaseBall(player):
        player.stopWalking()
        player.brain.tracker.trackBall()
        return player.goLater('findBall')

    if player.firstFrame():
        player.brain.tracker.stopHeadMoves()
        player.stopWalking()

    if player.brain.nav.isStopped() and player.brain.tracker.isStopped():
        my = player.brain.loc
        ball = Location(player.brain.ball.x, player.brain.ball.y)
        spinDir = my.spinDirToPoint(ball)
        player.setWalk(0, 0, spinDir*constants.FIND_BALL_SPIN_SPEED)

        player.brain.tracker.spinPan()

    if ((player.brain.play.isChaser() and transitions.shouldWalkFindBall(player))
        or (not player.brain.play.isChaser() and transitions.spunOnce())):
        return player.goLater('playbookPosition')

    return player.stay()
