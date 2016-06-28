import SharedTransitions as shared
import PlayOffBallTransitions as transitions
import RoleConstants as role
import ChaseBallTransitions as chase
import ChaseBallConstants as chaseConstants
import ClaimTransitions as claims
from SupporterConstants import getSupporterPosition, CHASER_DISTANCE, calculateHomePosition
import noggin_constants as NogginConstants
from ..headTracker import TrackingConstants as tracking
from ..navigator import Navigator as nav
from ..navigator import BrunswickSpeeds as speeds
from objects import Location, RobotLocation, RelRobotLocation
from ..util import *
from math import hypot, fabs, atan2, degrees
from ..headTracker import HeadMoves
import random

leftDefenderIsForward = True

# IMPORTANT China 2015 bug found
# TODO fix oscillation between positionAtHome and positionAsSupporter

@defaultState('branchOnRole')
@superState('gameControllerResponder')
@ifSwitchNow(transitions.shouldFindSharedBall, 'searchFieldForSharedBall')
@ifSwitchNow(transitions.shouldBeSupporter, 'positionAsSupporter')
@ifSwitchNow(transitions.shouldApproachBall, 'approachBall')
def playOffBall(player):
    """
    Superstate for all off ball play.
    """
    player.inKickingState = False

#USOPEN2016: Back to Search Field by Quad
@superState('playOffBall')
def branchOnRole(player):
    """
    Chasers are going to have a different behavior again.
    We will branch on behavior based on role here
    """

    # print "Entered Branch on Role"
    # print "----- evenDefenderIsForward, lastEvenDefenderForwardVal ----"
    # print player.brain.evenDefenderIsForward, lastEvenDefenderForwardVal

    # global lastEvenDefenderForwardVal
    # player.brain.evenDefenderIsForward = not lastEvenDefenderForwardVal
    # newEvenDefenderForwardVal = True
    # print("TIME SINCE PLAYING:", player.brain.gameController.timeSincePlaying)
    if role.isFirstChaser(player.role):
        if transitions.shouldFindSharedBall(player) and player.brain.gameController.timeSincePlaying > 75:
            return player.goNow('searchFieldForSharedBall')
        return player.goNow('playerFourSearchBehavior')
    elif role.isStriker(player.role):
        return player.goNow('playerFiveSearchBehavior')
    elif role.isLeftDefender(player.role):

        # print "Player Brain Left Forward 1: " + str(leftDefenderIsForward)

        # if (player.brain.sharedBall.ball_on) and (player.brain.sharedBall.x < NogginConstants.MIDFIELD_X):

            # print "WE ARE IN HERE"

            # return player.goNow('leftDefenderBack')
        if leftDefenderIsForward:

            # print "Changing to False"

            global leftDefenderIsForward

            leftDefenderIsForward = False

            # print "Player Brain Left Forward 2: " + str(leftDefenderIsForward)

            return player.goNow('leftDefenderForward')
        else:

            # print "Changing to True"

            global leftDefenderIsForward

            leftDefenderIsForward = True

            # print "Player Brain Left Forward 3: " + str(leftDefenderIsForward)

            return player.goNow('leftDefenderBack')
    else:
        return player.goNow('positionAtHome')

    # elif player.brain.sharedBall.ball_on and player.brain.sharedBall.x < NogginConstants.MIDFIELD_X:
    #     print "----- evenDefenderIsForward, lastEvenDefenderForwardVal ----"
    #     print player.brain.evenDefenderIsForward, lastEvenDefenderForwardVal
    #     return player.goNow('bothDefendersBack')

    # elif role.isLeftDefender(player.role):
    #     newEvenDefenderForwardVal = player.brain.teamMembers[3-1].evenDefenderIsForward
    # else:
    #     newEvenDefenderForwardVal = player.brain.teamMembers[2-1].evenDefenderIsForward

    # if lastEvenDefenderForwardVal is not newEvenDefenderForwardVal:
    #     lastEvenDefenderForwardVal = newEvenDefenderForwardVal
    #     player.brain.evenDefenderIsForward = lastEvenDefenderForwardVal

    # print "----- evenDefenderIsForward, lastEvenDefenderForwardVal ----"
    # print player.brain.evenDefenderIsForward, lastEvenDefenderForwardVal

    # if lastEvenDefenderForwardVal:
    #     return player.goNow('evenDefenderForward')
    # else:
    #     return player.goNow('oddDefenderForward')

