from math import fabs, sqrt, hypot
from . import NavConstants as constants
from man.noggin.util import MyMath
import noggin_constants as NogginConstants
from ..players import ChaseBallTransitions

def atDestinationCloser(nav):
    """
    Returns true if we are at an (x, y) close enough to the one we want

    """
    my = nav.brain.my

    if nav.destType is constants.BALL:
        return ChaseBallTransitions.ballInPosition(nav.brain.player)

    return my.distTo(nav.getDestination()) < (constants.CLOSER_XY +
                                              hypot(my.uncertX,
                                                         my.uncertY))

def atHeadingGoTo(my, targetHeading):
    hDiff = fabs(MyMath.sub180Angle(my.h - targetHeading))
    return hDiff < constants.AT_HEADING_GOTO_DEG

def atHeading(nav):
    """
    Returns true if we are at a heading close enough to what we want
    """
    my = nav.brain.my
    dest = nav.getDestination()

    if nav.destType is constants.BALL:
        return abs(nav.brain.ball.bearing) < constants.CLOSE_ENOUGH_H

    hDiff = fabs(MyMath.sub180Angle(my.h - dest.h))
    return hDiff < constants.CLOSE_ENOUGH_H and \
           my.uncertH < constants.LOC_IS_ACTIVE_H

#not used
def notAtHeading(my, targetHeading):
    hDiff = fabs(MyMath.sub180Angle(my.h - targetHeading))

    return hDiff > constants.ALMOST_CLOSE_ENOUGH_H and \
           my.uncertH < constants.LOC_IS_ACTIVE_H

def useFinalHeading(brain, position):
    if brain.gameController.currentState == 'gameReady':
        useFinalHeadingDist = constants.FINAL_HEADING_READY_DIST
    else:
        useFinalHeadingDist = constants.FINAL_HEADING_DIST

    distToPoint = brain.my.distTo(position)

    return (distToPoint <= useFinalHeadingDist)

def shouldSwitchPFKModes(nav):
    """
    True if we're near to the ball and using setSpeed, or far away and using setDest
    """
    ball = nav.brain.ball

    # using setSpeed
    if nav.currentState == 'goToPosition' or \
           nav.currentState == 'omniGoTo' :
        if ball.dist < 30:
            return True

    return False

######### BALL IN BOX ###############

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

####### AVOIDANCE STUFF ##############

def shouldAvoidObstacleLeft(nav):
    """
    Need to avoid an obstacle on our left side
    """
    sonar = nav.brain.sonar
    if (sonar.leftDist != sonar.UNKNOWN_VALUE and
        sonar.leftDist < constants.AVOID_OBSTACLE_SIDE_DIST):
        nav.shouldAvoidObstacleLeftCounter += 1
    else :
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
    if (sonar.rightDist != sonar.UNKNOWN_VALUE and
        sonar.rightDist < constants.AVOID_OBSTACLE_SIDE_DIST):
         nav.shouldAvoidObstacleRightCounter += 1
    else :
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
    if nav.walkX <= 0:
        return False

    return ((shouldAvoidObstacleLeft(nav) or
             shouldAvoidObstacleRight(nav)) and
            not nav.brain.player.penaltyKicking)

def shouldAvoidObstacleDuringApproachBall(nav):
    return (nav.brain.ball.dist >
            constants.SHOULD_AVOID_OBSTACLE_APPROACH_DIST and \
            shouldAvoidObstacle(nav) and
            (nav.brain.ball.dist > nav.brain.sonar.leftDist or
            nav.brain.ball.dist > nav.brain.sonar.rightDist))

KICK_STRAIGHT_BEARING_THRESH = 20.
########## CHASE STUFF ##############
def shouldChaseOrbit(myH, destH):
    hDiff = MyMath.sub180Angle(myH - destH)
    return( fabs(hDiff) > KICK_STRAIGHT_BEARING_THRESH)
