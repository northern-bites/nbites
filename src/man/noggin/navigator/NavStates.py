""" States for finding our way on the field """

from ..util import MyMath
from . import NavConstants as constants
from . import NavHelper as helper
from . import WalkHelper as walker
from . import NavTransitions as navTrans
from math import fabs

DEBUG = False

def doingSweetMove(nav):
    '''State that we stay in while doing sweet moves'''
    if nav.firstFrame():
        nav.doingSweetMove = True
        return nav.stay()

    if not nav.brain.motion.isBodyActive():
        nav.doingSweetMove = False
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
    if navTrans.atDestinationCloser(my, nav.dest):
        nav.walkToPointCount += 1
    else :
        nav.walkToPointCount = 0

    if nav.walkToPointCount > constants.GOTO_SURE_THRESH:
        return nav.goLater('spinToFinalHeading')

    destH = nav.brain.my.headingTo(nav.dest)

    if not navTrans.atHeadingGoTo(my, destH):
        nav.walkToPointSpinCount += 1
        if nav.walkToPointSpinCount > constants.GOTO_SURE_THRESH + 25:
            return nav.goLater('spinToWalkHeading')

    else :
        nav.walkToPointSpinCount = 0

    walkX, walkY, walkTheta = walker.getWalkStraightParam(my, nav.dest)

    helper.setSpeed(nav, walkX, walkY, walkTheta)

    return nav.stay()

def spinToWalkHeading(nav):
    """
    Spin to the heading needed to walk to a specific point
    """
    my = nav.brain.my
    headingDiff = my.getRelativeBearing(nav.dest)
    newSpinDir = MyMath.sign(headingDiff)

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
                             " and my h diff: " + str(headingDiff))

    if headingDiff < 20.:
        nav.stopSpinToWalkCount += 1
        if nav.stopSpinToWalkCount > constants.GOTO_SURE_THRESH:
            return nav.goLater('walkStraightToPoint')

    else :
        nav.stopSpinToWalkCount = 0

    sTheta = nav.curSpinDir * constants.MAX_SPIN_MAGNITUDE * \
        walker.getRotScale(headingDiff)

    if sTheta != nav.walkTheta:
        helper.setSpeed(nav, 0, 0, sTheta)

    if navTrans.atDestinationCloser(my, nav.dest):
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
    headingDiff = MyMath.sub180Angle(nav.brain.my.h - targetH)

    if DEBUG:
        nav.printf("Need to spin to %g, heading diff is %g,heading uncert is %g"
                   % (targetH, headingDiff, nav.brain.my.uncertH))
    spinDir = MyMath.sign(headingDiff)

    spin = spinDir*constants.MAX_SPIN_MAGNITUDE*walker.getRotScale(headingDiff)

    if navTrans.atHeading(nav.brain.my, targetH):
        nav.stopSpinToWalkCount += 1
    else:
        nav.stopSpinToWalkCount = 0

    if nav.stopSpinToWalkCount > constants.CHANGE_SPIN_DIR_THRESH:
        return nav.goLater('stop')

    helper.setSpeed(nav, 0, 0, spin)
    return nav.stay()

# WARNING: avoidObstacle could possibly go into our own box
def avoidObstacle(nav):
    """
    If we detect something in front of us, dodge it
    """

    avoidLeft = navTrans.shouldAvoidObstacleLeft(nav)
    avoidRight = navTrans.shouldAvoidObstacleRight(nav)

    # store previous state here, b/c lastDiffState gets
    # replaced when we perform 'goNow'
    nav.preAvoidState = nav.lastDiffState

    if (avoidLeft and avoidRight):
        return nav.goNow('avoidFrontObstacle')
    elif avoidLeft:
        return nav.goNow('avoidLeftObstacle')
    elif avoidRight:
        return nav.goNow('avoidRightObstacle')
    else:
        return nav.goNow(nav.lastDiffState)


