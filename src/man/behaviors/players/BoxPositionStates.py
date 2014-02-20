import SharedTransitions
import BoxTransitions
import noggin_constants as NogginConstants
from ..navigator import Navigator as nav
from ..util import *

@defaultState('positionAtHome')
@ifSwitch(boxTransitions.ballNotInBufferedBox, 'positionAtHome')
@ifSwitch(boxTransitions.shouldFindBall, 'findBall')
@superState('gameControllerResponder')
def planner(player):
    """
    Superstate for deciding what to do as a player.
    """
    pass

@stay
@ifSwitch(SharedTransitions.navAtPosition, 'watchForBall')
@ifSwitch(BoxTransitions.ballInBox, 'approachBall')
@superState('gameControllerResponder')
def positionAtHome(player):
    """
    Go to the player's home position
    If ball-chasing conditions are met it will exit and go to chase
    """
    if player.firstFrame():
        player.brain.nav.goTo(player.homePosition, precision = nav.GENERAL_AREA,
                              speed = nav.QUICK_SPEED, avoidObstacles = True,
                              fast = False, pb = False)
        # TODO check if this is necessary!
        #HACK so that tracker goes back to stopped.
        player.brain.tracker.stopHeadMoves()
        player.brain.tracker.repeatWidePan()

    ball = player.brain.ball
    loc = player.brain.loc
    ballDistance = ((loc.x - ball.x)**2 + (loc.y - ball.y)**2)**.5

    if ballDistance < 70 and ball.vis.frames_on > 2:
        player.brain.tracker.trackBall()
    else:
        player.brain.tracker.repeatWidePan()


@stay
@ifSwitch(BoxTransitions.tooFarFromHome(20), 'positionAtHome')
@ifSwitch(BoxTransitions.ballInBox, 'approachBall')
@superState('gameControllerResponder')
def watchForBall(player):
    """
    The player is at home, waiting for the ball to be within it's box (range)
    """
    if player.firstFrame():
        player.brain.nav.stand()
        player.brain.tracker.trackBall()