# @superState('playOffBall')
# @stay
# @ifSwitchNow(shared.navAtPosition, 'watchForBall')
# def positionAtDefenderHome(player):
#     """
#     Go to the player's home position.
#     """

#     home = calculateHomePosition(player)

#     if player.firstFrame():
#         player.brain.tracker.trackBall()
#         fastWalk = role.isChaser(player.role)
#         player.brain.nav.goTo(home, precision = nav.GRAINY,
#                               speed = speeds.SPEED_EIGHT, avoidObstacles = True,
#                               fast = fastWalk, pb = False)

#     if home.distTo(player.brain.loc) > 90:
#         player.brain.nav.updateDest(home)

#     print home.x, home.y, home.h

@superState('playOffBall')
@stay
@ifSwitchNow(shared.navAtPosition, 'watchForBall')
def positionAtHome(player):
    """
    Go to the player's home position.
    """

    home = player.homePosition

    if player.firstFrame():
        player.brain.tracker.trackBall()
        fastWalk = role.isChaser(player.role)
        player.brain.nav.goTo(home, precision = nav.HOME,
                              speed = speeds.SPEED_EIGHT, avoidObstacles = True,
                              fast = fastWalk, pb = False)

    player.brain.nav.updateDest(home)

@superState('playOffBall')
@stay
# @ifSwitchNow(transitions.shouldChangeDefenderPosition, 'positionAtHome')
# @ifSwitchNow(transitions.ballInOurHalf, 'playOffBall')
@ifSwitchNow(transitions.shouldSpinSearchFromWatching, 'spinAtHome')
def watchForBall(player):
    """
    The player is at home, waiting for the ball to be within box.
    """

    if player.firstFrame():
        print "-----------Player at home-----------"
        player.brain.tracker.trackBall()
        player.brain.nav.stand()

    # I commented this out because we were getting strange oscillations
    # between this and positonAtHome, and honestly we never go here unless
    # we are already at home... dumb...
    # if transitions.tooFarFromHome(player, 50, 20):
    #     return player.goLater('positionAtHome')

    if role.isFirstChaser(player.role):
        if player.stateTime >= tracking.INITIALIZE_HEADING_TIME + tracking.FULL_WIDE_PAN_TIME:
                return player.goNow('spinAtHome')
    elif role.isStriker(player.role):
        if player.stateTime >= tracking.INITIALIZE_HEADING_TIME + tracking.FULL_WIDE_PAN_TIME * 2:
            return player.goNow('spinAtHome')
    else:
        if player.stateTime >= tracking.INITIALIZE_HEADING_TIME + tracking.FULL_WIDE_PAN_TIME * 2:
            return player.goNow('spinAtHome')

@defaultState('doFirstHalfSpin')
@superState('playOffBall')
# @ifSwitchNow(transitions.shouldChangeDefenderPosition, 'positionAtHome')
# @ifSwitchNow(transitions.ballInOurHalf, 'playOffBall')
# @ifSwitchNow(transitions.stopSpinning, 'positionAtHome')
def spinAtHome(player):
    """
    Spin while at home.
    """

    pass

@superState('spinAtHome')
@stay
def doFirstHalfSpin(player):
    """
    Spin to where we think the ball is.
    """

    if player.firstFrame():
        player.brain.tracker.repeatFixedPitchLookAhead()

        if player.brain.playerNumber == 3:
            player.setWalk(0, 0, speeds.SPEED_SIX)
        else:
            player.setWalk(0, 0, -speeds.SPEED_SIX)

    if player.stateTime >= chaseConstants.SPEED_SIX_SPUN_ONCE_TIME / 2:
        return player.goNow('doPan')

