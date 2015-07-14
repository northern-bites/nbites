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
@ifSwitchNow(transitions.shouldSpinSearchFromWatching, 'doFirstHalfSpin')
def positionAtHome(player):
    """
    Go to the player's home position. Players look in the direction of the 
    shared ball if it is on with reliability >= 1.
    """
    if player.brain.ball.vis.frames_off < 10:
        ball = player.brain.ball
        bearing = ball.bearing_deg
    elif player.brain.sharedBall.ball_on:
        ball = player.brain.sharedBall
        bearing = degrees(atan2(ball.y - player.brain.loc.y,
                          ball.x - player.brain.loc.x)) - player.brain.loc.h
    else:
        ball = None

    if ball != None and not (role.isDefender(player.role) and NogginConstants.FIXED_D_HOME):
        if role.isLeftDefender(player.role):
            home = findDefenderHome(True, ball, bearing + player.brain.loc.h)
        elif role.isRightDefender(player.role):
            home = findDefenderHome(False, ball, bearing + player.brain.loc.h)
        elif role.isStriker(player.role):
            home = findStrikerHome(ball, bearing + player.brain.loc.h)
        else:
            home = player.homePosition
    else:
        home = player.homePosition

    if player.firstFrame():
        player.brain.tracker.trackBall()
        fastWalk = role.isChaser(player.role)
        player.brain.nav.goTo(home, precision = nav.HOME,
                              speed = nav.QUICK_SPEED, avoidObstacles = True,
                              fast = fastWalk, pb = False)

    player.brain.nav.updateDest(home)

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
        my = player.brain.loc
        ball = Location(player.brain.ball.x, player.brain.ball.y)
        spinDir = my.spinDirToPoint(ball)
        player.setWalk(0, 0, spinDir*Navigator.QUICK_SPEED)
        player.brain.tracker.lookToSpinDirection(spinDir)

    while player.stateTime < constants.SPUN_ONCE_TIME_THRESH / 2:
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
        my = player.brain.loc
        ball = Location(player.brain.ball.x, player.brain.ball.y)
        spinDir = -my.spinDirToPoint(ball)
        player.setWalk(0, 0, spinDir*Navigator.QUICK_SPEED)
        player.brain.tracker.lookToSpinDirection(spinDir)

    while player.stateTime < constants.SPUN_ONCE_TIME_THRESH / 2:
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
@ifSwitchNow(transitions.shouldFindSharedBall, 'searchFieldForSharedBall')
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
quad2Center = Location(NogginConstants.CENTER_FIELD_X * 1.4, NogginConstants.CENTER_FIELD_Y * 1.4)
quad3Center = Location(NogginConstants.CENTER_FIELD_X * 1.4, NogginConstants.CENTER_FIELD_Y * .6)
quad4Center = Location(NogginConstants.CENTER_FIELD_X * .6, NogginConstants.CENTER_FIELD_Y * 1.4)
points = [quad1Center, quad2Center, quad3Center, quad4Center]
