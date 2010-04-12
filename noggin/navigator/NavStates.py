""" States for finding our way on the field """

from ..util import MyMath
from . import NavConstants as constants
from . import NavHelper as helper
from ..players import ChaseBallConstants
from ..playbook.PBConstants import GOALIE
from math import fabs

DEBUG = False

def doingSweetMove(nav):
    '''executes the currently set sweetmove'''
    motion = nav.brain.motion
    if nav.firstFrame():
        nav.walkX, nav.walkY, nav.walkTheta = 0, 0, 0
        helper.setSpeed(motion, 0, 0, 0)
        helper.executeMove(motion, nav.sweetMove)

    if not nav.brain.motion.isBodyActive():
        del nav.sweetMove
        return nav.goNow('stopped')

    return nav.stay()


# States for the standard spin - walk - spin go to
def spinToWalkHeading(nav):
    """
    Spin to the heading needed to walk to a specific point
    """
    my = nav.brain.my
    targetH = my.getTargetHeading(nav.dest)
    newSpinDir = my.spinDirToHeading(targetH)

    if nav.firstFrame():
        nav.changeSpinDirCounter = 0
        nav.stopSpinToWalkCount = 0
        nav.curSpinDir = newSpinDir

    if newSpinDir != nav.curSpinDir:
        nav.changeSpinDirCounter += 1
    else:
        nav.changeSpinDirCounter = 0

    if nav.changeSpinDirCounter >  constants.CHANGE_SPIN_DIR_THRESH:
        nav.curSpinDir = newSpinDir
        nav.changeSpinDirCounter = 0
        if DEBUG: nav.printf("Switching spin directions my.h " +
                             str(nav.brain.my.h)+
                             " and my target h: " + str(targetH))

    if helper.atHeadingGoTo(my, targetH):
        nav.stopSpinToWalkCount += 1
    else :
        nav.stopSpinToWalkCount -= 1
        nav.stopSpinToWalkCount = max(0, nav.stopSpinToWalkCount)

    if nav.stopSpinToWalkCount > constants.GOTO_SURE_THRESH:
        return nav.goLater('walkStraightToPoint')
    if helper.atDestinationCloser(my, nav.dest):
        return nav.goLater('spinToFinalHeading')

    headingDiff = fabs(nav.brain.my.h - targetH)
    sTheta = nav.curSpinDir * constants.GOTO_SPIN_SPEED * \
        helper.getRotScale(headingDiff)

    if sTheta != nav.walkTheta:
        nav.walkX, nav.walkY = 0, 0
        nav.walkTheta = sTheta
        helper.setSpeed(nav.brain.motion, nav.walkX, nav.walkY, nav.walkTheta)

    return nav.stay()

def walkStraightToPoint(nav):
    """
    State to walk forward w/some turning
    until localization thinks we are close to the point
    Stops if we get there
    If we no longer are heading towards it change to the spin state.
    """
    if nav.firstFrame():
        nav.walkToPointCount = 0
        nav.walkToPointSpinCount = 0

    my = nav.brain.my
    if helper.atDestinationCloser(my, nav.dest):
        nav.walkToPointCount += 1
    else :
        nav.walkToPointCount = 0

    if nav.walkToPointCount > constants.GOTO_SURE_THRESH:
        return nav.goLater('spinToFinalHeading')


    targetH = my.getTargetHeading(nav.dest)

    if helper.notAtHeading(my, targetH):
        nav.walkToPointSpinCount += 1
    else :
        nav.walkToPointSpinCount = 0

    if nav.walkToPointSpinCount > constants.GOTO_SURE_THRESH:
        return nav.goLater('spinToWalkHeading')

    bearing = MyMath.sub180Angle(nav.brain.my.h - targetH)
    distToDest = my.dist(nav.dest)
    if distToDest < ChaseBallConstants.APPROACH_WITH_GAIN_DIST:
        gain = constants.GOTO_FORWARD_GAIN * distToDest
    else :
        gain = 1.0

    sTheta = MyMath.clip(MyMath.sign(bearing) *
                         constants.GOTO_STRAIGHT_SPIN_SPEED *
                         helper.getRotScale(bearing),
                         -constants.GOTO_STRAIGHT_SPIN_SPEED,
                         constants.GOTO_STRAIGHT_SPIN_SPEED )

    if fabs(sTheta) < constants.MIN_SPIN_MAGNITUDE_WALK:
        sTheta = 0

    sX = MyMath.clip(constants.GOTO_FORWARD_SPEED*gain,
              constants.WALK_TO_MIN_X_SPEED,
              constants.WALK_TO_MAX_X_SPEED)

    nav.walkY = 0
    nav.walkX = sX
    nav.walkTheta = sTheta

    helper.setSpeed(nav.brain.motion, nav.walkX, nav.walkY, nav.walkTheta)
    return nav.stay()

