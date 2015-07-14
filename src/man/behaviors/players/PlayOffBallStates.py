import SharedTransitions as shared
import PlayOffBallTransitions as transitions
import RoleConstants as role
import ChaseBallTransitions as chase
import ChaseBallConstants as chaseConstants
import ClaimTransitions as claims
from SupporterConstants import getSupporterPosition, CHASER_DISTANCE, findStrikerHome, findDefenderHome, calculateHomePosition
import noggin_constants as NogginConstants
from ..navigator import Navigator as nav
from objects import Location, RobotLocation
from ..util import *
from math import hypot, fabs, atan2, degrees
import random

@defaultState('positionAtHome')
@superState('gameControllerResponder')
@ifSwitchNow(transitions.shouldFindSharedBall, 'searchFieldForSharedBall')
@ifSwitchNow(transitions.shouldBeSupporter, 'positionAsSupporter')
@ifSwitchNow(transitions.shouldApproachBall, 'approachBall')
def playOffBall(player):
    """
    Superstate for all off ball play.
    """
    player.inKickingState = False

@superState('playOffBall')
@stay
@ifSwitchNow(shared.navAtPosition, 'watchForBall')
def positionAtHome(player):
    """
    Go to the player's home position.
    """
    home = calculateHomePosition(player)

    if player.firstFrame():
        player.brain.tracker.trackBall()
        fastWalk = role.isChaser(player.role)
        player.brain.nav.goTo(home, precision = nav.HOME,
                              speed = nav.QUICK_SPEED, avoidObstacles = True,
                              fast = fastWalk, pb = False)

    player.brain.nav.updateDest(home)

@superState('playOffBall')
@stay
@ifSwitchNow(transitions.shouldSpinSearchFromWatching, 'spinAtHome')
def watchForBall(player):
    """
    The player is at home, waiting for the ball to be within box.
    """
    if player.firstFrame():
        player.brain.tracker.trackBall()
        player.brain.nav.stand()

    if transitions.tooFarFromHome(player, 50, 20):
        return player.goLater('positionAtHome')

@defaultState('doFirstHalfSpin')
@superState('playOffBall')
@ifSwitchNow(transitions.stopSpinning, 'positionAtHome')
def spinAtHome(player):
    """
    Spin while at home.
    """
    pass

@superState('spinAtHome')
def doFirstHalfSpin(player):
    """
    Spin to where we think the ball is.
    """
    if player.firstFrame():
        player.setWalk(0, 0, nav.QUICK_SPEED)
        player.brain.tracker.lookToSpinDirection(1)

    while player.stateTime < chaseConstants.SPUN_ONCE_TIME_THRESH / 2:
        return player.stay()

    return player.goNow('doPan')

@superState('spinAtHome')
def doPan(player):
    """
    Wide pan for 5 seconds.
    """
    if player.firstFrame():
        player.stand()
        player.brain.tracker.repeatWidePan()

    while player.stateTime < 5:
        return player.stay()

    return player.goNow('doSecondHalfSpin')

@superState('spinAtHome')
def doSecondHalfSpin(player):
    """
    Keep spinning in the same direction.
    """
    if player.firstFrame():
        player.setWalk(0, 0, nav.QUICK_SPEED)
        player.brain.tracker.lookToSpinDirection(1)

    while player.stateTime < chaseConstants.SPUN_ONCE_TIME_THRESH / 2:
        return player.stay()

    if role.isFirstChaser(player.role):
        return player.goNow('searchFieldByQuad')
    return player.goNow('playOffBall')

@superState('playOffBall')
@stay
@ifSwitchLater(shared.ballOffForNFrames(120), 'playOffBall')
def positionAsSupporter(player):
    """
    Position to support teammate with claim.
    """
    positionAsSupporter.position = getSupporterPosition(player, player.role)
    fastWalk = role.isChaser(player.role)

    if player.firstFrame():
        player.brain.tracker.trackBall()

        player.brain.nav.goTo(positionAsSupporter.position, precision = nav.GENERAL_AREA,
                              speed = nav.QUICK_SPEED, avoidObstacles = True,
                              fast = fastWalk, pb = False)

    if positionAsSupporter.position.distTo(player.brain.loc) > 20:
        player.brain.nav.goTo(positionAsSupporter.position, precision = nav.GENERAL_AREA,
                              speed = nav.QUICK_SPEED, avoidObstacles = True,
                              fast = fastWalk, pb = False)
    
    player.brain.nav.updateDest(positionAsSupporter.position, fast = fastWalk)

@superState('playOffBall')
@stay
@ifSwitchNow(transitions.noBallFoundAtSharedBall, 'positionAtHome')
@ifSwitchNow(transitions.shouldFindFlippedSharedBall, 'searchFieldForFlippedSharedBall')
@ifSwitchNow(transitions.shouldStopLookingForSharedBall, 'positionAtHome')
def searchFieldForSharedBall(player):
    """
    Searches the field for the shared ball.
    """
    sharedball = Location(player.brain.sharedBall.x, player.brain.sharedBall.y)

    if player.firstFrame():
        player.brain.tracker.trackBall()
        player.brain.tracker.repeatBasicPan()
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
        player.brain.tracker.repeatBasicPan()
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
def searchFieldByQuad(player):
    """
    Search the field quadrant by quadrant. Choose first quadrant by loc heading.
    """
    if player.firstFrame():
        player.brain.tracker.trackBall()
        player.brain.tracker.repeatBasicPan()
        searchFieldByQuad.dest = min(points, key=lambda x:fabs(player.brain.loc.distTo(x)))
        player.brain.nav.goTo(searchFieldByQuad.dest, precision = nav.GRAINY,
                          speed = nav.QUICK_SPEED, avoidObstacles = True,
                          fast = True, pb = False)
        searchFieldByQuad.quadIndex = points.index(searchFieldByQuad.dest)
        searchFieldByQuad.quadsWalked = 0

    if shared.navAtPosition(player) and player.brain.loc.distTo(searchFieldByQuad.dest) < 60:
        searchFieldByQuad.quadIndex += 1
        searchFieldByQuad.quadsWalked += 1
        searchFieldByQuad.dest = points[searchFieldByQuad.quadIndex % len(points)]

    if searchFieldByQuad.quadsWalked > 3:
        return player.goLater('playOffBall')

    player.brain.nav.updateDest(searchFieldByQuad.dest)

quad1Center = Location(NogginConstants.CENTER_FIELD_X * .6, NogginConstants.CENTER_FIELD_Y * .6)
quad2Center = Location(NogginConstants.CENTER_FIELD_X * 1.4, NogginConstants.CENTER_FIELD_Y * 1.4)
quad3Center = Location(NogginConstants.CENTER_FIELD_X * 1.4, NogginConstants.CENTER_FIELD_Y * .6)
quad4Center = Location(NogginConstants.CENTER_FIELD_X * .6, NogginConstants.CENTER_FIELD_Y * 1.4)
points = [quad1Center, quad2Center, quad3Center, quad4Center]
