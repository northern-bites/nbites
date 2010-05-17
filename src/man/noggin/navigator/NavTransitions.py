from math import fabs
from . import NavConstants as constants
from man.noggin.util import MyMath
from man.noggin import NogginConstants

def atDestination(my, dest):
    """
    Returns true if we are at an (x, y) close enough to the one we want
    """
    return my.dist(dest) < constants.CLOSE_ENOUGH_XY

def atDestinationCloser(my, dest):
    """
    Returns true if we are at an (x, y) close enough to the one we want
    """
    return my.dist(dest) < constants.CLOSER_XY

def atDestinationGoalie(my, dest):
    return my.dist(dest) < constants.GOALIE_CLOSE

def atHeadingGoTo(my, targetHeading):
    hDiff = fabs(MyMath.sub180Angle(my.h - targetHeading))
    return hDiff < constants.AT_HEADING_GOTO_DEG

def atHeading(my, targetHeading):
    """
    Returns true if we are at a heading close enough to what we want
    """
    hDiff = fabs(MyMath.sub180Angle(my.h - targetHeading))

    return hDiff < constants.CLOSE_ENOUGH_H and \
           my.uncertH < constants.LOC_IS_ACTIVE_H

def notAtHeading(my, targetHeading):
    hDiff = fabs(MyMath.sub180Angle(my.h - targetHeading))

    return hDiff > constants.ALMOST_CLOSE_ENOUGH_H and \
           my.uncertH < constants.LOC_IS_ACTIVE_H

def useFinalHeading(brain, position):
    if brain.gameController.currentState == 'gameReady':
        useFinalHeadingDist = constants.FINAL_HEADING_READY_DIST
    else:
        useFinalHeadingDist = constants.FINAL_HEADING_DIST

    distToPoint = brain.my.dist(position)

    return (distToPoint <= useFinalHeadingDist)

######### BALL IN BOX ###############

def shouldChaseAroundBox(nav):
    ball = nav.brain.ball
    my = nav.brain.my
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

def shouldNotGoInBox(nav):
    return (False and nav.ball.inMyGoalBox() and
            nav.brain.ball.dist < constants.IGNORE_BALL_IN_BOX_DIST)
