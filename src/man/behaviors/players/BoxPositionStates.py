import SharedTransitions
import BoxTransitions
import noggin_constants as NogginConstants
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
        player.brain.tracker.stopHeadMoves() # HACK so that tracker goes back to stopped.
        player.brain.tracker.repeatWidePan()
        print "I'm going home!"

    ball = player.brain.ball
    loc = player.brain.loc
    ballDistance = ((loc.x - ball.x)**2 + (loc.y - ball.y)**2)**.5

    if ballDistance < 70 and ball.vis.frames_on >2:
        player.brain.tracker.trackBall()
    else:
        player.brain.tracker.repeatWidePan()


@stay
@ifSwitch(BoxTransitions.tooFarFromHome(20), 'positionAtHome')
@ifSwitch(BoxTransitions.ballInBox, 'chase')
def watchForBall(player):
    """
    The player is at home, waiting for the ball to be within it's box (range)
    """
    if player.firstFrame():
        player.brain.nav.stand()
        player.brain.tracker.trackBall()
