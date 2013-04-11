from ..headTracker.HeadMoves import (FIXED_PITCH_LEFT_SIDE_PAN,
                                      FIXED_PITCH_RIGHT_SIDE_PAN,
                                      FIXED_PITCH_PAN,
                                      FIXED_PITCH_SLOW_GOALIE_PAN)
#from vision import certainty
from ..navigator import Navigator as nav
from ..util import Transition
#import goalie
from GoalieConstants import RIGHT, LEFT, UNKNOWN
import GoalieTransitions
from objects import RelRobotLocation, RelLocation, Location
from noggin_constants import LINE_CROSS_OFFSET, GOALBOX_DEPTH, GOALBOX_WIDTH
#from vision import cornerID as IDs
from math import fabs, degrees, radians, sin, cos
from ..kickDecider import kicks
import noggin_constants as Constants
import goalie

DEBUG_OBSERVATIONS = False
DEBUG_POSITION = False

def updatePostObservations(player):
    """
    Updates the underlying C++ data structures.
    """
    if (player.brain.ygrp.on and
        player.brain.yglp.on):

        if(player.brain.ygrp.distance != 0.0 and
        #magic number
        player.brain.ygrp.distance < 400.0):
            player.system.pushRightPostObservation(player.brain.ygrp.distance,
                                                   player.brain.ygrp.bearing)
            if DEBUG_OBSERVATIONS:
                print "RIGHT: Saw right post."
                print "  Avg right x is now " + str(player.system.rightPostRelX())
                print "  Avg right y is now " + str(player.system.rightPostRelY())

        if (player.brain.yglp.distance != 0.0 and
            #magic number
            player.brain.yglp.distance < 400.0):
            player.system.pushLeftPostObservation(player.brain.yglp.distance,
                                                  player.brain.yglp.bearing)
            if DEBUG_OBSERVATIONS:
                print "LEFT: Saw left post."
                print "  Avg left x is now " + str(player.system.leftPostRelX())
                print "  Avg left y is now " + str(player.system.leftPostRelY())

def updateCrossObservations(player):
    cross = player.brain.interface.visionField.visual_cross
    if(cross.on and cross.distance != 0.0):
        player.system.pushCrossObservation(cross.distance,cross.bearing)

def spinToFaceGoal(player):
    if player.firstFrame():
        player.brain.tracker.lookToAngle(0)

        if (player.lastDiffState == 'decideRightSide'):
            player.side = RIGHT
        else:
            player.side = LEFT

        spinToFaceGoal.facingDest = RelRobotLocation(0.0, 0.0, 0.0)
        if player.side == RIGHT:
            spinToFaceGoal.facingDest.relH = 45
        else:
            spinToFaceGoal.facingDest.relH = -45

    if player.counter == 20:
        player.brain.nav.goTo(spinToFaceGoal.facingDest,
                              nav.CLOSE_ENOUGH, nav.CAREFUL_SPEED)

    return Transition.getNextState(player, spinToFaceGoal)


def walkToGoal(player):
    """
    Has the goalie walk in the general direction of the goal.
    """
    if player.firstFrame():
        # first decide which side you're coming in from
        if player.lastDiffState == 'gatherPostInfo':
            # don't change side
            player.side = player.side

        # based on that side, set up post observations
        if player.side == RIGHT:
            player.system.resetPosts(goalie.RIGHT_SIDE_RP_DISTANCE,
                                     goalie.RIGHT_SIDE_RP_ANGLE,
                                     goalie.RIGHT_SIDE_LP_DISTANCE,
                                     0.0)
        if player.side == LEFT:
            player.system.resetPosts(goalie.LEFT_SIDE_RP_DISTANCE,
                                     0.0,
                                     goalie.LEFT_SIDE_LP_DISTANCE,
                                     goalie.LEFT_SIDE_LP_ANGLE)


        player.system.home.relH = 0.0
        player.brain.nav.goTo(player.system.home, nav.CLOSE_ENOUGH,
                              nav.MEDIUM_SPEED, True)

    updatePostObservations(player)
    player.brain.tracker.lookToAngle(player.system.centerGoalBearing())
    player.system.home.relY = player.system.centerGoalRelY()
    player.system.home.relX = player.system.centerGoalRelX()
    player.system.home.relH = player.system.centerGoalBearing()

    return Transition.getNextState(player, walkToGoal)

