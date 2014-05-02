import SharedTransitions
import BoxPositionTransitions as transitions
import noggin_constants as NogginConstants
from ..navigator import Navigator as nav
from ..util import *

@superState('gameControllerResponder')
@stay
@ifSwitchNow(SharedTransitions.navAtPosition, 'watchForBall')
@ifSwitchNow(transitions.ballInBox, 'approachBall')
def positionAtHome(player):
    """
    Go to the player's home position
    If ball-chasing conditions are met it will exit and go to chase
    """
    if player.firstFrame():
        player.brain.nav.goTo(player.homePosition, precision = nav.GENERAL_AREA,
                              speed = nav.QUICK_SPEED, avoidObstacles = True,
                              fast = False, pb = False)
        player.brain.tracker.stopHeadMoves()
        player.brain.tracker.trackBall()

    ball = player.brain.ball
    loc = player.brain.loc
    ballDistance = ((loc.x - ball.x)**2 + (loc.y - ball.y)**2)**.5

    if ballDistance < 200 and ball.vis.frames_on > 2:
        player.brain.tracker.trackBall()
    elif player.brain.playerNumber != 4:
        player.brain.tracker.repeatWidePan()

@superState('gameControllerResponder')
@stay
@ifSwitchNow(transitions.tooFarFromHome(20), 'positionAtHome')
@ifSwitchNow(transitions.ballInBox, 'approachBall')
def watchForBall(player):
    """
    The player is at home, waiting for the ball to be within it's box (range)
    """
    if player.firstFrame():
        player.brain.nav.stand()
        player.brain.tracker.trackBall()
