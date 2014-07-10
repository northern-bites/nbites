import SharedTransitions as shared
import PlayOffBallTransitions as transitions
import RoleConstants as role
import ChaseBallTransitions as chase
import ChaseBallConstants as chaseConstants
import ClaimTransitions as claims
from SupporterConstants import getSupporterPosition
import noggin_constants as NogginConstants
from ..navigator import Navigator as nav
from objects import Location, RobotLocation
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
        return player.goNow('searchFieldForSharedBall')
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
    if player.firstFrame():
        position = getSupporterPosition(player, player.role)
        player.brain.nav.goTo(position, precision = nav.GENERAL_AREA,
                              speed = nav.QUICK_SPEED, avoidObstacles = True,
                              fast = False, pb = False)

@superState('playOffBall')
@stay
@ifSwitchNow(transitions.shouldStopLookingForSharedBall, 'searchFieldByQuad')
def searchFieldForSharedBall(player):
    """
    State used by chasers to search the field. Uses the shared ball if it is
    on and reliability score is high enough.
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
@ifSwitchNow(transitions.shouldFindSharedBall, 'searchFieldForSharedBall')
def searchFieldByQuad(player):
    """
    Search the field quadrant by quadrant. Choose first quadrant by shared ball
    if it is on.
    """
    quad1Center = Location(NogginConstants.CENTER_FIELD_X * .5, NogginConstants.CENTER_FIELD_Y * .5)
    quad2Center = Location(NogginConstants.CENTER_FIELD_X * .5, NogginConstants.CENTER_FIELD_Y * 1.5)
    quad3Center = Location(NogginConstants.CENTER_FIELD_X * 1.5, NogginConstants.CENTER_FIELD_Y * 1.5)
    quad4Center = Location(NogginConstants.CENTER_FIELD_X * 1.5, NogginConstants.CENTER_FIELD_Y * .5)

    if player.firstFrame():
        player.brain.tracker.trackBall()
        searchFieldByQuad.dest = quad3Center

    # update destination to send it to a new quadrant on the field
    # prearranged order; change or ranndomize?
    if shared.navAtPosition(player):
        if searchFieldByQuad.dest == quad1Center:
            searchFieldByQuad.dest = quad2Center
        elif searchFieldByQuad.dest == quad3Center:
            searchFieldByQuad.dest = quad1Center
        elif searchFieldByQuad.dest == quad2Center:
            searchFieldByQuad.dest = quad4Center
        elif searchFieldByQuad.dest == quad4Center:
            searchFieldByQuad.dest = quad3Center

    player.brain.nav.goTo(searchFieldByQuad.dest, precision = nav.GRAINY,
                          speed = nav.QUICK_SPEED, avoidObstacles = True,
                          fast = True, pb = False)