@superState('spinAtHome')
# @ifSwitchNow(transitions.shouldChangeDefenderPosition, 'positionAtHome')
# @ifSwitchNow(transitions.ballInOurHalf, 'playOffBall')
@stay
def doPan(player):
    """
    Wide pan for 5 seconds.
    """

    player.brain.defendingStateTime += 1

    if player.firstFrame():
        print "------------Doing Pan-------------"

        player.stand()
        player.brain.tracker.trackBall()

    if role.isFirstChaser(player.role):
        if player.stateTime >= tracking.INITIALIZE_HEADING_TIME + tracking.FULL_WIDE_PAN_TIME:
                return player.goNow('playerFourSearchBehavior')
    elif role.isStriker(player.role):
        if player.stateTime >= tracking.INITIALIZE_HEADING_TIME + tracking.FULL_WIDE_PAN_TIME:
            return player.goNow('playerFiveSearchBehavior')
    else:
        if player.stateTime >= tracking.INITIALIZE_HEADING_TIME + tracking.FULL_WIDE_PAN_TIME:
            return player.goNow('doSecondHalfSpin')

@superState('spinAtHome')
@stay
def doSecondHalfSpin(player):
    """
    Keep spinning in the same direction.
    """

    player.brain.defendingStateTime += 1

    if player.firstFrame():
        player.brain.tracker.repeatFixedPitchLookAhead()

        if player.brain.playerNumber == 3:
            player.setWalk(0, 0, speeds.SPEED_SIX)
        else:
            player.setWalk(0, 0, -speeds.SPEED_SIX)

    if player.stateTime > chaseConstants.SPEED_SIX_SPUN_ONCE_TIME / 2:
        return player.goNow('playOffBall')

@superState('playOffBall')
@stay
@ifSwitchLater(transitions.shouldNotBeSupporter, 'playOffBall')
def positionAsSupporter(player):
    """
    Position to support teammate with claim.
    """
    positionAsSupporter.position = getSupporterPosition(player, player.role)
    fastWalk = False
    playerFourSearchBehavior.pointIndex = -1
    playerFiveSearchBehavior.pointIndex = -1

    if player.firstFrame():
        player.brain.tracker.trackBall()

        player.brain.nav.goTo(positionAsSupporter.position, precision = nav.GENERAL_AREA,
                              speed = speeds.SPEED_EIGHT, avoidObstacles = True,
                              fast = fastWalk, pb = False)

    if positionAsSupporter.position.distTo(player.brain.loc) > 20:
        player.brain.nav.goTo(positionAsSupporter.position, precision = nav.GENERAL_AREA,
                              speed = speeds.SPEED_EIGHT, avoidObstacles = True,
                              fast = fastWalk, pb = False)

    player.brain.nav.updateDest(positionAsSupporter.position, fast = fastWalk)

