#
# The transitions for the goalie for the goalie states.
# Covers chase, position and save. Most of the counters
# for transitions are contained in these transitions.
#

import GoalieConstants as constants
import noggin_constants as field
import math
import VisualGoalieStates
from objects import RelRobotLocation

# Visual Goalie

def getLeftGoalboxCorner(player):
    vision = player.brain.interface.visionField
    for i in range(0, vision.visual_corner_size()):
        for j in range(0, vision.visual_corner(i).poss_id_size()):
            if (vision.visual_corner(i).poss_id(j) ==
                vision.visual_corner(i).corner_id.YELLOW_GOAL_LEFT_L):
                if (vision.visual_corner(i).orientation < 0):
                    return vision.visual_corner(i)

    return None

def getRightGoalboxCorner(player):
    vision = player.brain.interface.visionField
    for i in range(0, vision.visual_corner_size()):
        for j in range(0, vision.visual_corner(i).poss_id_size()):
            if (vision.visual_corner(i).poss_id(j) ==
                vision.visual_corner(i).corner_id.YELLOW_GOAL_RIGHT_L):
                if (vision.visual_corner(i).orientation > 0):
                    return vision.visual_corner(i)

    return None

# Rel X of corner from robot
def getCornerRelX(alpha, corner):
    return (corner.visual_detection.distance *
            math.cos(corner.physical_orientation + math.radians(alpha)))

# Rel Y of corner from robot
def getCornerRelY(alpha, corner):
    return (corner.visual_detection.distance *
            math.sin(corner.physical_orientation + math.radians(alpha)))

def getRobotGlobalHeading(alpha, corner):
    return math.degrees(corner.physical_orientation + math.radians(alpha) -
                        corner.visual_detection.bearing)

def badLeftCornerObservation(player):
    corner = getLeftGoalboxCorner(player)
    if not corner:
        return False

    dDist = math.fabs(constants.EXPECTED_CORNER_DIST_FROM_CENTER -
                      corner.visual_detection.distance)
    dBear = math.fabs(constants.EXPECTED_LEFT_CORNER_BEARING_FROM_CENTER -
                      corner.visual_detection.bearing_deg)

    if not (dDist > constants.CORNER_DISTANCE_THRESH or
            dBear > constants.CORNER_BEARING_THRESH):
        return False

    homeRelX = -(field.GOALBOX_DEPTH - getCornerRelX(90, corner) -
                 constants.GOALIE_OFFSET)
    homeRelY = -(field.GOALBOX_WIDTH/2.0 - getCornerRelY(90, corner))
    homeRelH = -getRobotGlobalHeading(90, corner)

    player.homeDirections += [RelRobotLocation(homeRelX,
                                               homeRelY,
                                               homeRelH)]


    if len(player.homeDirections) > constants.BUFFER_THRESH:
        player.homeDirections = player.homeDirections[1:]

    return True

def badRightCornerObservation(player):
    corner = getRightGoalboxCorner(player)
    if not corner:
        return False

    dDist = math.fabs(constants.EXPECTED_CORNER_DIST_FROM_CENTER -
                      corner.visual_detection.distance)
    dBear = math.fabs(constants.EXPECTED_RIGHT_CORNER_BEARING_FROM_CENTER -
                      corner.visual_detection.bearing_deg)

    if not (dDist > constants.CORNER_DISTANCE_THRESH or
            dBear > constants.CORNER_BEARING_THRESH):
        return False

    homeRelX = -(field.GOALBOX_DEPTH - getCornerRelX(0, corner) -
                 constants.GOALIE_OFFSET)
    homeRelY = field.GOALBOX_WIDTH/2.0 + getCornerRelY(0, corner)
    homeRelH = -getRobotGlobalHeading(0, corner)

    player.homeDirections += [RelRobotLocation(homeRelX,
                                               homeRelY,
                                               homeRelH)]

    if len(player.homeDirections) > constants.BUFFER_THRESH:
        player.homeDirections = player.homeDirections[1:]

    return True

### ALSO CHECK POSTS...
# def facingSideways(player):
#     """
#     If the robot is facing a post directly, it's probably turned around.
#     """
#     if ((player.brain.yglp.on and
#          fabs(player.brain.yglp.bearing_deg) < 30.0 and
#          player.brain.yglp.bearing_deg != 0.0 and
#          player.brain.yglp.distance < 300.0) or
#         (player.brain.ygrp.on and
#          fabs(player.brain.ygrp.bearing_deg) < 30.0 and
#          player.brain.ygrp.bearing_deg != 0.0 and
#          player.brain.ygrp.distance < 300.0)):
#         return True
#     else:
#         return False


def shouldReposition(player):
    return (badLeftCornerObservation(player) or
            badRightCornerObservation(player))

# def atGoalArea(player):
#     """
#     Checks if robot is close enough to the field edge to be at the goal.
#     """
#     #magic number
#     vision = player.brain.interface.visionField
#     return ((vision.visual_field_edge.distance_m < 110.0
#              and vision.visual_field_edge.distance_m != 0.0)
#             or (player.brain.yglp.distance < 20.0
#                 and player.brain.yglp.on
#                 and not player.brain.yglp.distance == 0.0)
#             or (player.brain.ygrp.distance < 20.0
#                 and player.brain.ygrp.on
#                 and not player.brain.ygrp.distance == 0.0))

# def ballIsInMyWay(player):
#     """
#     Checks if robot will run into ball while returning from penalty.
#     """
#     if not player.brain.ball.vis.on:
#         return False

#     return (fabs(player.brain.ball.rel_y < 20.0 and
#                  player.brain.ball.rel_x < 30.0))

