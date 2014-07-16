from math import fabs, hypot
from . import NavConstants as constants
import NavStates as states
from ..util import MyMath
import noggin_constants as NogginConstants
from ..players import ChaseBallTransitions
from . import NavHelper as helper
from objects import RelRobotLocation

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

    return relDest.within((x, y, h))

# Transition: Should I perform a dodge? Also sets up the direction.
def shouldDodge(nav):
    # If nav isn't avoiding things, just no
    if not states.goToPosition.avoidObstacles:
        return False

    if (nav.brain.obstacles[1] > constants.DODGE_DIST or
        nav.brain.obstacles[1] == 0.0):
        return False

    obsts = nav.brain.interface.fieldObstacles
    states.dodge.DDirects = ( obsts.obstacle(0).NONE,
                              obsts.obstacle(0).NORTH,
                              obsts.obstacle(0).NORTHEAST,
                              obsts.obstacle(0).EAST,
                              obsts.obstacle(0).SOUTHEAST,
                              obsts.obstacle(0).SOUTH,
                              obsts.obstacle(0).SOUTHWEST,
                              obsts.obstacle(0).WEST,
                              obsts.obstacle(0).NORTHWEST )

    info = [0.] * 9 #8 directions
    setPosition = False

    # Get relative robot / ball desitnation
    relLoc = helper.getRelativeDestination(nav.brain.loc,
                                           states.goToPosition.dest)
    walkingDest = getDirection(relLoc.relH)
    ballDir = getDirection(nav.brain.ball.bearing_deg)

    for i in range(1, len(nav.brain.obstacles)):
        if (nav.brain.obstacles[i] != 0.0 and
            nav.brain.obstacles[i] < constants.DODGE_DIST):
            info[i] = nav.brain.obstacles[i]
            if (i == int(ballDir) and nav.brain.ball.distance <
                nav.brain.obstacles[i]):
                return False
            if i <= 3 or i >= 7:
                setPosition = True

    if setPosition:
        states.dodge.targetDest = walkingDest
        states.dodge.positions = info
        doneDodging.timer = 0
        return True

    return False

# Check if an obstacle is no longer there, or if we've completed the dodge
def doneDodging(nav):
    timerDone = False
    obstacles = False

    doneDodging.timer += 1
    if doneDodging.timer > 180:
        timerDone = True

    for i in range(1, len(nav.brain.obstacles)):
        if (nav.brain.obstacles[i] != 0.0 and
            nav.brain.obstacles[i] < constants.DODGE_DIST and
            (i <= 3 or i >= 7)):
            obstacles = True

    return timerDone and not obstacles

    # print "DODGE DONE ", nav.brain.obstacles
    # print noObstacles

    # return (atDestination(nav) or !obstacles)

def getDirection(h):
    if (h < helper.constants.ZONE_WIDTH * -7. or
       h > helper.constants.ZONE_WIDTH * 7.):
        # SOUTH
        return states.dodge.DDirects[5]
    elif (h < helper.constants.ZONE_WIDTH * -5.):
        # SOUTHEAST
        return states.dodge.DDirects[4]
    elif (h < helper.constants.ZONE_WIDTH * -3.):
        # EAST
        return states.dodge.DDirects[3]
    elif (h < helper.constants.ZONE_WIDTH * -1):
        # NORTHEAST
        return states.dodge.DDirects[2]
    elif (h < helper.constants.ZONE_WIDTH):
        # NORTH
        return states.dodge.DDirects[1]
    elif (h < helper.constants.ZONE_WIDTH * 3):
        # NORTHWEST
        return states.dodge.DDirects[8]
    elif (h < helper.constants.ZONE_WIDTH * 5):
        # WEST
        return states.dodge.DDirects[7]
    else:
        # SOUTHWEST
        return states.dodge.DDirects[6]

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
