import SharedTransitions
import BoxTransitions
from ..navigator import Navigator as nav
from ..util import *

@stay
@ifSwitch(SharedTransitions.navAtPosition, 'watchForBall')
@ifSwitch(BoxTransitions.ballInBox, 'chase')
def positionAtHome(player):
    """
    Go to the player's home position
    If ball-chasing conditions are met it will exit and go to chase
    """
    if player.firstFrame():
        player.brain.nav.goTo(player.homePosition, precision = nav.GENERAL_AREA,
                              speed = nav.QUICK_SPEED, avoidObstacles = True, 
                              fast = False, pb = False)

@stay
@ifSwitch(BoxTransitions.ballInBox, 'chase')
def watchForBall(player):
    """
    The player is at home, waiting for the ball to be within it's box (range)
    """
    if player.firstFrame():
        player.brain.tracker.trackBall()
