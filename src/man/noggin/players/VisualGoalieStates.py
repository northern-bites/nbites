from man.motion.HeadMoves import (FIXED_PITCH_LEFT_SIDE_PAN,
                                  FIXED_PITCH_RIGHT_SIDE_PAN,
                                  FIXED_PITCH_PAN,
                                  FIXED_PITCH_SLOW_GOALIE_PAN)
from vision import certainty
from ..navigator import Navigator as nav
from ..util import Transition
import goalie
from GoalieConstants import RIGHT, LEFT, UNKNOWN
from GoalieTransitions import onLeftSideline, onRightSideline, walkedTooFar
from objects import RelRobotLocation, RelLocation
from noggin_constants import LINE_CROSS_OFFSET, GOALBOX_DEPTH, GOALBOX_WIDTH
from vision import cornerID as IDs
from math import fabs

DEBUG_OBSERVATIONS = False
DEBUG_POSITION = False

def updatePostObservations(player):
    """
    Updates the underlying C++ data structures.
    """
    if (player.brain.vision.ygrp.on and
        player.brain.vision.ygrp.certainty != certainty.NOT_SURE and
        player.brain.vision.ygrp.dist != 0.0 and
        #magic number
        player.brain.vision.ygrp.dist < 400.0):
        player.system.pushRightPostObservation(player.brain.vision.ygrp.dist,
                                               player.brain.vision.ygrp.bearing)
        if DEBUG_OBSERVATIONS:
            print "RIGHT: Saw right post."
            print "  Pushed " + str(player.brain.vision.ygrp.bearing) + " " + str(player.brain.vision.ygrp.dist)

    if (player.brain.vision.yglp.on and
        player.brain.vision.yglp.dist != 0.0 and
        #magic number
        player.brain.vision.yglp.dist < 400.0):
        player.system.pushLeftPostObservation(player.brain.vision.yglp.dist,
                                              player.brain.vision.yglp.bearing)
        if DEBUG_OBSERVATIONS:
            print "LEFT: Saw left post."
            print "  Pushed " + str(player.brain.vision.yglp.bearing) + " " + str(player.brain.vision.yglp.dist)


def updateCrossObservations(player):
    if(player.brain.vision.cross.on and
       player.brain.vision.cross.dist != 0.0):
        player.system.pushCrossObservation(player.brain.vision.cross.dist,
                                           player.brain.vision.cross.bearing)

def walkToGoal(player):
    """
    Has the goalie walk in the general direction of the goal.
    """
    if player.firstFrame():
        # first decide which side you're coming in from
        if player.lastState == 'gatherPostInfo':
            # don't change side
            player.side = player.side
        elif ((hasattr(walkToGoal, 'incomingTransition') and
             walkToGoal.incomingTransition.condition == onRightSideline) or
            player.lastState == 'gameReady'):
            player.side = RIGHT
        else:
            player.side = LEFT

        # based on that side, set up post observations
        if player.side == RIGHT:
            player.brain.tracker.repeatHeadMove(FIXED_PITCH_LEFT_SIDE_PAN)
            player.system.resetPosts(goalie.RIGHT_SIDE_RP_DISTANCE,
                                     goalie.RIGHT_SIDE_RP_ANGLE,
                                     goalie.RIGHT_SIDE_LP_DISTANCE,
                                     goalie.RIGHT_SIDE_LP_ANGLE)
        if player.side == LEFT:
            player.brain.tracker.repeatHeadMove(FIXED_PITCH_RIGHT_SIDE_PAN)
            player.system.resetPosts(goalie.LEFT_SIDE_RP_DISTANCE,
                                     goalie.LEFT_SIDE_RP_ANGLE,
                                     goalie.LEFT_SIDE_LP_DISTANCE,
                                     goalie.LEFT_SIDE_LP_ANGLE)

        player.system.home.relH = player.system.centerGoalBearing()

        player.brain.nav.goTo(player.system.home,
                              nav.CLOSE_ENOUGH, nav.FAST_SPEED)

    updatePostObservations(player)

    player.system.home.relY = player.system.centerGoalRelY()
    player.system.home.relX = player.system.centerGoalRelX()
    player.system.home.relH = player.system.centerGoalBearing()

    return Transition.getNextState(player, walkToGoal)