# def ballMoreImportant(player):
#     """
#     Goalie needs to chase, not localize itself.
#     """
#     if player.brain.ball.vis.on and player.brain.ball.vis.distance < 100.0:
#         return True

#     if (player.brain.ball.vis.on and player.brain.ball.vis.distance < 150.0 and
#         player.aggressive):
#         return True

# def facingForward(player):
#     """
#     Checks if a robot is facing the cross, which is more or less forward
#     if it is in the goal.
#     """
#     #magic numbers
#     vision = player.brain.interface.visionField
#     return (vision.visual_field_edge.distance_m > 800.0 or
#             (fabs(vision.visual_cross.bearing) < 10.0 and
#              vision.visual_cross.distance > 0.0))

def facingBall(player):
    """
    Checks if the ball is right in front of it.
    """
    #magic numbers
    return (math.fabs(player.brain.ball.vis.bearing_deg) < 10.0 and
            player.brain.ball.vis.on)

# def notTurnedAround(player):
#     """
#     Checks that we are actually facing the field when returning from
#     penalty.
#     """
#     return (player.brain.interface.visionField.visual_field_edge.distance_m
#             > 400.0)

# def onThisSideline(player):
#     """
#     Looks for a T corner or far goals to determine which sideline it's
#     standing on.
#     """
#     vision = player.brain.interface.visionField
#     return (vision.visual_field_edge.distance_m < 250.0 and
#             vision.visual_field_edge.distance_m > 100.0)

# def unsure(player):
#     return (not onThisSideline(player) and
#             player.counter > 60)

# def shouldPerformSave(player):
#     """
#     Checks that the ball is moving toward it and close enough to save.
#     """
#     return (player.brain.ball.vel_x < 0.0 and
#             player.brain.ball.speed > 15.0 and
#             player.brain.ball.rel_x_dest < 0.0 and
#             abs(player.brain.ball.rel_y_intersect_dest) < 80.0 and
#             player.brain.ball.distance < 230.0 and
#             player.brain.ball.vis.on)

## These three are penalty kick transitions. They need to be tuned.
def shouldDiveRight(player):
    return (player.brain.ball.vel_x < 0.0 and
            player.brain.ball.speed > 30.0 and
            player.brain.ball.rel_x_dest < 0.0 and
            player.brain.ball.rel_y_intersect_dest < -5.0 and
            player.brain.ball.vis.on)

def shouldDiveLeft(player):
    return (player.brain.ball.vel_x < 0.0 and
            player.brain.ball.speed > 30.0 and
            player.brain.ball.rel_x_dest < 0.0 and
            player.brain.ball.rel_y_intersect_dest > 5.0 and
            player.brain.ball.vis.on)

def shouldSquat(player):
    return (player.brain.ball.vel_x < 0.0 and
            player.brain.ball.speed > 30.0 and
            player.brain.ball.rel_x_dest < 0.0 and
            abs(player.brain.ball.rel_y_intersect_dest) < 10.0 and
            player.brain.ball.vis.on)

def shouldClearBall(player):
    """
    Checks that the ball is more or less in the goal box.
    """
    # less than 1.5 minutes left or winning/losing badly
    shouldBeAggressive = (player.brain.game.secs_remaining < 90 or
                          (abs(player.brain.game.team(0).score -
                               player.brain.game.team(1).score) > 1))

    if shouldBeAggressive and not player.aggressive:
        print "The goalie is now AGGRESSIVE"
        player.aggressive = True
    elif not shouldBeAggressive and player.aggressive:
        print "The goalie is no longer AGGRESSIVE"
        player.aggressive = False

    # ball must be visible
    if not player.brain.ball.vis.on:
        return False

    # if definitely within goal box
    if (player.brain.ball.vis.distance < 80.0):
        return True

    # farther out but being aggressive
    if (player.brain.ball.vis.distance < 120 and
        player.aggressive):
        return True

    # if to sides of box
    if (player.brain.ball.vis.distance < 120.0 and
        math.fabs(player.brain.ball.vis.bearing_deg) > 40.0):
        return True

    # to goalie's sides, being aggressive
    if (math.fabs(player.brain.ball.vis.bearing_deg) > 50.0 and
        player.aggressive):
        return True

    return False

def ballLostStopChasing(player):
    """
    If the robot does not see the ball while chasing, it is lost. Delay
    in case our shoulder pads are just hiding it.
    """
    if not player.brain.ball.vis.on and player.counter > 150:
        return True

def ballMovedStopChasing(player):
    """
    If the robot has been chasing for a while and it is far away, it should
    stop chasing.
    """
    return (player.brain.ball.vis.distance > 100.0 and
            player.counter > 200.0)

def walkedTooFar(player):
    # for the odometry reset delay
    if player.counter < 3:
        return False

    if player.aggressive:
        return False

    return (player.brain.interface.odometry.x > 90.0 or
            math.fabs(player.brain.interface.odometry.y) > 140.0)

def reachedMyDestination(player):
    """
    The robot has reached the ball after walking to it.
    """
    return player.brain.nav.isAtPosition()

def doneWalking(player):
    """
    HACK for walkTo from nav, which does not always switch to done.
    """
    return player.brain.nav.currentState == 'standing'

def successfulKick(player):
    return player.counter > 80

def whiffed(player):
    """
    If the ball is just sitting at the goalie's feet after kicking, it
    should try again.
    """
    return (player.brain.ball.rel_x < 40.0 and
            math.fabs(player.brain.ball.rel_y) < 25.0 and
            player.brain.ball.vis.on)
