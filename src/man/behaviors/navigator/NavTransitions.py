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

    states.dodge.DDirects = (nav.brain.interface.fieldObstacles.obstacle(0).NONE,
                      nav.brain.interface.fieldObstacles.obstacle(0).NORTH,
                      nav.brain.interface.fieldObstacles.obstacle(0).NORTHEAST,
                      nav.brain.interface.fieldObstacles.obstacle(0).EAST,
                      nav.brain.interface.fieldObstacles.obstacle(0).SOUTHEAST,
                      nav.brain.interface.fieldObstacles.obstacle(0).SOUTH,
                      nav.brain.interface.fieldObstacles.obstacle(0).SOUTHWEST,
                      nav.brain.interface.fieldObstacles.obstacle(0).WEST,
                      nav.brain.interface.fieldObstacles.obstacle(0).NORTHWEST
    )

    # Get the obstacle model
    info = [0.] * 9 #8 directions
    obsts = nav.brain.interface.fieldObstacles
    size = nav.brain.interface.fieldObstacles.obstacle_size()
    setPosition = False

    # Get relative robot desitnation
    relLoc = helper.getRelativeDestination(nav.brain.loc,
                                            states.goToPosition.dest)

    walkingDest = getDirection(relLoc.relH)
    destR = int(walkingDest) + 1
    destL = int(walkingDest) - 1
    if destR > 8:
        destR = 1
    if destL < 1:
        destL = 8

    # print walkingDest

    for i in range(size):
        curr_obst = obsts.obstacle(i)
        if ((curr_obst.position is not curr_obst.position.NONE) and
            curr_obst.distance < constants.DONE_DODGE_DIST):
            info[int(curr_obst.position)] = curr_obst.distance
            setPosition = True
            # Only dodge if I am walking towards the obstacle
            # if (curr_obst.position == walkingDest or
            #     int(curr_obst.position) == destR or
            #     int(curr_obst.position) == destL):
            #     setPosition = True

    print "INFO: ", info

    if setPosition:
        print walkingDest
        states.dodge.targetDest = walkingDest
        states.dodge.positions = info
        doneDodging.targetDest = walkingDest
        doneDodging.positions = info
        return True

    return False


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

# Check if an obstacle is no longer there, or if we've completed the dodge
def doneDodging(nav):
    destR = int(doneDodging.targetDest) + 1
    destL = int(doneDodging.targetDest) - 1
    if destR > 8:
        destR = 1
    if destL < 1:
        destL = 8

    noObstacles = True
    fieldObs = nav.brain.interface.fieldObstacles
    print "DONE DODGING: ", doneDodging.positions
    for i in range(len(doneDodging.positions)):
        if (doneDodging.positions[i] < constants.DONE_DODGE_DIST and
            doneDodging.positions[i] is not fieldObs.obstacle(0).position.NONE):
            noObstacles = False
            break

    print "NO OBSTACLES: ", noObstacles
    print nav.brain.interface.motionStatus.standing
        # if (doneDodging.positions[i] == doneDodging.targetDest or
        #     int(doneDodging.positions[i]) == destR or
        #     int(doneDodging.positions[i]) == destL):
        #     if doneDodging.positions[i] is not fieldObs.obstacle(0).position.NONE:
        #         noObstacles = False
        #         break

    return (nav.brain.interface.motionStatus.standing or noObstacles)

    # return (nav.brain.interface.motionStatus.standing or
            # nav.brain.interface.obstacle.position is not doneDodging.position)

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
