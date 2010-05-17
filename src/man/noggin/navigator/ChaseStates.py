from . import NavHelper as helper
from . import WalkHelper as walker
from . import NavTransitions as navTrans
from . import NavConstants as constants
from . import BrunswickSpeeds as speeds
from man.noggin.util import MyMath
from man.noggin.typeDefs.Location import RobotLocation
from man.noggin import NogginConstants
from man.noggin.playbook.PBConstants import GOALIE
from math import fabs

DEBUG = False

def walkSpinToBall(nav):

    ball = nav.brain.ball
    nav.dest = ball
    nav.dest.h = ball.heading

    # Set our walk towards the ball
    walkX, walkY, walkTheta = \
           walker.getWalkSpinParam(nav.brain.my, nav.dest)

    helper.setSpeed(nav, walkX, walkY, walkTheta)

    #if we're close to the ball...
    if navTrans.atDestinationCloser(nav.brain.my, nav.dest):
        # and facing  it, stop
        if abs(ball.bearing) < 10:
            return nav.goNow('stop')

    if not nav.brain.play.isRole(GOALIE):
        if navTrans.shouldNotGoInBox(nav):
            return nav.goLater('ballInMyBox')
        elif navTrans.shouldChaseAroundBox(nav):
            return nav.goLater('chaseAroundBox')

    return nav.stay()

def chaseAroundBox(nav):
    # does not work if we are in corner and ball is not- we'll run through box
    # would be nice to force spin towards ball at dest

    if nav.firstFrame():
        # reset dest to new RobotLocation to avoid problems w/dist calculations
        nav.dest = RobotLocation()
        nav.shouldNotChaseAroundBox = 0
        nav.brain.CoA.setRobotGait(nav.brain.motion)

    if not navTrans.shouldChaseAroundBox(nav):
        nav.shouldChaseAroundBox += 1
    else:
        nav.shouldChaseAroundBox = 0

    if nav.shouldChaseAroundBox > constants.STOP_CHASING_AROUND_BOX:
        return nav.goLater('walkSpinToBall')

    ball = nav.brain.ball
    my = nav.brain.my

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
        nav.setWalk(0, 0, constants.BALL_SPIN_SPEED *
                    MyMath.sign(ball.bearing) )

    elif fabs(ball.bearing) < constants.BALL_APPROACH_BEARING_OFF_THRESH :
        nav.stopWalking()

    if not nav.ball.inMyGoalBox():
        return nav.goLater('chase')

    return nav.stay()


# Values for controlling the strafing
PFK_MAX_Y_SPEED = speeds.MAX_Y_SPEED
PFK_MIN_Y_SPEED = speeds.MIN_Y_SPEED
PFK_MAX_X_SPEED = speeds.MAX_X_SPEED
PFK_MIN_X_SPEED = speeds.MIN_X_MAGNITUDE
PFK_MIN_Y_MAGNITUDE = speeds.MIN_Y_MAGNITUDE
PFK_X_GAIN = 0.12
PFK_Y_GAIN = 0.6


def positionForKick(nav):
    ## nav.dest = kick.getKickPosition()

    ## sX,sY,sTheta = walker.getOmniWalkParam(nav.brain.my, nav.dest)

    ball = nav.brain.ball

    # Determine approach speed
    relY = ball.relX
    relX = ball.relY

    sY = MyMath.clip(relY * PFK_Y_GAIN,
                     PFK_MIN_Y_SPEED,
                     PFK_MAX_Y_SPEED)

    sY = max(PFK_MIN_Y_MAGNITUDE,sY) * MyMath.sign(sY)

    if ball.dist > 5:
        sX = MyMath.clip(relX * PFK_X_GAIN,
                         PFK_MIN_X_SPEED,
                         PFK_MAX_X_SPEED)
    else:
        sX = 0.0

    helper.setSpeed(nav,sX,sY,0)
    return nav.stay()