@superState('playOffBall')
@stay
@ifSwitchNow(transitions.noBallFoundAtSharedBall, 'playerFourSearchBehavior') #was: positionAtHome USOPEN2016
@ifSwitchNow(transitions.shouldFindFlippedSharedBall, 'searchFieldForFlippedSharedBall')
@ifSwitchNow(transitions.shouldStopLookingForSharedBall, 'playerFourSearchBehavior') #was: positionAtHome USOPEN2016
def searchFieldForSharedBall(player):
    """
    Searches the field for the shared ball.
    """
    sharedball = Location(player.brain.sharedBall.x, player.brain.sharedBall.y)

    if player.firstFrame():
        player.brain.tracker.trackBall()
        player.brain.tracker.repeatWideSnapPan()
        player.sharedBallCloseCount = 0
        player.sharedBallOffCount = 0
        player.brain.nav.goTo(sharedball, precision = nav.GENERAL_AREA,
                              speed = speeds.SPEED_EIGHT, avoidObstacles = True,
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
@ifSwitchNow(transitions.shouldStopLookingForFlippedSharedBall, 'playerFourSearchBehavior') #was: positionAtHome USOPEN2016
def searchFieldForFlippedSharedBall(player):
    """
    Flips the shared ball and searches for it.
    """
    sharedball = Location(-1*(player.brain.sharedBall.x-NogginConstants.MIDFIELD_X) + NogginConstants.MIDFIELD_X,
                          -1*(player.brain.sharedBall.y-NogginConstants.MIDFIELD_Y) + NogginConstants.MIDFIELD_Y)

    if player.firstFrame():
        player.brain.tracker.trackBall()
        player.brain.tracker.repeatWideSnapPan()
        player.sharedBallCloseCount = 0
        player.brain.nav.goTo(sharedball, precision = nav.GENERAL_AREA,
                              speed = speeds.SPEED_EIGHT, avoidObstacles = True,
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


# @superState('playOffBall')
# @stay
# @ifSwitchNow(shared.navAtPosition, 'watchForBall')
# def positionAtDefenderHome(player):
#     """
#     Go to the player's home position.
#     """

#     home = calculateHomePosition(player)

#     if player.firstFrame():
#         player.brain.tracker.trackBall()
#         fastWalk = role.isChaser(player.role)
#         player.brain.nav.goTo(home, precision = nav.GRAINY,
#                               speed = speeds.SPEED_EIGHT, avoidObstacles = True,
#                               fast = fastWalk, pb = False)

#     if home.distTo(player.brain.loc) > 90:
#         player.brain.nav.updateDest(home)

#     print home.x, home.y, home.h

@superState('playOffBall')
@stay
# @ifSwitchNow(transitions.ballInOurHalf, 'playOffBall')
def leftDefenderForward(player):

    if player.firstFrame():
        player.brain.tracker.trackBall()
        player.brain.home = role.evenDefenderForward

        if player.brain.home.distTo(player.brain.loc) < 45:
            adjustHeading.desiredHeading = -25
            return player.goNow('adjustHeading')

        player.brain.nav.goTo(player.brain.home, precision = nav.GRAINY,
                          speed = speeds.SPEED_EIGHT, avoidObstacles = True,
                          fast = True, pb = False)

    if player.brain.home.distTo(player.brain.loc) < 45:
        adjustHeading.desiredHeading = role.evenDefenderForward.h
        return player.goNow('adjustHeading')

@superState('playOffBall')
@stay
# @ifSwitchNow(transitions.ballInOurHalf, 'playOffBall')
def leftDefenderBack(player):

    if player.firstFrame():
        player.brain.tracker.trackBall()
        player.brain.home = role.evenDefenderBack

        if player.brain.home.distTo(player.brain.loc) < 45:
            adjustHeading.desiredHeading = -25
            return player.goNow('adjustHeading')

        player.brain.nav.goTo(player.brain.home, precision = nav.GRAINY,
                          speed = speeds.SPEED_EIGHT, avoidObstacles = True,
                          fast = True, pb = False)

    if player.brain.home.distTo(player.brain.loc) < 45:
        adjustHeading.desiredHeading = role.evenDefenderBack.h
        return player.goNow('adjustHeading')

# @superState('playOffBall')
# @stay
# @ifSwitchNow(transitions.ballInTheirHalf, 'playOffBall')
# def rightDefenderBack(player):

#     if player.firstFrame():
#         player.brain.tracker.trackBall()
#         player.brain.home = role.rightDefenderBack

#         if player.brain.home.distTo(player.brain.loc) < 45:
#             return player.goNow('watchForBall')

#         player.brain.nav.goTo(player.brain.home, precision = nav.GRAINY,
#                           speed = speeds.SPEED_EIGHT, avoidObstacles = True,
#                           fast = True, pb = False)

#     if player.brain.home.distTo(player.brain.loc) < 45:
#         return player.goNow('watchForBall')

@superState('playOffBall')
@stay
def playerFourSearchBehavior(player):

    if player.firstFrame():
        player.brain.tracker.trackBall()
        if playerFourSearchBehavior.pointIndex == -1:
            playerFourSearchBehavior.dest = min(playerFourPoints, key = lambda x:fabs(player.brain.loc.distTo(x)))
            playerFourSearchBehavior.pointIndex = playerFourPoints.index(playerFourSearchBehavior.dest)
            playerFourSearchBehavior.pointsWalked = 0
        else:
            playerFourSearchBehavior.pointIndex += 1
            playerFourSearchBehavior.pointsWalked += 1
            playerFourSearchBehavior.dest = playerFourPoints[playerFourSearchBehavior.pointIndex % len(playerFourPoints)]

        player.brain.nav.goTo(playerFourSearchBehavior.dest, precision = nav.HOME,
                          speed = speeds.SPEED_SEVEN, avoidObstacles = True,
                          fast = True, pb = False)

    if shared.navAtPosition(player) and player.brain.loc.distTo(playerFourSearchBehavior.dest) < 60:
        if playerFourSearchBehavior.pointIndex % len(playerFourPoints) == 0:
            adjustHeading.desiredHeading = 180
            return player.goNow("adjustHeading")
        elif playerFourSearchBehavior.pointIndex % len(playerFourPoints) == 1:
            adjustHeading.desiredHeading = -90
            return player.goNow("adjustHeading")
        else:
            adjustHeading.desiredHeading = 90
            return player.goNow("adjustHeading")

    player.brain.nav.updateDest(playerFourSearchBehavior.dest)

playerFourWayPoint1 = Location(NogginConstants.CENTER_FIELD_X - 160, NogginConstants.CENTER_FIELD_Y - 40)
playerFourWayPoint2 = Location(NogginConstants.CENTER_FIELD_X + 170, NogginConstants.MY_GOALBOX_TOP_Y + 110)
playerFourWayPoint3 = Location(NogginConstants.CENTER_FIELD_X + 70, NogginConstants.MY_GOALBOX_BOTTOM_Y - 110)
playerFourPoints = [playerFourWayPoint1, playerFourWayPoint2, playerFourWayPoint3]
playerFourSearchBehavior.pointIndex = -1
playerFourSearchBehavior.pointsWalked = 0
playerFourSearchBehavior.dest = playerFourWayPoint1

@superState('playOffBall')
@stay
def playerFiveSearchBehavior(player):

    if player.firstFrame():
        player.brain.tracker.trackBall()
        if playerFiveSearchBehavior.pointIndex == -1:

            playerFiveSearchBehavior.dest = min(playerFivePoints, key = lambda x:fabs(player.brain.loc.distTo(x)))
            playerFiveSearchBehavior.pointIndex = playerFivePoints.index(playerFiveSearchBehavior.dest)
            playerFiveSearchBehavior.pointsWalked = 0
        else:
            playerFiveSearchBehavior.pointIndex += 1
            playerFiveSearchBehavior.pointsWalked += 1
            playerFiveSearchBehavior.dest = playerFivePoints[playerFiveSearchBehavior.pointIndex % len(playerFivePoints)]

        player.brain.nav.goTo(playerFiveSearchBehavior.dest, precision = nav.HOME,
                          speed = speeds.SPEED_SEVEN, avoidObstacles = True,
                          fast = True, pb = False)

    if shared.navAtPosition(player) and player.brain.loc.distTo(playerFiveSearchBehavior.dest) < 60:
        if playerFiveSearchBehavior.pointIndex % len(playerFivePoints) == 0:
            adjustHeading.desiredHeading = role.strikerRightHome.h
            return player.goNow("adjustHeading")
        elif playerFiveSearchBehavior.pointIndex % len(playerFivePoints) == 2:
            adjustHeading.desiredHeading = role.strikerLeftHome.h
            return player.goNow("adjustHeading")
        else:
            return player.goNow("spinAtWayPoint")

    player.brain.nav.updateDest(playerFiveSearchBehavior.dest)

#Can we specify heading as third parameter
playerFiveWayPoint1 = Location(NogginConstants.LANDMARK_YELLOW_GOAL_CROSS_X - 100, NogginConstants.MIDFIELD_Y - 200)
playerFiveWayPoint2 = Location(NogginConstants.CENTER_FIELD_X + 130, NogginConstants.CENTER_FIELD_Y)
playerFiveWayPoint3 = Location(NogginConstants.LANDMARK_YELLOW_GOAL_CROSS_X - 100, NogginConstants.MIDFIELD_Y + 200) # Mirror of 1
playerFiveWayPoint4 = Location(NogginConstants.CENTER_FIELD_X + 320, NogginConstants.CENTER_FIELD_Y)
playerFivePoints = [playerFiveWayPoint1, playerFiveWayPoint2, playerFiveWayPoint3, playerFiveWayPoint4]
playerFiveSearchBehavior.pointIndex = -1
playerFiveSearchBehavior.pointsWalked = 0
playerFiveSearchBehavior.dest = playerFiveWayPoint1

@superState('playOffBall')
@stay
def adjustHeading(player):

    if player.firstFrame():
        # Spin to home heading
        player.stand()
        player.brain.tracker.repeatFixedPitchLookAhead()
        dest = RelRobotLocation(0, 0, player.brain.loc.h - adjustHeading.desiredHeading)
        player.brain.nav.goTo(dest, precision = nav.HOME,
                          speed = speeds.SPEED_SIX, avoidObstacles = False,
                          fast = True, pb = False)
        # player.setWalk(0, 0, player.brain.loc.h - adjustHeading.desiredHeading)

        # or math.fabs()
    if fabs(player.brain.loc.h - adjustHeading.desiredHeading) < 25:
        player.stand()

        if role.isDefender(player.role):
            return player.goNow('watchForBall')
        else:
            return player.goNow("panAtWayPoint")

@superState('playOffBall')
@stay
def panAtWayPoint(player):

    if player.firstFrame():
        player.stand()
        player.brain.tracker.trackBall()

    # if role.isFirstChaser(player.role) and not playerFourSearchBehavior.pointIndex % len(playerFourPoints) == 0:
    #     if player.stateTime >= FULL_WIDE_PAN_TIME:
    #         return player.goNow("playerFourSearchBehavior")

    # elif player.stateTime >= FULL_WIDE_PAN_TIME * 2:
    #     return player.goNow("spinAtHome")

    if role.isFirstChaser(player.role) and not playerFourSearchBehavior.pointIndex % len(playerFourPoints) == 0:
        if player.stateTime >= tracking.INITIALIZE_HEADING_TIME + tracking.FULL_WIDE_PAN_TIME:
            return player.goNow("playerFourSearchBehavior")
    elif role.isStriker(player.role):
        if player.stateTime >= tracking.INITIALIZE_HEADING_TIME + tracking.FULL_WIDE_PAN_TIME * 2:
            return player.goNow('spinAtHome')
    else:
        if player.stateTime >= tracking.INITIALIZE_HEADING_TIME + tracking.FULL_WIDE_PAN_TIME:
            return player.goNow('spinAtHome')

    # if player.stateTime >= tracking.FULL_WIDE_PAN_TIME:
    #     if role.isFirstChaser(player.role) and not playerFourSearchBehavior.pointIndex % len(playerFourPoints) == 0:
    #         return player.goNow("playerFourSearchBehavior")
    #     else:
    #         return player.goNow("spinAtHome")

@superState('playOffBall')
@stay
def spinAtWayPoint(player):
    """
    Keep spinning in the same direction.
    """
    if player.firstFrame():
        player.stand()
        player.brain.tracker.repeatFixedPitchLookAhead()
        player.setWalk(0, 0, speeds.SPEED_SIX)

    if player.stateTime > chaseConstants.SPEED_SIX_SPUN_ONCE_TIME:
        return player.goNow('playerFiveSearchBehavior')

# @superState('playOffBall')
# @stay
# def searchFieldByQuad(player):
#     """
#     Search the field quadrant by quadrant. Choose first quadrant by loc heading.
#     """
#     if player.firstFrame():
#         player.brain.tracker.trackBall()
#         # player.brain.tracker.repeatWideSnapPan()
#         searchFieldByQuad.dest = min(points, key=lambda x:fabs(player.brain.loc.distTo(x)))
#         player.brain.nav.goTo(searchFieldByQuad.dest, precision = nav.GRAINY,
#                           speed = speeds.SPEED_EIGHT, avoidObstacles = True,
#                           fast = True, pb = False)
#         searchFieldByQuad.quadIndex = points.index(searchFieldByQuad.dest)
#         searchFieldByQuad.quadsWalked = 0

#     if shared.navAtPosition(player) and player.brain.loc.distTo(searchFieldByQuad.dest) < 60:
#         searchFieldByQuad.quadIndex += 1
#         searchFieldByQuad.quadsWalked += 1
#         searchFieldByQuad.dest = points[searchFieldByQuad.quadIndex % len(points)]

#     if searchFieldByQuad.quadsWalked > 3:
#         return player.goLater('playOffBall')

#     player.brain.nav.updateDest(searchFieldByQuad.dest)

# quad1Center = Location(NogginConstants.CENTER_FIELD_X * .6, NogginConstants.CENTER_FIELD_Y * .6)
# quad2Center = Location(NogginConstants.CENTER_FIELD_X * 1.4, NogginConstants.CENTER_FIELD_Y * 1.4)
# quad3Center = Location(NogginConstants.CENTER_FIELD_X * 1.4, NogginConstants.CENTER_FIELD_Y * .6)
# quad4Center = Location(NogginConstants.CENTER_FIELD_X * .6, NogginConstants.CENTER_FIELD_Y * 1.4)
# points = [quad1Center, quad2Center, quad3Center, quad4Center]