def avoidFrontObstacle(nav):
    # Backup
    # strafe away from the closer one?
    # strafe towards dest?

    # ever a good time to backup?
    # we'll probably want to go forward again and most obstacle
    # are moving, so pausing might make more sense

    if nav.firstFrame():
        nav.doneAvoidingCounter = 0
        nav.printf(nav.brain.sonar)
        nav.printf("Avoid by backup");
        helper.setSpeed(nav, constants.DODGE_BACK_SPEED, 0, 0)

    avoidLeft = navTrans.shouldAvoidObstacleLeft(nav)
    avoidRight = navTrans.shouldAvoidObstacleRight(nav)

    if (avoidLeft and avoidRight):
        nav.doneAvoidingCounter -= 1
        nav.doneAvoidingCounter = max(0, nav.doneAvoidingCounter)
        return nav.stay()

    elif avoidRight:
        return nav.goNow('avoidRightObstacle')

    elif avoidLeft:
        return nav.goNow('avoidLeftObstacle')

    else:
        nav.doneAvoidingCounter += 1

    if nav.doneAvoidingCounter > constants.DONE_AVOIDING_FRAMES_THRESH:
        nav.shouldAvoidObstacleRight = 0
        nav.shouldAvoidObstacleLeft = 0
        return nav.goLater(nav.preAvoidState)

    return nav.stay()

def avoidLeftObstacle(nav):
    """
    dodges right if we only detect something to the left of us
    """

    if nav.firstFrame():
        nav.doneAvoidingCounter = 0
        nav.printf(nav.brain.sonar)
        nav.printf("Avoid by right dodge");
        helper.setSpeed(nav, 0, constants.DODGE_RIGHT_SPEED, 0)

    avoidLeft = navTrans.shouldAvoidObstacleLeft(nav)
    avoidRight = navTrans.shouldAvoidObstacleRight(nav)

    if (avoidLeft and avoidRight):
        return nav.goNow('avoidFrontObstacle')
    elif avoidRight:
        return nav.goNow('avoidRightObstacle')
    elif avoidLeft:
        nav.doneAvoidingCounter -= 1
        nav.doneAvoidingCounter = max(0, nav.doneAvoidingCounter)
        return nav.stay()
    else:
        nav.doneAvoidingCounter += 1

    if nav.doneAvoidingCounter > constants.DONE_AVOIDING_FRAMES_THRESH:
        nav.shouldAvoidObstacleRight = 0
        nav.shouldAvoidObstacleLeft = 0
        return nav.goLater(nav.preAvoidState)

    return nav.stay()

def avoidRightObstacle(nav):
    """
    dodges left if we only detect something to the left of us
    """

    if nav.firstFrame():
        nav.doneAvoidingCounter = 0
        nav.printf(nav.brain.sonar)
        nav.printf("Avoid by left dodge");
        helper.setSpeed(nav, 0, constants.DODGE_LEFT_SPEED, 0)

    avoidLeft = navTrans.shouldAvoidObstacleLeft(nav)
    avoidRight = navTrans.shouldAvoidObstacleRight(nav)

    if (avoidLeft and avoidRight):
        return nav.goNow('avoidFrontObstacle')
    elif avoidLeft:
        return nav.goNow('avoidLeftObstacle')
    elif avoidRight:
        nav.doneAvoidingCounter -= 1
        nav.doneAvoidingCounter = max(0, nav.doneAvoidingCounter)
        return nav.stay()
    else:
        nav.doneAvoidingCounter += 1

    if nav.doneAvoidingCounter > constants.DONE_AVOIDING_FRAMES_THRESH:
        nav.shouldAvoidObstacleRight = 0
        nav.shouldAvoidObstacleLeft = 0
        return nav.goLater(nav.preAvoidState)

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

def destWalking(nav):
    """
    State to be used when we are walking to a destination
    """
    if nav.firstFrame() or nav.newDestination:
        if (nav.destGain < 0):
            nav.destGain = 1;
            # @todo Leaving the actual interface of the destGain parameter unimplemented
            # Wils, figure out how you want to set it up --Nathan

        helper.setDestination(nav, nav.destX, nav.destY, nav.destTheta, nav.destGain)
        nav.newDestination = False

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
        nav.walkX = nav.walkY = nav.walkTheta = \
                    nav.stepX = nav.stepY = nav.stepTheta = nav.numSteps = 0

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

        sY = constants.ORBIT_STRAFE_SPEED

        sT = constants.ORBIT_SPIN_SPEED

        walkX = (nav.brain.ball.relX - 25) * .8
        walkY = -1 * MyMath.sign(nav.angleToOrbit) * 10
        walkTheta = MyMath.sign(nav.angleToOrbit) * 20

        helper.setSpeed(nav, walkX, walkY, walkTheta )

    #  (frames/second) / (degrees/second) * degrees
    framesToOrbit = 100

    if nav.counter >= framesToOrbit:
        return nav.goLater('stop')
    return nav.stay()
