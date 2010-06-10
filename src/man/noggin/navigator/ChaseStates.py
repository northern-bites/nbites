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
        if fabs(ball.bearing) < 10.:
            return nav.goNow('stop')

    if not nav.brain.play.isRole(GOALIE):
        if navTrans.shouldNotGoInBox(ball):
            return nav.goLater('ballInMyBox')
        elif navTrans.shouldChaseAroundBox(nav.brain.my, ball):
            return nav.goLater('chaseAroundBox')
        elif navTrans.shouldAvoidObstacleDuringApproachBall(nav):
            return nav.goLater('avoidObstacle')

    return nav.stay()

def chaseAroundBox(nav):
    # does not work if we are in corner and ball is not- we'll run through box
    # would be nice to force spin towards ball at dest

    if nav.firstFrame():
        # reset dest to new RobotLocation to avoid problems w/dist calculations
        nav.dest = RobotLocation()
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
PFK_MIN_X_SPEED = speeds.MIN_X_SPEED
PFK_MIN_Y_MAGNITUDE = speeds.MIN_Y_MAGNITUDE
PFK_MIN_X_MAGNITUDE = speeds.MIN_X_MAGNITUDE
PFK_X_GAIN = 0.12
PFK_Y_GAIN = 0.6


def positionForKick(nav):
    """
    move to position relative to ball given by offset and heading calculated
    by kick decider without running into the ball
    """

    (x_offset, y_offset, heading) = nav.brain.kickDecider.currentKick.getPosition()

    if not navTrans.atHeading(nav.brain.my, heading):
        nav.angleToOrbit = MyMath.sub180Angle(nav.brain.my.h - heading)
        print nav.angleToOrbit
        return nav.goNow('orbitBallThruAngle')

    # if we need to orbit, switch to orbit state
    ball = nav.brain.ball

    # Determine approach speed
    target_x = ball.relX - x_offset

    if -1. <= target_x < 2.:
        sX = 0
    else:
        sX = MyMath.clip(target_x * PFK_X_GAIN,
                         PFK_MIN_X_SPEED,
                         PFK_MAX_X_SPEED)
        sX = max(PFK_MIN_X_MAGNITUDE,sX) * MyMath.sign(sX)

    target_y = ball.relY - y_offset

    if fabs(target_y) < 1.0:
        sY = 0
    else:
        sY = MyMath.clip(target_y * PFK_Y_GAIN,
                         PFK_MIN_Y_SPEED,
                         PFK_MAX_Y_SPEED)

        sY = max(PFK_MIN_Y_MAGNITUDE,sY) * MyMath.sign(sY)

    # calculate spin speed
    hDiff = MyMath.sub180Angle(nav.brain.my.h - heading)
    sTheta = MyMath.sign(hDiff) * walker.getRotScale(hDiff) * .1 *\
             constants.OMNI_MAX_SPIN_SPEED
    sTheta = MyMath.clip(sTheta,
                         constants.OMNI_MIN_SPIN_SPEED,
                         constants.OMNI_MAX_SPIN_SPEED)

    if fabs(sTheta) < constants.OMNI_MIN_SPIN_MAGNITUDE:
        sTheta = 0.0

    if sX == 0.0 and sY == 0.0 and sTheta == 0.0:
        return nav.goNow('stop')

    helper.setSpeed(nav,sX,sY,sTheta)

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

MIN_ORBIT_REL_X = 14.
MAX_ORBIT_REL_X = 16.
MAX_DIFF_REL_Y = 2.

def orbitAdjust(nav):
    # needs to adjust far, far less
    # if we need to orbit, switch to orbit state
    ball = nav.brain.ball

    if fabs(ball.relY) < (MAX_DIFF_REL_Y - .5) and \
           ((MIN_ORBIT_REL_X + .5) < ball.relX < (MAX_ORBIT_REL_X - .5)):
        return nav.goNow('orbitBallThruAngle')

    else:
        if fabs(ball.relY) >= (MAX_DIFF_REL_Y - .5):
            sY = MyMath.clip(ball.relY * PFK_Y_GAIN,
                             PFK_MIN_Y_SPEED,
                             PFK_MAX_Y_SPEED)

            sY = max(PFK_MIN_Y_MAGNITUDE,sY) * MyMath.sign(sY)

        else:
            sY = 0.0

        if ((MIN_ORBIT_REL_X +.5) > ball.relX or
            ball.relX >( MAX_ORBIT_REL_X - .5)):
            sX = MyMath.clip((ball.relX - MIN_ORBIT_REL_X )* PFK_X_GAIN,
                             PFK_MIN_X_SPEED,
                             PFK_MAX_X_SPEED)
            sX = max(PFK_MIN_X_MAGNITUDE,sX) * MyMath.sign(sX)

        else:
            sX = 0.0

    helper.setSpeed(nav,sX,sY,0)
    return nav.stay()

def orbitBallThruAngle(nav):
    """
    Circles around a point in front of robot, for a certain angle
    """
    ball = nav.brain.ball
    # ball is too off-center to orbit, center on it
    if nav.firstFrame() and nav.lastDiffState == 'positionForKick':
        nav.orbitFrames = 0
        if fabs(nav.angleToOrbit) < constants.MIN_ORBIT_ANGLE:
            return nav.goNow('positionForKick')

    if fabs(ball.relY) > MAX_DIFF_REL_Y or \
           ball.relX <= MIN_ORBIT_REL_X or \
           ball.relX >= MAX_ORBIT_REL_X:
        print ball.relX
        return nav.goNow('orbitAdjust')

    if nav.firstFrame():
        if nav.angleToOrbit < 0:
            orbitDir = constants.ORBIT_LEFT
        else:
            orbitDir = constants.ORBIT_RIGHT

        if fabs(nav.angleToOrbit) <= constants.ORBIT_SMALL_ANGLE:
            sY = constants.ORBIT_STRAFE_SPEED * constants.ORBIT_SMALL_GAIN
            sT = constants.ORBIT_SPIN_SPEED * constants.ORBIT_SMALL_GAIN

        elif fabs(nav.angleToOrbit) <= constants.ORBIT_LARGE_ANGLE:
            sY = constants.ORBIT_STRAFE_SPEED * \
                constants.ORBIT_MID_GAIN
            sT = constants.ORBIT_SPIN_SPEED * \
                constants.ORBIT_MID_GAIN
        else :
            sY = constants.ORBIT_STRAFE_SPEED * \
                constants.ORBIT_LARGE_GAIN
            sT = constants.ORBIT_SPIN_SPEED * \
                constants.ORBIT_LARGE_GAIN

        walkX = -1.
        walkY = orbitDir*sY
        walkTheta = orbitDir*sT
        helper.setSpeed(nav, walkX, walkY, walkTheta )

    #  (frames/second) / (degrees/second) * degrees + a little b/c we run slow
    framesToOrbit = fabs((constants.FRAME_RATE / nav.walkTheta) *
                         nav.angleToOrbit)
    nav.orbitFrames += 1

    if nav.orbitFrames >= framesToOrbit:
        return nav.goLater('positionForKick')
    return nav.stay()
