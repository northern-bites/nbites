from math import fabs, hypot
from . import NavConstants as constants
import NavStates as states
from man.noggin.util import MyMath
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
    relDest = helper.getRelativeDestination(nav.brain.my, states.goToPosition.dest)
    my = nav.brain.my
    (x, y, h) = states.goToPosition.precision

    if (not helper.isDestinationRelative(states.goToPosition.dest)):
        return relDest.within((x + my.uncertX, y + my.uncertY, h + my.uncertH))
    else:
        return relDest.within((x, y, h))


def notAtLocPosition(nav):
    return not atDestination(nav)

def walkedEnough(nav):
    deltaDest = states.walkingTo.deltaDest
    dest = states.walkingTo.dest
    precision = states.walkingTo.precision

    #check if we've "passed" the point we were supposed to go to
    #with odometry
    if (dest.relX * deltaDest.relX < 0 and
        dest.relY * deltaDest.relY < 0 and
        dest.relH * deltaDest.relH < 0):
        return True

    return deltaDest.within(precision)

######### BALL IN BOX ###############

#keeping this code around for posterity; do we actually need it?

def shouldChaseAroundBox(my, ball):

    # 3 common, simple cases where we don't need to worry about the box.
    if my.x > NogginConstants.MY_GOALBOX_RIGHT_X:
        if ball.loc.x > NogginConstants.MY_GOALBOX_RIGHT_X:
            return False

    if my.y < NogginConstants.MY_GOALBOX_BOTTOM_Y:
        if ball.loc.y < NogginConstants.MY_GOALBOX_BOTTOM_Y:
            return False

    if my.y > NogginConstants.MY_GOALBOX_TOP_Y:
        if ball.loc.y > NogginConstants.MY_GOALBOX_TOP_Y:
            return False

    # handle more complex cases where correct behavior isn't obvious
    intersect = MyMath.linesIntersect

    return ( intersect( my.x, my.y, ball.loc.x, ball.loc.y, # BOTTOM_GOALBOX_LINE
                        NogginConstants.MY_GOALBOX_LEFT_X,
                        NogginConstants.MY_GOALBOX_BOTTOM_Y,
                        NogginConstants.MY_GOALBOX_RIGHT_X,
                        NogginConstants.MY_GOALBOX_BOTTOM_Y) or
             intersect( my.x, my.y, ball.loc.x, ball.loc.y, # LEFT_GOALBOX_LINE
                        NogginConstants.MY_GOALBOX_RIGHT_X,
                        NogginConstants.MY_GOALBOX_TOP_Y,
                        NogginConstants.MY_GOALBOX_RIGHT_X,
                        NogginConstants.MY_GOALBOX_BOTTOM_Y) or
             intersect( my.x, my.y, ball.loc.x, ball.loc.y, # BOTTOM_GOALBOX_LINE
                        NogginConstants.MY_GOALBOX_LEFT_X,
                        NogginConstants.MY_GOALBOX_TOP_Y,
                        NogginConstants.MY_GOALBOX_RIGHT_X,
                        NogginConstants.MY_GOALBOX_TOP_Y) )

####### AVOIDANCE STUFF ##############

def shouldAvoidObstacleLeft(nav):
    """
    Need to avoid an obstacle on our left side
    """
    sonar = nav.brain.sonar
    obstacles = nav.brain.vision.obstacles
    leftFoot = nav.brain.sensors.leftFootBumper
    rightFoot = nav.brain.sensors.rightFootBumper
    if (leftFoot.left or leftFoot.right):
        nav.shouldAvoidObstacleLeftCounter += 3
    elif (obstacles.onLeft):
        nav.shouldAvoidObstacleLeftCounter += 1
    elif (obstacles.offField and
          sonar.leftDist != sonar.UNKNOWN_VALUE and
          sonar.leftDist < constants.AVOID_OBSTACLE_SIDE_DIST):
        nav.shouldAvoidObstacleLeftCounter += 1
    else:
        nav.shouldAvoidObstacleLeftCounter = 0
    if nav.shouldAvoidObstacleLeftCounter > \
            constants.AVOID_OBSTACLE_FRAMES_THRESH:
        return True
    return False

def shouldAvoidObstacleRight(nav):
    """
    Need to avoid an obstacle on our right side
    """
    sonar = nav.brain.sonar
    obstacles = nav.brain.vision.obstacles
    leftFoot = nav.brain.sensors.leftFootBumper
    rightFoot = nav.brain.sensors.rightFootBumper
    if (rightFoot.left or rightFoot.right):
        nav.shouldAvoidObstacleRightCounter += 3
    elif (obstacles.onRight):
        nav.shouldAvoidObstacleRightCounter += 1
    elif (obstacles.offField and
          sonar.rightDist != sonar.UNKNOWN_VALUE and
          sonar.rightDist < constants.AVOID_OBSTACLE_SIDE_DIST):
        nav.shouldAvoidObstacleRightCounter += 1
    else:
        nav.shouldAvoidObstacleRightCounter = 0
    if nav.shouldAvoidObstacleRightCounter > \
            constants.AVOID_OBSTACLE_FRAMES_THRESH:
        return True

    return False

def shouldAvoidObstacle(nav):
    """
    Should avoid an obstacle
    """
    # don't dodge an object in front when we're not going forward
    # @todo: this is terrible since we're not always in walk mode (we're
    # mostly in goTo modes)
    if states.walking.speeds[0] < 0:
        return False

    return ((shouldAvoidObstacleLeft(nav) or
             shouldAvoidObstacleRight(nav)) and
            not nav.brain.player.penaltyKicking)
