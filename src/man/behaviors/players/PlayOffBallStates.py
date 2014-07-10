import SharedTransitions as shared
import PlayOffBallTransitions as transitions
import RoleConstants as role
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
@ifSwitchNow(transitions.shouldBeSupporter, 'positionAsSupporter')
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
        return player.goNow('searchFieldForBall')
    else:
        return player.goNow('positionAtHome')

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
    # defenders position at midpoint between ball and goal
    if role.isLeftDefender(player.role):
        if player.brain.ball.y < NogginConstants.MIDFIELD_Y:
            waitForBallPosition = RobotLocation((player.brain.ball.x + NogginConstants.BLUE_GOALBOX_RIGHT_X)
                                                * .5, (player.brain.ball.y +
                                                       NogginConstants.GOALBOX_WIDTH * .25 +
                                                       NogginConstants.BLUE_GOALBOX_BOTTOM_Y)
                                                * .5, player.brain.ball.bearing_deg +
                                                player.brain.loc.h)
        else:
            waitForBallPosition = RobotLocation((player.brain.ball.x + NogginConstants.BLUE_GOALBOX_RIGHT_X)*.5,
                                                (player.brain.ball.y + NogginConstants.MIDFIELD_Y)*.5,
                                                player.brain.ball.bearing_deg + player.brain.loc.h)

    # other defender will position between ball and midpoint between closest goalpost to ball
    # and middle of goal
    elif role.isRightDefender(player.role):
        if player.brain.ball.y >=  NogginConstants.MIDFIELD_Y:
            waitForBallPosition = RobotLocation((player.brain.ball.x + NogginConstants.BLUE_GOALBOX_RIGHT_X)
                                                * .5, (player.brain.ball.y +
                                                       NogginConstants.GOALBOX_WIDTH * .75 +
                                                       NogginConstants.BLUE_GOALBOX_BOTTOM_Y)
                                                * .5, player.brain.ball.bearing_deg +
                                                player.brain.loc.h)

        else:
            waitForBallPosition = RobotLocation((player.brain.ball.x + NogginConstants.BLUE_GOALBOX_RIGHT_X)*.5,
                                                (player.brain.ball.y + NogginConstants.MIDFIELD_Y)*.5,
                                                player.brain.ball.bearing_deg + player.brain.loc.h)

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

    if role.isChaser(player.role):
        fast = True
    else:
        fast = False

    player.brain.nav.goTo(waitForBallPosition, precision = nav.GENERAL_AREA,
                          speed = nav.QUICK_SPEED, avoidObstacles = True,
                          fast = fast, pb = False)

@superState('playOffBall')
@stay
@ifSwitchNow(transitions.shouldStopLookingForSharedBall, 'walkSearchFieldForBall')
def searchFieldForBall(player):
    """
    State used by chasers to search the field. Uses the shared ball if it is
    on. Moves to different quads of the field somewhat randomly.
    """
    if player.firstFrame():
        player.brain.tracker.trackBall()

    sharedball = Location(player.brain.sharedBall.x, player.brain.sharedBall.y)
    player.brain.nav.goTo(sharedball,
                          precision = nav.GENERAL_AREA,
                          speed = nav.QUICK_SPEED,
                          avoidObstacles = True,
                          fast = True, pb = False)

@superState('playOffBall')
@stay
@ifSwitchNow(transitions.shouldFindSharedBall, 'searchFieldForBall')
def walkSearchFieldForBall(player):
    """
    Walk and search for ball, randomly walking to the center of each field 
    quadrant.
    """
    quad1Center = Location(NogginConstants.CENTER_FIELD_X * .5, NogginConstants.CENTER_FIELD_Y * .5)
    quad2Center = Location(NogginConstants.CENTER_FIELD_X * .5, NogginConstants.CENTER_FIELD_Y * 1.5)
    quad3Center = Location(NogginConstants.CENTER_FIELD_X * 1.5, NogginConstants.CENTER_FIELD_Y * 1.5)
    quad4Center = Location(NogginConstants.CENTER_FIELD_X * 1.5, NogginConstants.CENTER_FIELD_Y * .5)

    if player.firstFrame():
        player.brain.tracker.trackBall()
        walkSearchFieldForBall.dest = quad3Center

    # update destination to send it to a new quadrant on the field
    # prearranged order; change or ranndomize?
    if shared.navAtPosition(player):
        if walkSearchFieldForBall.dest == quad1Center:
            walkSearchFieldForBall.dest = quad2Center
        elif walkSearchFieldForBall.dest == quad3Center:
            walkSearchFieldForBall.dest = quad1Center
        elif walkSearchFieldForBall.dest == quad2Center:
            walkSearchFieldForBall.dest = quad4Center
        elif walkSearchFieldForBall.dest == quad4Center:
            walkSearchFieldForBall.dest = quad3Center

    player.brain.nav.goTo(walkSearchFieldForBall.dest, precision = nav.GRAINY,
                          speed = nav.QUICK_SPEED, avoidObstacles = True,
                          fast = True, pb = False)