def dodgeBall(player):
    if player.firstFrame():
        if player.brain.ball.loc.relY < 0.0:
            dodgeDestY = player.brain.ball.loc.relY + 20.0
        else:
            dodgeDestY = player.brain.ball.loc.relY - 20.0
        player.brain.tracker.trackBallFixedPitch()
        dodgeBall.dodgeDest = RelRobotLocation(player.brain.ball.loc.relX,
                                           dodgeDestY,
                                           0.0)
        player.brain.nav.goTo(dodgeBall.dodgeDest)

    # update dest based on ball loc
    if player.brain.ball.loc.relY < 0.0:
        dodgeBall.dodgeDest.relY = player.brain.ball.loc.relY + 20.0
    else:
        dodgeBall.dodgeDest.relY = player.brain.ball.loc.relY - 20.0
    dodgeBall.dodgeDest.relX = player.brain.ball.loc.relX

    return Transition.getNextState(player, dodgeBall)

def gatherPostInfo(player):
    if player.firstFrame():
        if player.side == RIGHT:
            player.brain.tracker.repeatHeadMove(FIXED_PITCH_LEFT_SIDE_PAN)
        if player.side == LEFT:
            player.brain.tracker.repeatHeadMove(FIXED_PITCH_RIGHT_SIDE_PAN)
        player.brain.nav.stop()

    if player.counter > 90:
        return player.goLater('walkToGoal')

    return player.stay()

def spinAtGoal(player):
    if player.firstFrame():
        player.system.home.relX = 0
        player.system.home.relY = 0
        # Decide which way to rotate based on the way we came from
        if player.side == RIGHT:
            player.system.home.relH = -90
        else:
            player.system.home.relH = 90
        player.brain.nav.goTo(player.system.home,
                              nav.CLOSE_ENOUGH, nav.CAREFUL_SPEED)

    player.brain.tracker.lookToAngle(0.0)

    return Transition.getNextState(player, spinAtGoal)

# clearIt->kickBall->didIKickIt->returnToGoal
def clearIt(player):
    if player.firstFrame():
        returnToGoal.storedOdo = RelRobotLocation(player.brain.loc.lastOdoX,
                                                  player.brain.loc.lastOdoY,
                                                  0.0)
        if player.brain.ball.loc.relY < 0.0:
            player.side = RIGHT
        else:
            player.side = LEFT
        clearIt.ballDest = RelRobotLocation(player.brain.ball.loc.relX - 18.0,
                                            player.brain.ball.loc.relY,
                                            0.0)
        player.brain.nav.goTo(clearIt.ballDest,
                              nav.CLOSE_ENOUGH,
                              nav.FAST_SPEED)

    # magic number
    clearIt.ballDest.relX = player.brain.ball.loc.relX - 18.0
    clearIt.ballDest.relY = player.brain.ball.loc.relY

    return Transition.getNextState(player, clearIt)

def didIKickIt(player):
    if player.firstFrame():
        player.brain.nav.stop()
    return Transition.getNextState(player, didIKickIt)

def standStill(player):
    if player.firstFrame():
        player.brain.nav.stop()
    return player.stay()

def spinToFaceBall(player):
    if player.firstFrame():
        facingDest = RelRobotLocation(0.0, 0.0, 0.0)
        if player.brain.ball.loc.bearing < 0.0:
            facingDest.relH = -90
        else:
            facingDest.relH = 90
        player.brain.nav.goTo(facingDest,
                              nav.CLOSE_ENOUGH, nav.CAREFUL_SPEED)

    return Transition.getNextState(player, spinToFaceBall)

