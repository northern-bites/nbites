import SharedTransitions as shared
import BoxPositionTransitions as transitions
import ChaseBallTransitions as chase
import ClaimTransitions as claims
import RoleConstants as role
import noggin_constants as NogginConstants
from ..navigator import Navigator as nav
from objects import RobotLocation
from ..util import *

# TODO Nikki -- rename this file

@defaultState('branchOnRole')
@superState('gameControllerResponder')
@ifSwitchNow(claims.shouldCedeClaim, 'positionAsSupporter')
@ifSwitchNow(transitions.shouldApproachBall, 'approachBall')
def playOffBall(player):
    """
    Superstate for all off ball play.
    """
    player.inKickingState = False

@superState('playOffBall')
def branchOnRole(player):
    """
    Chasers have different behavior than defenders, so we branch on
    role here.
    """
    if role.isChaser(player.role):
        player.goNow('searchFieldForBall')
    else:
        player.goNow('positionAtHome')

@superState('playOffBall')
@stay
@ifSwitchNow(shared.navAtPosition, 'watchForBall')
def positionAtHome(player):
    """
    Go to the player's home position
    If ball-chasing conditions are met it will exit and go to chase
    """
    if player.firstFrame():
        player.brain.tracker.trackBall()

    # TODO investigate whether this is the best way to move
    player.brain.nav.goTo(player.homePosition, precision = nav.GENERAL_AREA,
                          speed = nav.QUICK_SPEED, avoidObstacles = True,
                          fast = False, pb = False)

@superState('playOffBall')
@stay
@ifSwitchNow(transitions.tooFarFromHome(20), 'positionAtHome')
def watchForBall(player):
    """
    The player is at home, waiting for the ball to be within it's box (range)
    """
    if player.firstFrame():
        player.brain.nav.stand()

@superState('playOffBall')
@stay
@ifSwitchLater(shared.ballOffForNFrames(30), 'branchOnRole')
def positionAsSupporter(player):
    # TODO Nikki -- defenders should position between ball and goal
    if role.isDefender(player.role):
        pass
    elif role.isChaser(player.role):
        if (player.brain.ball.x > player.brain.loc.x and
            player.brain.ball.y > player.brain.loc.y):
            waitForBallPosition = RobotLocation(player.brain.ball.x - 60,
                                                player.brain.ball.y - 60,
                                                player.brain.ball.bearing_deg + player.brain.loc.h)
        elif (player.brain.ball.x > player.brain.loc.x and
            player.brain.ball.y < player.brain.loc.y):
            waitForBallPosition = RobotLocation(player.brain.ball.x - 60,
                                                player.brain.ball.y + 60,
                                                player.brain.ball.bearing_deg + player.brain.loc.h)
        elif (player.brain.ball.x < player.brain.loc.x and
            player.brain.ball.y > player.brain.loc.y):
            waitForBallPosition = RobotLocation(player.brain.ball.x + 60,
                                                player.brain.ball.y - 60,
                                                90)
        elif (player.brain.ball.x < player.brain.loc.x and
            player.brain.ball.y < player.brain.loc.y):
            waitForBallPosition = RobotLocation(player.brain.ball.x + 60,
                                                player.brain.ball.y + 60,
                                                -90)
    # TODO cherry picker needs supporter position
    elif role.isCherryPicker(player.role):
        pass

    player.brain.nav.goTo(waitForBallPosition, precision = nav.GENERAL_AREA,
                          speed = nav.QUICK_SPEED, avoidObstacles = True,
                          fast = False, pb = False)

@superState('playOffBall')
def searchFieldForBall(player):
    """
    State used by chasers to search the field. Uses the shared ball if it is
    on. Moves to different quads of the field somewhat randomly.
    """
    # TODO Nikki -- your code goes here
    pass
