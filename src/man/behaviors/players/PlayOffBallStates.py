import SharedTransitions as shared
import PlayOffBallTransitions as transitions
import RoleConstants as role
import ChaseBallTransitions as chase
import ChaseBallConstants as chaseConstants
import ClaimTransitions as claims
from SupporterConstants import getSupporterPosition, CHASER_DISTANCE, findStrikerHome, findDefenderHome
import noggin_constants as NogginConstants
from ..navigator import Navigator as nav
from objects import Location, RobotLocation
from ..util import *
from math import hypot, fabs, atan2, degrees
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
        if transitions.shouldFindSharedBall(player):
            return player.goNow('searchFieldForSharedBall')
        return player.goNow('positionAtHome')
    return player.goNow('positionAtHome')

@superState('playOffBall')
@stay
@ifSwitchNow(shared.navAtPosition, 'watchForBall')
def positionAtHome(player):
    """
    Go to the player's home position. Defenders look in the direction of the 
    shared ball if it is on with reliability >= 2. Cherry pickers look in the direction
    of the shared ball if it is on with reliability >= 1.
    """

    if role.isFirstChaser(player.role) and transitions.shouldFindSharedBall(player):
        return player.goLater('searchFieldForSharedBall')

    if player.brain.ball.vis.frames_off < 10:
        ball = player.brain.ball
        bearing = ball.bearing_deg
    elif player.brain.sharedBall.ball_on:
        ball = player.brain.sharedBall
        bearing = degrees(atan2(ball.y - player.brain.loc.y,
                        ball.x - player.brain.loc.x)) - player.brain.loc.h
    else:
        ball = None
        home = player.homePosition

    if ball != None:
        if role.isLeftDefender(player.role):
            home = findDefenderHome(True, ball, bearing + player.brain.loc.h)
        elif role.isRightDefender(player.role):
            home = findDefenderHome(False, ball, bearing + player.brain.loc.h)
        elif role.isStriker(player.role):
            home = findStrikerHome(ball, bearing + player.brain.loc.h)
        else:
            home = player.homePosition

    if player.firstFrame():
        if role.isCherryPicker(player.role):
            player.brain.tracker.repeatWidePan()
        else:
            player.brain.tracker.trackBall()
        
        fastWalk = role.isCherryPicker(player.role)
        player.brain.nav.goTo(home, precision = nav.HOME,
                              speed = nav.QUICK_SPEED, avoidObstacles = True,
                              fast = fastWalk, pb = False)

    player.brain.nav.updateDest(home)

@superState('playOffBall')
@stay
def watchForBall(player):
    """
    The player is at home, waiting for the ball to be within it's box (range)
    """
    if player.firstFrame():
        player.brain.tracker.trackBall()
        player.brain.nav.stand()

    if transitions.tooFarFromHome(20, player):
        return player.goNow('positionAtHome')

@superState('playOffBall')
@stay
@ifSwitchLater(shared.ballOffForNFrames(120), 'playOffBall')
def positionAsSupporter(player):
    if (role.isChaser(player.role) and role.isChaser(player.roleOfClaimer) and 
        player.brain.ball.distance > hypot(CHASER_DISTANCE, CHASER_DISTANCE)):
        fast = True
    else:
        fast = False

    positionAsSupporter.position = getSupporterPosition(player, player.role)

    if player.firstFrame():
        player.brain.tracker.trackBall()

        player.brain.nav.goTo(positionAsSupporter.position, precision = nav.GENERAL_AREA,
                              speed = nav.QUICK_SPEED, avoidObstacles = True,
                              fast = False, pb = False)

    if positionAsSupporter.position.distTo(player.brain.loc) > 20:
        player.brain.nav.goTo(positionAsSupporter.position, precision = nav.GENERAL_AREA,
                              speed = nav.QUICK_SPEED, avoidObstacles = True,
                              fast = False, pb = False)
    
    player.brain.nav.updateDest(positionAsSupporter.position, fast=fast)

