import ChaseBallConstants as constants
import ChaseBallTransitions as transitions
from ..util import *
from objects import Location
from math import fabs, degrees

@superState('gameControllerResponder')
def findBall(player):
    """
    State to spin to find the ball.
    """
    if player.firstFrame():
        player.brain.tracker.stopHeadMoves()
        player.stopWalking()
        player.inKickingState = False

    if transitions.shouldChaseBall(player):
        player.stopWalking()
        player.brain.tracker.trackBall()
        return player.goLater('spinToFoundBall')

    if player.brain.nav.isStopped() and player.brain.tracker.isStopped():
        #TODO CONST
        if degrees(player.brain.ball.bearing) >= 25:
            my = player.brain.loc
            ball = Location(player.brain.ball.x, player.brain.ball.y)
            spinDir = my.spinDirToPoint(ball)
            player.setWalk(0, 0, spinDir*constants.FIND_BALL_SPIN_SPEED)

            player.brain.tracker.lookToSpinDirection(spinDir)
        else:
            return player.goNow('findBallForward')

    if transitions.spunOnce(player):
        return player.goLater('playOffBall')

    return player.stay()

@superState('gameControllerResponder')
def spinToFoundBall(player):
    """
    spins to the ball until it is facing the ball 
    """
    if player.firstFrame():
        player.brain.tracker.trackBall()
        print "spinning to found ball"

    theta = math.degrees(player.brain.ball.bearing)
    spinToFoundBall.isFacingBall = math.fabs(theta) <= constants.FACING_BALL_ACCEPTABLE_BEARING

    if spinToFoundBall.isFacingBall:
        print "facing ball"
        return player.goNow('approachBall')

    # spins the appropriate direction
    if theta < 0:
        player.brain.nav.walk(0., 0., -1*constants.FIND_BALL_SPIN_SPEED)
    else:
        player.brain.nav.walk(0., 0., constants.FIND_BALL_SPIN_SPEED)

    return player.stay()

@superState('gameControllerResponder')
def spinToFoundBall(player):
    """
    spins to the ball until it is facing the ball 
    """
    if player.firstFrame():
        player.brain.tracker.trackBall()
        print "spinning to found ball"

    theta = degrees(player.brain.ball.bearing)
    spinToFoundBall.isFacingBall = fabs(theta) <= constants.FACING_BALL_ACCEPTABLE_BEARING

    if spinToFoundBall.isFacingBall:
        print "facing ball"
        return player.goNow('approachBall')

    # spins the appropriate direction
    if theta < 0.:
        player.brain.nav.walk(0., 0., -1*constants.FIND_BALL_SPIN_SPEED)
    else:
        player.brain.nav.walk(0., 0., constants.FIND_BALL_SPIN_SPEED)

    return player.stay()

@defaultState('backPedal')
@superState('gameControllerResponder')
@ifSwitchLater(transitions.shouldChaseBall, 'spinToFoundBall')
def findBallForward(player):
    pass

#TODO CONSTS
@superState('findBallForward')
def backPedal(player):
    if player.firstFrame():
        player.setWalk(-.6, 0., 0.)

    elif player.stateTime > 3:
        return player.goLater('spinForwardSearch')

    return player.stay()

#TODO CONSTS
@superState('findBallForward')
def spinForwardSearch(player):
    if player.firstFrame():
        my = player.brain.loc
        ball = Location(player.brain.ball.x, player.brain.ball.y)
        spinDir = my.spinDirToPoint(ball)
        player.setWalk(.1, .8*spinDir*-1, .3*spinDir*constants.FIND_BALL_SPIN_SPEED)
        player.brain.tracker.lookToAngle(spinDir*15)
    if transitions.spunOnce(player):
        return player.goLater('playOffBall')

    return player.stay()