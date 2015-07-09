from math import fabs, hypot
from . import NavConstants as constants
import NavStates as states
from ..util import MyMath
import noggin_constants as NogginConstants
from ..players import ChaseBallTransitions
from . import NavHelper as helper
from objects import RelRobotLocation
from ..util import Transition

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


def shouldDodge(nav):
    # If nav isn't avoiding things, just no
    if not states.goToPosition.avoidObstacles:
        return False

    # these will let us know if we didn't have an obstacle detector of this type
    shouldDodge.prevSOrACount = shouldDodge.sOrACount
    shouldDodge.prevVCount = shouldDodge.vCount

    shouldDodge.willDodge = False

    for i, detector in enumerate(nav.brain.obstacleDetectors):
        if detector == 's' or detector == 'a':
            shouldDodge.sOrACount += 1
            if shouldDodge.sOrACount >= 7:
                shouldDodge.willDodge = True
        elif detector == 'v':
            shouldDodge.vCount += 1
            if shouldDodge.vCount >= 3:
                shouldDodge.willDodge = True

        if shouldDodge.willDodge:
            shouldDodge.sOrACount = 0
            shouldDodge.vCount = 0
            states.dodge.obstaclePosition = i
            doneDodging.timer = 0
            doneDodging.obstaclePosition = i
            doneDodging.detectorDodged = detector
            return True
            
    # reset counters if we did not get that observation
    if shouldDodge.prevSOrACount == shouldDodge.sOrACount:
        shouldDodge.sOrACount = 0
    if shouldDodge.prevVCount == shouldDodge.vCount:
        shouldDodge.vCount = 0

    return False

# Check if an obstacle is no longer there, or if we've completed the dodge
def doneDodging(nav):
    timerDone = False
    obstacles = True

    doneDodging.timer += 1
    if doneDodging.timer > 70:
        timerDone = True

    if nav.brain.obstacleDetectors[doneDodging.obstaclePosition] != doneDodging.detectorDodged:
        obstacles = False

    return timerDone and not obstacles

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