def dodgeBall(player):
    if player.firstFrame():
        if player.brain.ball.rel_y < 0.0:
            dodgeDestY = player.brain.ball.rel_y + 20.0
        else:
            dodgeDestY = player.brain.ball.rel_y - 20.0
        player.brain.tracker.trackBall()
        dodgeBall.dodgeDest = RelRobotLocation(player.brain.ball.rel_x,
                                               dodgeDestY,
                                               0.0)
        player.brain.nav.goTo(dodgeBall.dodgeDest)

    # update dest based on ball loc
    if player.brain.ball.rel_y < 0.0:
        dodgeBall.dodgeDest.relY = player.brain.ball.rel_y + 20.0
    else:
        dodgeBall.dodgeDest.relY = player.brain.ball.rel_y - 20.0
    dodgeBall.dodgeDest.relX = player.brain.ball.rel_x

    return Transition.getNextState(player, dodgeBall)

def spinAtGoal(player):
    if player.firstFrame():
        spinAtGoal.home = RelRobotLocation(0, 0, 0)
        # Decide which way to rotate based on the way we came from
        if player.side == RIGHT:
            spinAtGoal.home.relH = -90
        else:
            spinAtGoal.home.relH = 90
        player.brain.nav.goTo(spinAtGoal.home,
                              nav.CLOSE_ENOUGH, nav.CAREFUL_SPEED)

        player.brain.tracker.lookToAngle(0.0)

    return Transition.getNextState(player, spinAtGoal)

# clearIt->kickBall->didIKickIt->returnToGoal
def clearIt(player):
    if player.firstFrame():
        player.brain.tracker.trackBall()
        if player.brain.ball.rel_y < 0.0:
            player.side = RIGHT
            player.kick = kicks.RIGHT_STRAIGHT_KICK
        else:
            player.side = LEFT
            player.kick = kicks.LEFT_STRAIGHT_KICK

        kickPose = player.kick.getPosition()
        clearIt.ballDest = RelRobotLocation(player.brain.ball.rel_x -
                                            kickPose[0],
                                            player.brain.ball.rel_y -
                                            kickPose[1],
                                            0.0)

        # reset odometry
        player.brain.interface.motionRequest.reset_odometry = True
        player.brain.interface.motionRequest.timestamp = int(player.brain.time * 1000)
        clearIt.odoDelay = True
        return player.stay()

    if clearIt.odoDelay:
        clearIt.odoDelay = False
        player.brain.nav.goTo(clearIt.ballDest,
                              nav.CLOSE_ENOUGH,
                              nav.FAST_SPEED)

    kickPose = player.kick.getPosition()
    clearIt.ballDest.relX = player.brain.ball.rel_x - kickPose[0]
    clearIt.ballDest.relY = player.brain.ball.rel_y - kickPose[1]

    return Transition.getNextState(player, clearIt)

def shouldISaveIt(player):
    if player.firstFrame():
        player.brain.tracker.trackBall()
        player.brain.nav.stop()
    return Transition.getNextState(player, shouldISaveIt)

def didIKickIt(player):
    if player.firstFrame():
        player.brain.nav.stop()
    return Transition.getNextState(player, didIKickIt)

def spinToFaceBall(player):
    if player.firstFrame():
        facingDest = RelRobotLocation(0.0, 0.0, 0.0)
        if player.brain.ball.bearing_deg < 0.0:
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
        player.brain.tracker.lookToAngle(-90)

    return Transition.getNextState(player, decideRightSide)

def returnToGoal(player):
    if player.firstFrame():
        if player.lastDiffState == 'didIKickIt':
            correctedDest =(RelRobotLocation(0.0, 0.0, 0.0 ) -
                            returnToGoal.kickPose)
        else:
            correctedDest = (RelRobotLocation(0.0, 0.0, 0.0) -
                             RelRobotLocation(player.brain.interface.odometry.x,
                                              player.brain.interface.odometry.y,
                                              0.0))

        if fabs(correctedDest.relX) < 5:
            correctedDest.relX = 0.0
        if fabs(correctedDest.relY) < 5:
            correctedDest.relY = 0.0
        if fabs(correctedDest.relH) < 5:
            correctedDest.relH = 0.0

        player.brain.nav.walkTo(correctedDest)

    return Transition.getNextState(player, returnToGoal)

def findGoalboxCorner(player):
    if player.firstFrame():
        player.brain.tracker.repeatHeadMove(FIXED_PITCH_SLOW_GOALIE_PAN)
        player.brain.nav.stop()

    return Transition.getNextState(player, findGoalboxCorner)

def getRobotGlobalHeading(alpha, bearing, phys):
    return degrees(phys + radians(alpha) - bearing)

def getRobotRelX(alpha, distance, phys):
    return (-distance * cos(phys + radians(alpha)))

def getRobotRelY(alpha, distance, phys):
    return (-distance * sin(phys + radians(alpha)))