@superState('playOffBall')
@stay
@ifSwitchNow(transitions.shouldFindFlippedSharedBall, 'searchFieldForFlippedSharedBall')
@ifSwitchNow(transitions.shouldStopLookingForSharedBall, 'positionAtHome')
def searchFieldForSharedBall(player):
    """
    Searches the field for the shared ball.
    """
    sharedball = Location(player.brain.sharedBall.x, player.brain.sharedBall.y)

    if player.firstFrame():
        player.brain.tracker.trackBall()
        player.brain.tracker.repeatWidePan()
        player.sharedBallCloseCount = 0
        player.sharedBallOffCount = 0
        player.brain.nav.goTo(sharedball, precision = nav.GENERAL_AREA,
                              speed = nav.QUICK_SPEED, avoidObstacles = True,
                              fast = True, pb = False)

    if sharedball.distTo(player.brain.loc) < 100:
        player.sharedBallCloseCount += 1
    else:
        player.sharedBallCloseCount = 0

    if not transitions.shouldFindSharedBall(player):
        player.sharedBallOffCount += 1
    else:
        player.sharedBallOffCount = 0

    player.brain.nav.updateDest(sharedball)

@superState('playOffBall')
@stay
@ifSwitchNow(transitions.shouldStopLookingForFlippedSharedBall, 'positionAtHome')
def searchFieldForFlippedSharedBall(player):
    """
    Flips the shared ball and searches for it.
    """
    sharedball = Location(-1*(player.brain.sharedBall.x-NogginConstants.MIDFIELD_X) + NogginConstants.MIDFIELD_X,
                          -1*(player.brain.sharedBall.y-NogginConstants.MIDFIELD_Y) + NogginConstants.MIDFIELD_Y)

    if player.firstFrame():
        player.brain.tracker.trackBall()
        player.brain.tracker.repeatWidePan()
        player.sharedBallCloseCount = 0
        player.brain.nav.goTo(sharedball, precision = nav.GENERAL_AREA,
                              speed = nav.QUICK_SPEED, avoidObstacles = True,
                              fast = True, pb = False)

    if sharedball.distTo(player.brain.loc) < 100:
        player.sharedBallCloseCount += 1
    else:
        player.sharedBallCloseCount = 0

    if not transitions.shouldFindSharedBall(player):
        player.sharedBallOffCount += 1
    else:
        player.sharedBallOffCount = 0

    player.brain.nav.updateDest(sharedball)

@superState('playOffBall')
@stay
@ifSwitchNow(transitions.shouldFindSharedBall, 'searchFieldForSharedBall')
def searchFieldByQuad(player):
    """
    Search the field quadrant by quadrant. Choose first quadrant by loc heading.
    """
    if player.firstFrame():
        player.brain.tracker.trackBall()
        player.brain.tracker.repeatWidePan()
        searchFieldByQuad.dest = min(points, key=lambda x:fabs(player.brain.loc.getRelativeBearing(x)))
        player.brain.nav.goTo(searchFieldByQuad.dest, precision = nav.GRAINY,
                          speed = nav.QUICK_SPEED, avoidObstacles = True,
                          fast = True, pb = False)

    if shared.navAtPosition(player):
        if searchFieldByQuad.dest == quad1Center:
            searchFieldByQuad.dest = quad2Center
        elif searchFieldByQuad.dest == quad3Center:
            searchFieldByQuad.dest = quad1Center
        elif searchFieldByQuad.dest == quad2Center:
            searchFieldByQuad.dest = quad4Center
        elif searchFieldByQuad.dest == quad4Center:
            searchFieldByQuad.dest = quad3Center

    player.brain.nav.updateDest(searchFieldByQuad.dest)

quad1Center = Location(NogginConstants.CENTER_FIELD_X * .6, NogginConstants.CENTER_FIELD_Y * .6)
quad2Center = Location(NogginConstants.CENTER_FIELD_X * .6, NogginConstants.CENTER_FIELD_Y * 1.4)
quad3Center = Location(NogginConstants.CENTER_FIELD_X * 1.4, NogginConstants.CENTER_FIELD_Y * 1.4)
quad4Center = Location(NogginConstants.CENTER_FIELD_X * 1.4, NogginConstants.CENTER_FIELD_Y * .6)
points = [quad1Center, quad2Center, quad3Center, quad4Center]