def spinToFinalHeading(nav):
    """
    Spins until we are facing the final desired heading
    Stops when at heading
    """
    if nav.firstFrame():
        nav.stopSpinToWalkCount = 0

    targetH = nav.dest.h

    #may be able to keep sign of this and eliminate spinDir
    headingDiff = fabs(MyMath.sub180Angle(nav.brain.my.h - targetH))
    if DEBUG:
        nav.printf("Need to spin to %g, heading diff is %g,heading uncert is %g"
                   % (targetH, headingDiff, nav.brain.my.uncertH))
    spinDir = nav.brain.my.spinDirToHeading(targetH)

    spin = spinDir*constants.GOTO_SPIN_SPEED*helper.getRotScale(headingDiff)

    if helper.atHeading(nav.brain.my, targetH):
        nav.stopSpinToWalkCount += 1
    else:
        nav.stopSpinToWalkCount = 0

    if nav.stopSpinToWalkCount > constants.CHANGE_SPIN_DIR_THRESH:
        return nav.goLater('stop')

    nav.walkX, nav.walkY = 0, 0
    nav.walkTheta = spin
    helper.setSpeed(nav.brain.motion, nav.walkX, nav.walkY, nav.walkTheta)
    return nav.stay()

def omniWalkToPoint(nav):
    my = nav.brain.my
    dest = nav.dest
    if nav.firstFrame():
        nav.walkToPointCount = 0
    if nav.brain.play.isRole(GOALIE):
        if helper.atDestinationGoalie(my, dest) and helper.atHeading(my, dest.h):
            return nav.goNow('stop')
    else:
        if helper.atDestinationCloser(my, dest) and helper.atHeading(my, dest.h):
            return nav.goNow('stop')

    nav.walkX, nav.walkY, nav.walkTheta = helper.getOmniWalkParam(my, dest)
    helper.setSpeed(nav.brain.motion, nav.walkX, nav.walkY, nav.walkTheta)

    return nav.stay()

# State to be used with standard setSpeed movement
def walking(nav):
    """
    State to be used when setSpeed is called
    """
    if nav.updatedTrajectory:
        helper.setSpeed(nav.brain.motion, nav.walkX, nav.walkY, nav.walkTheta)
        nav.updatedTrajectory = False

    return nav.stay()
# State to use with the setSteps method
def stepping(nav):
    """
    We use this to go a specified number of steps.
    This is different from walking.
    """
    if nav.firstFrame():
        helper.step(nav.brain.motion, nav.stepX, nav.stepY, nav.stepTheta, nav.numSteps)
    elif not nav.brain.motion.isWalkActive():
        return nav.goNow("stopped")
    return nav.stay()

### Stopping States ###
def stop(nav):
    """
    Wait until the walk is finished.
    """
    if nav.firstFrame():
        helper.setSpeed(nav.brain.motion, 0, 0, 0)
        nav.walkX = nav.walkY = nav.walkTheta = nav.stepX = nav.stepY \
                    = nav.stepTheta = nav.numSteps = 0

    if not nav.brain.motion.isWalkActive():
        return nav.goNow('stopped')

    return nav.stay()

def stopped(nav):

    return nav.stay()

def orbitPoint(nav):
    if nav.updatedTrajectory:
        helper.setSpeed(nav.brain.motion, nav.walkX, nav.walkY, nav.walkTheta)
        nav.updatedTrajectory = False

    return nav.stay()


def orbitPointThruAngle(nav):
    """
    Circles around a point in front of robot, for a certain angle
    """
    if fabs(nav.angleToOrbit) < constants.MIN_ORBIT_ANGLE:
        return nav.goNow('stop')
    if nav.updatedTrajectory:
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

        nav.walkX = -0.5
        nav.walkY = orbitDir*sY
        nav.walkTheta = orbitDir*sT
        helper.setSpeed(nav.brain.motion, nav.walkX, nav.walkY, nav.walkTheta )

    #  (frames/second) / (degrees/second) * degrees
    framesToOrbit = fabs((constants.FRAME_RATE / nav.walkTheta) *
                         nav.angleToOrbit)
    if nav.counter >= framesToOrbit:
        return nav.goLater('stop')
    return nav.stay()
