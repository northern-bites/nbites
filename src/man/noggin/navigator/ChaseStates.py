from . import NavHelper as helper
from . import WalkHelper as walker
from . import NavTransitions as navTrans
from . import NavConstants as constants
from man.noggin.util import MyMath
from objects import (RobotLocation, RelLocation)

import noggin_constants as NogginConstants
from man.noggin.playbook.PBConstants import GOALIE
from math import fabs

DEBUG = False


def crossoverTowardsBall(nav):
    CROSSOVER_DIST = 40         # Cross over for 40cm

    brain = nav.brain
    my = brain.my
    ball = nav.brain.ball

    if ball.dist < 100:
        return nav.stay()

    # Get location 40cm away in line towards the ball
    destH = my.headingTo(ball)
    distToBall = ball.dist

    relDestX = ball.relX * CROSSOVER_DIST / distToBall
    relDestY = ball.relY * CROSSOVER_DIST / distToBall

    nonRelDest = RobotLocation(relDestX + my.x,
                               relDestY + my.y,
                               destH)
    nav.dest = RelLocation(my, relDestX, relDestY, ball.bearing)

    (walkX, walkY, walkTheta) = walker.getOmniWalkParam(my, nav.dest)

    helper.setSpeed(nav, walkX, walkY, walkTheta)

    if (navTrans.atDestinationCloserAndFacing(nav.brain.my,
                                              nonRelDest,
                                              ball.bearing) or
        (abs(ball.bearing) < 20)
        and ball.vis.on):
        return nav.goLater('walkSpinToBall')

    if not nav.brain.play.isRole(GOALIE):
        if navTrans.shouldNotGoInBox(ball):
            return nav.goLater('ballInMyBox')
        elif navTrans.shouldChaseAroundBox(nav.brain.my, ball):
            return nav.goLater('chaseAroundBox')
        elif navTrans.shouldAvoidObstacleDuringApproachBall(nav):
            return nav.goLater('avoidObstacle')

    return nav.stay()

LEFT_FOOT_OFFSET = 8

def walkSpinToBall(nav):
    ball = nav.brain.ball

    nav.dest = ball
    nav.dest.relX += LEFT_FOOT_OFFSET
    nav.dest.h = ball.heading
    nav.dest.relH = ball.bearing

    # Set our walk towards the ball
    if (nav.dest.dist > constants.OMNI_WALK_DIST_THRESH):
        walkX, walkY, walkTheta = walker.getWalkStraightParam(nav.brain.my, nav.dest)
    else:
        walkX, walkY, walkTheta = walker.getOmniWalkParam(nav.brain.my, nav.dest)

    helper.setSpeed(nav, walkX, walkY, walkTheta)

    if not nav.brain.play.isRole(GOALIE):
        if navTrans.shouldNotGoInBox(ball):
            return nav.goLater('ballInMyBox')
        elif navTrans.shouldChaseAroundBox(nav.brain.my, ball):
            return nav.goLater('chaseAroundBox')
        elif navTrans.shouldAvoidObstacleDuringApproachBall(nav):
            return nav.goLater('avoidObstacle')

    return nav.stay()

# TODO: clean up!!
def chaseAroundBox(nav):
    # does not work if we are in corner and ball is not- we'll run through box
    # would be nice to force spin towards ball at dest

    if nav.firstFrame():
        # reset dest to new RobotLocation to avoid problems w/dist calculations
        nav.dest = RobotLocation(0, 0, 0)
        nav.shouldChaseAroundBox = 0

    ball = nav.brain.ball
    my = nav.brain.my

    if not navTrans.shouldChaseAroundBox(my, ball):
        nav.shouldChaseAroundBox += 1
    else:
        nav.shouldChaseAroundBox = 0

    if nav.shouldChaseAroundBox > constants.STOP_CHASING_AROUND_BOX:
        return nav.goNow('walkSpinToBall')

    elif navTrans.shouldAvoidObstacleDuringApproachBall(nav):
        return nav.goNow('avoidObstacle')

    if my.x > NogginConstants.MY_GOALBOX_RIGHT_X:
        # go to corner nearest ball
        if ball.y > NogginConstants.MY_GOALBOX_TOP_Y:
            nav.dest.x = (NogginConstants.MY_GOALBOX_RIGHT_X +
                          constants.GOALBOX_OFFSET)
            nav.dest.y = (NogginConstants.MY_GOALBOX_TOP_Y +
                          constants.GOALBOX_OFFSET)
            nav.dest.h = my.headingTo(nav.dest)

        elif ball.y < NogginConstants.MY_GOALBOX_BOTTOM_Y:
            nav.dest.x = (NogginConstants.MY_GOALBOX_RIGHT_X +
                          constants.GOALBOX_OFFSET)
            nav.dest.y = (NogginConstants.MY_GOALBOX_BOTTOM_Y -
                          constants.GOALBOX_OFFSET)
            nav.dest.h = my.headingTo(nav.dest)

    if my.x < NogginConstants.MY_GOALBOX_RIGHT_X:
        # go to corner nearest ball
        if my.y > NogginConstants.MY_GOALBOX_TOP_Y:
            nav.dest.x = (NogginConstants.MY_GOALBOX_RIGHT_X +
                          constants.GOALBOX_OFFSET)
            nav.dest.y = (NogginConstants.MY_GOALBOX_TOP_Y +
                          constants.GOALBOX_OFFSET)
            nav.dest.h = my.headingTo(nav.dest)

        if my.y < NogginConstants.MY_GOALBOX_BOTTOM_Y:
            nav.dest.x = (NogginConstants.MY_GOALBOX_RIGHT_X +
                          constants.GOALBOX_OFFSET)
            nav.dest.y = (NogginConstants.MY_GOALBOX_BOTTOM_Y -
                          constants.GOALBOX_OFFSET)
            nav.dest.h = my.headingTo(nav.dest)

    walkX, walkY, walkTheta = walker.getWalkSpinParam(my, nav.dest)
    helper.setSpeed(nav, walkX, walkY, walkTheta)

    return nav.stay()

def ballInMyBox(nav):
    if nav.firstFrame():
        nav.brain.tracker.activeLoc()

    ball = nav.brain.ball

    if fabs(ball.bearing) > constants.BALL_APPROACH_BEARING_THRESH:
        nav.setWalk(0, 0,
                    constants.BALL_SPIN_SPEED * MyMath.sign(ball.bearing) )

    elif fabs(ball.bearing) < constants.BALL_APPROACH_BEARING_OFF_THRESH :
        nav.stopWalking()

    if not nav.ball.inMyGoalBox():
        return nav.goLater('chase')

    return nav.stay()

def dribble(nav):
    ball = nav.brain.ball
    nav.dest = ball
    nav.dest.h = ball.heading

    # Set our walk towards the ball
    walkX, walkY, walkTheta = \
           walker.getWalkSpinParam(nav.brain.my, nav.dest)

    helper.setDribbleSpeed(nav, walkX, walkY, walkTheta)

    if not nav.brain.play.isRole(GOALIE):
        if navTrans.shouldNotGoInBox(ball):
            return nav.goLater('ballInMyBox')
        elif navTrans.shouldChaseAroundBox(nav.brain.my, ball):
            return nav.goLater('chaseAroundBox')

    return nav.stay()
