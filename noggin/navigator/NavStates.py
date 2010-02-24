""" States for finding our way on the field """

from ..util import MyMath
from . import NavMath
from . import NavConstants as constants
from ..players import ChaseBallConstants
from ..playbook.PBConstants import GOALIE
from math import fabs, cos, sin, radians

DEBUG = False

def doingSweetMove(nav):
    '''executes the currently set sweetmove'''
    if nav.firstFrame():
        nav.helper.setSpeed(0, 0, 0)
        nav.helper.executeMove(nav.sweetMove)

    if not nav.brain.motion.isBodyActive():
        del nav.sweetMove
        return nav.goNow('stopped')

    return nav.stay()


# States for the standard spin - walk - spin go to
def spinToWalkHeading(nav):
    """
    Spin to the heading needed to walk to a specific point
    """
    targetH = MyMath.getTargetHeading(nav.brain.my,
                                      nav.destX,
                                      nav.destY)
    headingDiff = fabs(nav.brain.my.h - targetH)
    newSpinDir = MyMath.getSpinDir(nav.brain.my.h, targetH)

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

    if NavMath.atHeadingGoTo(nav, targetH):
        nav.stopSpinToWalkCount += 1
    else :
        nav.stopSpinToWalkCount -= 1
        nav.stopSpinToWalkCount = max(0, nav.stopSpinToWalkCount)

    if nav.stopSpinToWalkCount > constants.GOTO_SURE_THRESH:
        return nav.goLater('walkStraightToPoint')
    if NavMath.atDestinationCloser(nav):
        return nav.goLater('spinToFinalHeading')

    sTheta = nav.curSpinDir * constants.GOTO_SPIN_SPEED * \
        NavMath.getRotScale(headingDiff)

    if sTheta != nav.walkTheta:
        nav.helper.setSpeed(0, 0, sTheta)

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

    if NavMath.atDestinationCloser(nav):
        nav.walkToPointCount += 1
    else :
        nav.walkToPointCount = 0

    if nav.walkToPointCount > constants.GOTO_SURE_THRESH:
        return nav.goLater('spinToFinalHeading')

    my = nav.brain.my
    targetH = MyMath.getTargetHeading(my, nav.destX, nav.destY)

    if NavMath.notAtHeading(nav, targetH):
        nav.walkToPointSpinCount += 1
    else :
        nav.walkToPointSpinCount = 0

    if nav.walkToPointSpinCount > constants.GOTO_SURE_THRESH:
        return nav.goLater('spinToWalkHeading')

    bearing = MyMath.getRelativeBearing(my.x, my.y, my.h, nav.destX,nav.destY)
    distToDest = MyMath.dist(my.x, my.y, nav.destX, nav.destY)
    if distToDest < ChaseBallConstants.APPROACH_WITH_GAIN_DIST:
        gain = constants.GOTO_FORWARD_GAIN * MyMath.dist(my.x, my.y,
                                                         nav.destX, nav.destY)
    else :
        gain = 1.0

    sTheta = MyMath.clip(MyMath.sign(bearing) *
                         constants.GOTO_STRAIGHT_SPIN_SPEED *
                         NavMath.getRotScale(bearing),
                         -constants.GOTO_STRAIGHT_SPIN_SPEED,
                         constants.GOTO_STRAIGHT_SPIN_SPEED )

    if fabs(sTheta) < constants.MIN_SPIN_MAGNITUDE_WALK:
        sTheta = 0

    sX = MyMath.clip(constants.GOTO_FORWARD_SPEED*gain,
                     constants.WALK_TO_MIN_X_SPEED,
                     constants.WALK_TO_MAX_X_SPEED)

    nav.helper.setSpeed(sX, 0, sTheta)
    return nav.stay()

def spinToFinalHeading(nav):
    """
    Spins until we are facing the final desired heading
    Stops when at heading
    """
    if nav.firstFrame():
        nav.stopSpinToWalkCount = 0

    targetH = nav.destH

    headingDiff = nav.brain.my.h - targetH
    if DEBUG:
        nav.printf("Need to spin to %g, heading diff is %g,heading uncert is %g"
                   % (targetH, headingDiff, nav.brain.my.uncertH))
    spinDir = MyMath.getSpinDir(nav.brain.my.h, targetH)

    spin = spinDir*constants.GOTO_SPIN_SPEED*NavMath.getRotScale(headingDiff)

    if NavMath.atHeading(nav, targetH):
        nav.stopSpinToWalkCount += 1
    else:
        nav.stopSpinToWalkCount = 0

    if nav.stopSpinToWalkCount > constants.CHANGE_SPIN_DIR_THRESH:
        return nav.goLater('stop')

    nav.helper.setSpeed(0, 0, spin)
    return nav.stay()

def omniWalkToPoint(nav):
    if nav.firstFrame():
        nav.walkToPointCount = 0
    if nav.brain.play.isRole(GOALIE):
        if NavMath.atDestinationGoalie(nav) and NavMath.atHeading(nav):
            return nav.goNow('stop')
    else:
        if NavMath.atDestinationCloser(nav) and NavMath.atHeading(nav):
            return nav.goNow('stop')

    sX, sY, sTheta = nav.helper.getOmniWalkParam(nav.destX, nav.destY, nav.destH)
    nav.helper.setSpeed(sX, sY, sTheta)

    return nav.stay()

# State to be used with standard setSpeed movement
def walking(nav):
    """
    State to be used when setSpeed is called
    """
    if nav.firstFrame() or nav.updatedTrajectory:
        nav.helper.setSpeed(nav.walkX, nav.walkY, nav.walkTheta)
        nav.updatedTrajectory = False

    return nav.stay()
# State to use with the setSteps method
def stepping(nav):
    """
    We use this to go a specified number of steps.
    This is different from walking.
    """
    if nav.firstFrame():
        nav.helper.step(nav.stepX, nav.stepY, nav.stepTheta, nav.numSteps)
    elif not nav.brain.motion.isWalkActive():
        return nav.goNow("stopped")
    return nav.stay()

### Stopping States ###
def stop(nav):
    """
    Wait until the walk is finished.
    """
    if nav.firstFrame():
        nav.helper.setSpeed(0, 0, 0)
        nav.walkX = nav.walkY = nav.walkTheta = nav.stepX = nav.stepY \
                    = nav.stepTheta = nav.numSteps = 0

    if not nav.brain.motion.isWalkActive():
        return nav.goNow('stopped')

    return nav.stay()

def stopped(nav):

    return nav.stay()

def orbitPoint(nav):
    if nav.firstFrame():
        helper.setSpeed(nav, 0,
                     nav.orbitDir*constants.ORBIT_STRAFE_SPEED,
                     nav.orbitDir*constants.ORBIT_SPIN_SPEED )
    return nav.stay()


def orbitPointThruAngle(nav):
    """
    Circles around a point in front of robot, for a certain angle
    """
    if fabs(nav.angleToOrbit) < constants.MIN_ORBIT_ANGLE:
        return nav.goNow('stop')
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

        helper.setSpeed(nav, -0.5, orbitDir*sY, orbitDir*sT)

    #  (frames/second) / (degrees/second) * degrees
    framesToOrbit = fabs((constants.FRAME_RATE / nav.walkTheta) *
                         nav.angleToOrbit)
    if nav.counter >= framesToOrbit:
        return nav.goLater('stop')
    return nav.stay()