def decideLeftSide(player):
    if player.firstFrame():
        player.side = UNKNOWN
        player.brain.tracker.lookToAngle(90)

    return Transition.getNextState(player, decideLeftSide)

def decideRightSide(player):
    if player.firstFrame():
        player.side = UNKNOWN
        player.brain.tracker.lookToAngle(goalie.RIGHT_SIDE_ANGLE)

    return Transition.getNextState(player, decideRightSide)

def returnToGoal(player):
    if player.firstFrame():
        if player.lastDiffState == 'didIKickIt':
            correctedDest = returnToGoal.storedOdo- returnToGoal.kickPose
            correctedDest.relX = correctedDest.relX - 15.0

        else:
            correctedDest = (returnToGoal.storedOdo -
                             # magic number
                             RelRobotLocation(player.brain.loc.lastOdoX + 15,
                                              player.brain.loc.lastOdoY,
                                              0.0))

        player.brain.nav.walkTo(correctedDest)

    return Transition.getNextState(player, returnToGoal)

def findGoalboxCorner(player):
    if player.firstFrame():
        player.brain.tracker.repeatHeadMove(FIXED_PITCH_SLOW_GOALIE_PAN)

    return Transition.getNextState(player, findGoalboxCorner)

def centerAtGoalBasedOnCorners(player):
    if player.firstFrame():
        centerAtGoalBasedOnCorners.home = RelRobotLocation(-10.0, 0.0, 0.0)
        player.brain.nav.goTo(centerAtGoalBasedOnCorners.home,
                              nav.GENERAL_AREA,
                              nav.FAST_SPEED)

    for corner in player.brain.vision.fieldLines.corners:
        # if it is possible that this is the desired corner
        if(centerAtGoalBasedOnCorners.cornerID in corner.possibilities):
            if(centerAtGoalBasedOnCorners.cornerID == IDs.YELLOW_GOAL_LEFT_L
               and corner.visualOrientation < 0):
                centerAtGoalBasedOnCorners.cornerDirection = corner.bearing
                heading = corner.getRobotGlobalHeadingIfFieldAngleIs(90)
                relX = corner.getRobotRelXIfFieldAngleIs(90)
                relY = corner.getRobotRelYIfFieldAngleIs(90)
            elif(centerAtGoalBasedOnCorners.cornerID ==
                 IDs.YELLOW_GOAL_RIGHT_L and corner.visualOrientation > 0):
                centerAtGoalBasedOnCorners.cornerDirection = corner.bearing
                heading = corner.getRobotGlobalHeadingIfFieldAngleIs(0)
                relX = corner.getRobotRelXIfFieldAngleIs(0)
                relY = corner.getRobotRelYIfFieldAngleIs(0)
            else:
                continue

            centerAtGoalBasedOnCorners.home.relH = -heading
            centerAtGoalBasedOnCorners.home.relX = -(GOALBOX_DEPTH + relX)
            if centerAtGoalBasedOnCorners.cornerID == IDs.YELLOW_GOAL_LEFT_L:
                centerAtGoalBasedOnCorners.home.relY = -(GOALBOX_WIDTH/2.0 +
                                                         relY)
            else:
                centerAtGoalBasedOnCorners.home.relY = (GOALBOX_WIDTH/2.0 -
                                                        relY)

            # corrections to make nav STOP!
            if fabs(centerAtGoalBasedOnCorners.home.relH) < 5:
                centerAtGoalBasedOnCorners.home.relH = 0

            if fabs(centerAtGoalBasedOnCorners.home.relX) < 10:
                centerAtGoalBasedOnCorners.home.relX = 0

            if fabs(centerAtGoalBasedOnCorners.home.relY) < 10:
                centerAtGoalBasedOnCorners.home.relY = 0

            lookTo = RelLocation(-relX, -relY)

            player.brain.tracker.helper.lookToPointFixedPitch(lookTo)

    return Transition.getNextState(player, centerAtGoalBasedOnCorners)
