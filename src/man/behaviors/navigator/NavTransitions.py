from math import fabs, hypot
from . import NavConstants as constants
import NavStates as states
from ..util import MyMath
import noggin_constants as NogginConstants
from ..players import ChaseBallTransitions
from . import NavHelper as helper

DEBUG = True

def atDestination(nav):
    """
    Returns true if we're close enough to nav's current destination;
    Takes into account loc uncertainty to check if we're at least close to
    the destination according to our belief
    """
    relDest = helper.getRelativeDestination(nav.brain.loc, states.goToPosition.dest)
    my = nav.brain.loc
    (x, y, h) = states.goToPosition.precision

    if (not helper.isDestinationRelative(states.goToPosition.dest)):
        # HACK HACK '30's below should be loc uncerts.
        return relDest.within((x + 30, y + 30, h + 30))
    else:
        return relDest.within((x, y, h))

# Should the robot dodge TO THE LEFT? (ie something is on its right)
def shouldDodgeLeft(nav):
    if not states.goToPosition.avoidObstacles:
        return False

    # check sonars
    sonarState = nav.brain.interface.sonarState
    # sonar values are given to us in meters
    sonars = (sonarState.us_right*100 < constants.AVOID_OBSTACLE_SIDE_DIST)

    #check vision
    vision = nav.brain.interface.visionObstacle.on_right

    #check feet
    footBumperState = nav.brain.interface.footBumperState
    feet = (footBumperState.r_foot_bumper_left.pressed or
            footBumperState.r_foot_bumper_right.pressed)

    #check hands
    armState = nav.brain.interface.armContactState
    arms = ((armState.left_push_direction is armState.WEST) or
            (armState.right_push_direction is armState.WEST))

    # Prioritize arms, experimentally
    if arms:
        return True

    # Take 2 of 3, indicates that we should dodge
    if (feet and vision):
        return True
    if (vision and sonars):
        return True
    elif (sonars and feet):
        return True

    else:
        return False

# Should the robot dodge TO THE RIGHT? (ie something is on its left)
def shouldDodgeRight(nav):
    if not states.goToPosition.avoidObstacles:
        return False

    # check sonars
    sonarState = nav.brain.interface.sonarState
    # sonar values are given to us in meters
    sonars = (sonarState.us_left*100 < constants.AVOID_OBSTACLE_SIDE_DIST)

    #check vision
    vision = nav.brain.interface.visionObstacle.on_left

    #check feet
    footBumperState = nav.brain.interface.footBumperState
    feet = (footBumperState.l_foot_bumper_left.pressed or
            footBumperState.l_foot_bumper_right.pressed)

    #check hands
    armState = nav.brain.interface.armContactState
    arms = ((armState.left_push_direction is armState.EAST) or
            (armState.right_push_direction is armState.EAST))

    # Prioritize arms, experimentally
    if arms:
        return True

    # If 2 of 3, indicates that we should dodge
    if (feet and vision):
        return True
    if (vision and sonars):
        return True
    elif (sonars and feet):
        return True

    else:
        return False

# ie should move backwards
def shouldDodgeBack(nav):
    if not states.goToPosition.avoidObstacles:
        return False

    #check hands
    armState = nav.brain.interface.armContactState
    arms = ((armState.left_push_direction is armState.SOUTH) or
            (armState.right_push_direction is armState.SOUTH))

    # Only use arms, experimentally
    if arms:
        return True

    else:
        return False

# ie should move forwards
def shouldDodgeForward(nav):
    if not states.goToPosition.avoidObstacles:
        return False

    #check hands
    armState = nav.brain.interface.armContactState
    arms = ((armState.left_push_direction is armState.NORTH) or
            (armState.right_push_direction is armState.NORTH))

    # Only use arms, experimentally
    if arms:
        return True

    else:
        return False

def doneDodging(nav):
    return nav.brain.interface.motionStatus.standing

def notAtLocPosition(nav):
    return not atDestination(nav)

######### BALL IN BOX ###############

#keeping this code around for posterity; do we actually need it?
#not safe to call as of 4/4/2013

def shouldChaseAroundBox(my, ball):

    # 3 common, simple cases where we don't need to worry about the box.
    if my.x > NogginConstants.MY_GOALBOX_RIGHT_X:
        if ball.x > NogginConstants.MY_GOALBOX_RIGHT_X:
            return False

    if my.y < NogginConstants.MY_GOALBOX_BOTTOM_Y:
        if ball.y < NogginConstants.MY_GOALBOX_BOTTOM_Y:
            return False

    if my.y > NogginConstants.MY_GOALBOX_TOP_Y:
        if ball.y > NogginConstants.MY_GOALBOX_TOP_Y:
            return False

    # handle more complex cases where correct behavior isn't obvious
    intersect = MyMath.linesIntersect

    return ( intersect( my.x, my.y, ball.x, ball.y, # BOTTOM_GOALBOX_LINE
                        NogginConstants.MY_GOALBOX_LEFT_X,
                        NogginConstants.MY_GOALBOX_BOTTOM_Y,
                        NogginConstants.MY_GOALBOX_RIGHT_X,
                        NogginConstants.MY_GOALBOX_BOTTOM_Y) or
             intersect( my.x, my.y, ball.x, ball.y, # LEFT_GOALBOX_LINE
                        NogginConstants.MY_GOALBOX_RIGHT_X,
                        NogginConstants.MY_GOALBOX_TOP_Y,
                        NogginConstants.MY_GOALBOX_RIGHT_X,
                        NogginConstants.MY_GOALBOX_BOTTOM_Y) or
             intersect( my.x, my.y, ball.x, ball.y, # BOTTOM_GOALBOX_LINE
                        NogginConstants.MY_GOALBOX_LEFT_X,
                        NogginConstants.MY_GOALBOX_TOP_Y,
                        NogginConstants.MY_GOALBOX_RIGHT_X,
                        NogginConstants.MY_GOALBOX_TOP_Y) )
