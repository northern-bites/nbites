import ChaseBallConstants as constants
import ChaseBallTransitions as transitions
from ..util import *
from objects import Location
from ..navigator import Navigator

@superState('gameControllerResponder')
def findBall(player):
    """
    State to spin to find the ball.
    """
    if player.firstFrame():
        player.brain.tracker.stopHeadMoves()
        player.stopWalking()

    if player.brain.nav.isStopped() and player.brain.tracker.isStopped():
        my = player.brain.loc
        ball = Location(player.brain.ball.x, player.brain.ball.y)
        spinDir = my.spinDirToPoint(ball)
        player.setWalk(0, 0, spinDir*constants.FIND_BALL_SPIN_SPEED)

        player.brain.tracker.spinPan()

    if transitions.shouldChaseBall(player):
        player.stopWalking()
        player.brain.tracker.trackBall()
        return player.goLater('approachBall')

    if transitions.spunOnce(player):
        return player.goLater('positionAtHome')

    return player.stay()

@superState('gameControllerResponder')
def findSharedBall(player):
    if player.firstFrame():
        player.brain.tracker.stopHeadMoves()
        player.stopWalking()

    if player.brain.nav.isStopped() and player.brain.tracker.isStopped():
        my = player.brain.loc
        sharedball = Location(player.brain.sharedBall.x, player.brain.sharedBall.y)
        spinDir = my.spinDirToPoint(sharedball)
        player.setWalk(0, 0, spinDir*constants.FIND_BALL_SPIN_SPEED)

        player.brain.tracker.spinPan()

    if transitions.shouldChaseBall(player):
        player.stopWalking()
        player.brain.tracker.trackSharedBall()
        player.brain.nav.goTo(player, sharedball

        player.brain.nav.goTo(sharedball,
                              precision = Navigator.GENERAL_AREA,
                              speed = Navigator.QUICK_SPEED,
                              avoidObstacles = True,
                              fast = True, pb = False)


)
    # how to set goto stuff??