def centerAtGoalBasedOnCorners(player):
    if player.firstFrame():
        centerAtGoalBasedOnCorners.home = RelRobotLocation(-10.0, 0.0, 0.0)
        player.brain.nav.goTo(centerAtGoalBasedOnCorners.home,
                              nav.GENERAL_AREA,
                              nav.FAST_SPEED)

    vision = player.brain.interface.visionField

    # this is a hack that could cause problems
    if vision.visual_corner_size() == 0:
        return Transition.getNextState(player, centerAtGoalBasedOnCorners)

    corner = vision.visual_corner(0)

    for i in range(0, vision.visual_corner_size()):
        # if it is possible that this is the desired corner
        for j in range(0, vision.visual_corner(i).poss_id_size()):
            if (vision.visual_corner(i).poss_id(j) ==
                centerAtGoalBasedOnCorners.cornerID):
                if(centerAtGoalBasedOnCorners.cornerID ==
                   vision.visual_corner(i).corner_id.YELLOW_GOAL_LEFT_L and
                   vision.visual_corner(i).orientation < 0):
                    corner = vision.visual_corner(i)
                    centerAtGoalBasedOnCorners.cornerDirection = \
                        corner.visual_detection.bearing
                    heading = \
                        getRobotGlobalHeading(90,
                                              corner.visual_detection.bearing,
                                              corner.physical_orientation)
                    relX = getRobotRelX(90,
                                        corner.visual_detection.distance,
                                        corner.physical_orientation)
                    relY = getRobotRelY(90,
                                        corner.visual_detection.distance,
                                        corner.physical_orientation)

                elif(centerAtGoalBasedOnCorners.cornerID ==
                   vision.visual_corner(i).corner_id.YELLOW_GOAL_RIGHT_L and
                   vision.visual_corner(i).orientation > 0):
                    corner = vision.visual_corner(i)
                    centerAtGoalBasedOnCorners.cornerDirection = \
                        corner.visual_detection.bearing
                    heading = \
                        getRobotGlobalHeading(0,
                                              corner.visual_detection.bearing,
                                              corner.physical_orientation)
                    relX = getRobotRelX(0,
                                        corner.visual_detection.distance,
                                        corner.physical_orientation)
                    relY = getRobotRelY(0,
                                        corner.visual_detection.distance,
                                        corner.physical_orientation)
                else:
                    continue

                centerAtGoalBasedOnCorners.home.relH = -heading
                centerAtGoalBasedOnCorners.home.relX = -(GOALBOX_DEPTH + relX)
                if (centerAtGoalBasedOnCorners.cornerID ==
                    corner.corner_id.YELLOW_GOAL_LEFT_L):
                    centerAtGoalBasedOnCorners.home.relY = -(GOALBOX_WIDTH/2.0 +
                                                             relY)
                else:
                    centerAtGoalBasedOnCorners.home.relY = (GOALBOX_WIDTH/2.0 -
                                                            relY)

                player.brain.tracker.lookToAngle(centerAtGoalBasedOnCorners.cornerDirection)

                break

    # corrections to make nav STOP!
    if fabs(centerAtGoalBasedOnCorners.home.relH) < 5:
        centerAtGoalBasedOnCorners.home.relH = 0

    if fabs(centerAtGoalBasedOnCorners.home.relX) < 10:
        centerAtGoalBasedOnCorners.home.relX = 0

    if fabs(centerAtGoalBasedOnCorners.home.relY) < 10:
        centerAtGoalBasedOnCorners.home.relY = 0

    return Transition.getNextState(player, centerAtGoalBasedOnCorners)

def repositionAfterWhiff(player):
    if player.firstFrame():
        # reset odometry
        player.brain.interface.motionRequest.reset_odometry = True
        player.brain.interface.motionRequest.timestamp = int(player.brain.time * 1000)

        if player.brain.ball.rel_y < 0.0:
            player.kick = kicks.RIGHT_STRAIGHT_KICK
        else:
            player.kick = kicks.LEFT_STRAIGHT_KICK

        kickPose = player.kick.getPosition()
        repositionAfterWhiff.ballDest = RelRobotLocation(player.brain.ball.rel_x -
                                                         kickPose[0],
                                                         player.brain.ball.rel_y -
                                                         kickPose[1],
                                                         0.0)
        player.brain.nav.goTo(repositionAfterWhiff.ballDest,
                              nav.CLOSE_ENOUGH,
                              nav.FAST_SPEED)

    # if it took more than 5 seconds, forget it
    if player.counter > 150:
        returnToGoal.kickPose.relX += player.brain.interface.odometry.x
        returnToGoal.kickPose.relY += player.brain.interface.odometry.y
        returnToGoal.kickPose.relH += player.brain.interface.odometry.h

        return player.goLater('returnToGoal')

    kickPose = player.kick.getPosition()
    repositionAfterWhiff.ballDest.relX = (player.brain.ball.rel_x -
                                          kickPose[0])
    repositionAfterWhiff.ballDest.relY = (player.brain.ball.rel_y -
                                          kickPose[1])

    return Transition.getNextState(player, repositionAfterWhiff)
