import SharedTransitions as shared
import PlayOffBallTransitions as transitions
import PlayOffBallConstants as constants
import ChaseBallTransitions as chase
import ChaseBallConstants as chaseConstants
import ClaimTransitions as claims
import noggin_constants as NogginConstants
from ..navigator import Navigator as nav
from objects import RobotLocation
from objects import Location
from ..util import *
import random

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
    # TODO abstract role number properly
    if player.role == 4 or player.role == 5:
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
    # defenders position at midpoint between ball and goal
    if player.role == 2:
        waitForBallPosition = RobotLocation((player.brain.ball.x + BLUE_GOALBOX_RIGHT_X) * .5,
                                  (player.brain.ball.y + NogginConstants.MIDFIELD_Y) * .5,
                                  player.brain.ball.bearing_deg + player.brain.loc.h)
    # other defender will position between ball and midpoint between closest goalpost to ball
    # and middle of goal
    elif player.role == 3:
        if player.brain.ball.y < NogginConstants.MIDFIELD_Y:
            waitForBallPosition = RobotLocation((player.brain.ball.x + BLUE_GOALBOX_RIGHT_X)
                                                * .5, (player.brain.ball.y +
                                                       NogginConstants.GOALBOX_WIDTH * .25 +
                                                       NogginConstants.BLUE_GOALBOX_BOTTOM_Y)
                                                * .5, player.brain.ball.bearing_deg +
                                                player.brain.loc.h)

        elif player.brain.ball.y >=  NogginConstants.MIDFIELD_Y:
            waitForBallPosition = RobotLocation((player.brain.ball.x + BLUE_GOALBOX_RIGHT_X)
                                                * .5, (player.brain.ball.y +
                                                       NogginConstants.GOALBOX_WIDTH * .75 +
                                                       NogginConstants.BLUE_GOALBOX_BOTTOM_Y)
                                                * .5, player.brain.ball.bearing_deg +
                                                player.brain.loc.h)

    elif player.role == 4 or player.role == 5:
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

    player.brain.nav.goTo(waitForBallPosition, precision = nav.GENERAL_AREA,
                          speed = nav.QUICK_SPEED, avoidObstacles = True,
                          fast = False, pb = False)

@superState('playOffBall')
@stay
@ifSwitchNow(chase.shouldStopLookingForSharedBall, 'walkSearchFieldForBall')
def searchFieldForBall(player):
    """
    State used by chasers to search the field. Uses the shared ball if it is
    on. Moves to different quads of the field somewhat randomly.
    """
    player.brain.tracker.trackSharedBall()
    player.brain.nav.goTo(sharedball,
                          precision = nav.GENERAL_AREA,
                          speed = nav.QUICK_SPEED,
                          avoidObstacles = True,
                          fast = True, pb = False)

@superState('playOffBall')
@stay
@ifSwitchNow(transitions.shouldFindSharedBall, 'searchFieldForBall')
def walkSearchFieldForBall(player):
# Walk and search for ball, randomly walking to the center of each field quadrant

    if player.firstFrame():
        dest = constants.quad1Center

    player.brain.tracker.trackBall()
    player.brain.nav.goTo(dest, precision = nav.GENERAL_AREA,
                          speed = nav.QUICK_SPEED, avoidObstacles = True,
                          fast = True, pb = False)

    # update destination to send it to a new quadrant on the field
    # prearranged order; change or ranndomize?
    if shared.navAtPosition():
        if dest == constants.quad1Center:
            dest = constants.quad3Center
        elif dest == constants.quad3Center:
            dest = constants.quad2Center
        elif dest == constants.quad2Center:
            dest = constants.quad4Center
        elif dest == constants.quad4Center:
            dest = constants.quad1Center
