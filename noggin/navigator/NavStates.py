""" States for finding our way on the field """

from ..util import MyMath
from . import NavConstants as constants
from . import NavHelper as helper
from ..playbook.PBConstants import GOALIE
from math import fabs

DEBUG = False

def positioningPlaybook(nav):
    """positions us in ready state"""
    if nav.firstFrame():
        nav.spinToPointCount = 0
        nav.omniWalkToCount = 0
    my = nav.brain.my
    dest = nav.dest

    dest.h = my.getTargetHeading(dest)
    headingDiff = fabs(my.h - dest.h)

    if headingDiff < constants.HEADING_THRESHOLD_TO_SPIN:
        nav.spinToPointCount = 0
        walkX, walkY, walkTheta = helper.getWalkStraightParam(my, dest)
    else:
        nav.spinToPointCount += 1
        if nav.spinToPointCount > constants.FRAMES_THRESHHOLD_TO_SPIN:
            walkX, walkY, walkTheta = helper.getSpinOnlyParam(my, dest)
        else:
            walkX, walkY, walkTheta = helper.getWalkStraightParam(my, dest)

    helper.setSpeed(nav.brain.motion, walkX, walkY, walkTheta)
    nav.walkX, nav.walkY, nav.walkTheta = walkX, walkY, walkTheta
    nav.curSpinDir = MyMath.sign(walkTheta)

    if helper.useFinalHeading(nav.brain, dest):
        nav.omniWalkToCount += 1
        if nav.omniWalkToCount > constants.FRAMES_THRESHOLD_TO_POSITION_OMNI:
            del nav.omniWalkToCount
            del nav.spinToPointCount
            return nav.goLater('positionOmni')
    else:
        nav.omniWalkToCount = 0

    return nav.stay()

def positionOmni(nav):
    if nav.firstFrame():
        nav.stopOmniCount = 0
    my = nav.brain.my
    dest = nav.dest

    dest.h = 0 #NogginConstants.OPP_GOAL_HEADING
    walkX, walkY, walkTheta = helper.getOmniWalkParam(my, dest)

    helper.setSpeed(nav.brain.motion, walkX, walkY, walkTheta)
    nav.walkX, nav.walkY, nav.walkTheta = walkX, walkY, walkTheta
    nav.curSpinDir = MyMath.sign(walkTheta)

    if not helper.useFinalHeading(nav.brain, dest):
        nav.stopOmniCount += 1
        if nav.stopOmniCount > constants.FRAMES_THRESHOLD_TO_POSITION_PLAYBOOK:
            del nav.stopOmniCount
            return nav.goLater('positioningPlaybook')

    if nav.brain.play.isRole(GOALIE):
        if helper.atDestinationGoalie(my, dest) and helper.atHeading(my, dest.h):
            del nav.stopOmniCount
            return nav.goNow('stop')
    else:
        if helper.atDestinationCloser(my, dest) and helper.atHeading(my, dest.h):
            del nav.stopOmniCount
            return nav.goNow('stop')

    return nav.stay()


def doingSweetMove(nav):
    '''executes the currently set sweetmove'''
    motion = nav.brain.motion
    if nav.firstFrame():
        helper.setSpeed(nav, 0, 0, 0)
        helper.executeMove(motion, nav.sweetMove)

    if not motion.isBodyActive():
        del nav.sweetMove
        return nav.goNow('stopped')

    return nav.stay()


# States for the standard spin - walk - spin go to
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

    nav.dest.h = nav.dest.getTargetHeading(my)

    if helper.notAtHeading(my, nav.dest.h):
        nav.walkToPointSpinCount += 1
    else :
        nav.walkToPointSpinCount = 0

    if nav.walkToPointSpinCount > constants.GOTO_SURE_THRESH:
        return nav.goLater('spinToWalkHeading')

    walkY, walkX, walkTheta = helper.getWalkStraightParam(my, nav.dest)

    helper.setSpeed(nav, walkX, walkY, walkTheta)

    return nav.stay()

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

    headingDiff = fabs(nav.brain.my.h - targetH)
    sTheta = nav.curSpinDir * constants.GOTO_SPIN_SPEED * \
        helper.getRotScale(headingDiff)

    if sTheta != nav.walkTheta:
        helper.setSpeed(nav, 0, 0, sTheta)

    if nav.stopSpinToWalkCount > constants.GOTO_SURE_THRESH:
        return nav.goLater('walkStraightToPoint')
    if helper.atDestinationCloser(my, nav.dest):
        return nav.goLater('spinToFinalHeading')

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

    helper.setSpeed(nav, 0, 0, spin)
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

    walkX, walkY, walkTheta = helper.getOmniWalkParam(my, dest)
    helper.setSpeed(nav, walkX, walkY, walkTheta)

    return nav.stay()

# State to be used with standard setSpeed movement
def walking(nav):
    """
    State to be used when setSpeed is called
    """
    if nav.updatedTrajectory:
        helper.setSpeed(nav, nav.walkX, nav.walkY, nav.walkTheta)
        nav.updatedTrajectory = False

    return nav.stay()
# State to use with the setSteps method
def stepping(nav):
    """
    We use this to go a specified number of steps.
    This is different from walking.
    """
    if nav.firstFrame():
        helper.step(nav, nav.stepX, nav.stepY, nav.stepTheta, nav.numSteps)
    elif not nav.brain.motion.isWalkActive():
        return nav.goNow("stopped")
    return nav.stay()

### Stopping States ###
def stop(nav):
    """
    Wait until the walk is finished.
    """
    if nav.firstFrame():
        helper.setSpeed(nav, 0, 0, 0)
        nav.walkX = nav.walkY = nav.walkTheta = nav.stepX = nav.stepY \
                    = nav.stepTheta = nav.numSteps = 0

    if not nav.brain.motion.isWalkActive():
        return nav.goNow('stopped')

    return nav.stay()

def stopped(nav):

    return nav.stay()

def orbitPoint(nav):
    if nav.updatedTrajectory:
        helper.setSpeed(nav, nav.walkX, nav.walkY, nav.walkTheta)
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

        walkX = -0.5
        walkY = orbitDir*sY
        walkTheta = orbitDir*sT
        helper.setSpeed(nav, walkX, walkY, walkTheta )

    #  (frames/second) / (degrees/second) * degrees
    framesToOrbit = fabs((constants.FRAME_RATE / nav.walkTheta) *
                         nav.angleToOrbit)
    if nav.counter >= framesToOrbit:
        return nav.goLater('stop')
    return nav.stay()
