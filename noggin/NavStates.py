''' States for finding our way on the field '''

from .util import MyMath

GOTO_FORWARD_SPEED = 4
WAIT_BETWEEN_MOVES = 0
GOTO_SPIN_SPEED = 10
GOTO_SPIN_STRAFE = 0
GOTO_SURE_THRESH = 5

CHANGE_SPIN_DIR_THRESH = 3

DEBUG = False
# States for the standard spin - walk - spin go to
def spinToWalkHeading(nav):
    """
    Spin to the heading needed to walk to a specific point
    """
    targetH = MyMath.getTargetHeading(nav.brain.my, nav.destX, nav.destY)
    headingDiff = abs(nav.brain.my.h - targetH)
    newSpinDir = MyMath.getSpinDir(nav.brain.my, targetH)


    if nav.firstFrame():
        nav.setSpeed(0,0,0)
        nav.stopSpinToWalkCount = 0
        nav.noWalkSet = True
        nav.curSpinDir = newSpinDir
        nav.changeSpinDirCounter = 0

    if nav.noWalkSet and nav.brain.motion.isWalkActive():
        nav.brain.CoA.setRobotTurnGait(nav.brain.motion)
        if DEBUG: nav.printf("Waiting for walk to stop")
        return nav.stay()


#     nav.printf("Target heading is " + str(targetH))
#     nav.printf("Current heading is " + str(nav.brain.my.h))

    spinDir = nav.curSpinDir
    if newSpinDir != nav.curSpinDir:
        nav.changeSpinDirCounter += 1
    else:
        nav.changeSpinDirCounter -= 1
        nav.changeSpinDirCounter = max(0,nav.changeSpinDirCounter)

    if nav.changeSpinDirCounter >  CHANGE_SPIN_DIR_THRESH:
        spinDir = newSpinDir
        nav.curSpinDir = spinDir
        nav.changeSpinDirCounter = 0
        if DEBUG: nav.printf("Switching spin directions my.h " + str(nav.brain.my.h)+
                   " and my target h: " + str(targetH))
    else:
        spinDir = nav.curSpinDir

    if spinDir == None:
        nav.printf("Spindir is none: nav.curSpinDir" +str(nav.curSpinDir) +
                   " newSpinDir is " + str(newSpinDir))
    nav.setSpeed(0, spinDir * GOTO_SPIN_STRAFE,
                 spinDir * GOTO_SPIN_SPEED*nav.getRotScale(headingDiff))
    nav.noWalkSet = False

    if nav.atHeading(targetH):
        nav.stopSpinToWalkCount += 1

    if nav.stopSpinToWalkCount > GOTO_SURE_THRESH:
        return nav.goLater('walkToPoint')

    return nav.stay()

def walkToPoint(nav):
    """
    State to walk forward until localization thinks we are close to the point
    Stops if we get there
    If we no longer are heading towards it change to the spin state
    """
    if nav.firstFrame():
        nav.setSpeed(0,0,0)
        nav.walkToPointCount = 0
        nav.walkToPointSpinCount = 0
        nav.noWalkSet  = True

    if nav.noWalkSet and nav.brain.motion.isWalkActive():
        nav.brain.CoA.setRobotGait(nav.brain.motion)
        if DEBUG: nav.printf("Waiting for walk to stop")
        return nav.stay()

    nav.setSpeed(GOTO_FORWARD_SPEED, 0, 0)
    nav.noWalkSet = False


    if nav.atDestination():
        nav.walkToPointCount += 1

    if nav.walkToPointCount > GOTO_SURE_THRESH:
        if nav.destH is None:
#             nav.printf("Stopping at position (" + str(nav.brain.my.x) +
#                        ", " + str(nav.brain.my.y) + ") going to (" + str(nav.destX)
#                        + ", " + str(nav.destY) + ")")
            return nav.goLater('stop')
        else:
            return nav.goLater('spinToFinalHeading')

    targetH = MyMath.getTargetHeading(nav.brain.my, nav.destX, nav.destY)

    if nav.notAtHeading(targetH):
        nav.walkToPointSpinCount += 1

    if nav.walkToPointSpinCount > GOTO_SURE_THRESH:
        return nav.goLater('spinToWalkHeading')

    return nav.stay()

def spinToFinalHeading(nav):
    """
    Spins until we are facing the final desired heading
    Stops when at heading
    """
    if nav.firstFrame():
        nav.setSpeed(0,0,0)
        nav.noWalkSet  = True

    if nav.noWalkSet and nav.brain.motion.isWalkActive():
        nav.brain.CoA.setRobotTurnGait(nav.brain.motion)
        nav.printf("Waiting for walk to stop")
        return nav.stay()

    targetH = nav.destH#MyMath.getTargetHeading(nav.brain.my, nav.destX, nav.destY)
    headingDiff = abs(nav.brain.my.h - targetH)
    if DEBUG: nav.printf("Need to spin to %g, heading diff is %g, heading uncert is %g" %
               (targetH, headingDiff, nav.brain.my.uncertH))
    spinDir = MyMath.getSpinDir(nav.brain.my, targetH)
    nav.setSpeed(0, spinDir*GOTO_SPIN_STRAFE, 
                 spinDir*GOTO_SPIN_SPEED*nav.getRotScale(headingDiff))
    nav.noWalkSet = False

    if nav.atHeading():
        return nav.goLater('stop')
    return nav.stay()

# State to be used with standard setSpeed movement
def walking(nav):
    """
    State to be used when setSpeed is called
    """
    if nav.firstFrame() or nav.updatedTrajectory:
        #HACK nav.brain.CoA.setRobotGait(nav.brain.motion)
#         nav.printf("New walk is (" + str(nav.walkX) + ", " +
#                    str(nav.walkY) + ", " + str(nav.walkTheta) + ")")
        nav.setSpeed(nav.walkX, nav.walkY, nav.walkTheta)
        nav.updatedTrajectory = False

    return nav.stay()
# State to use with the setSteps method
def stepping(nav):
    """
    We use this to go a specified number of steps.
    This is different from walking.
    """
    if nav.firstFrame():
        nav.step(nav.stepX, nav.stepY, nav.stepTheta, nav.numSteps)
    elif not nav.brain.motion.isWalkActive():
        return nav.goLater("stopped")
    return nav.stay()

### Stopping States ###
def stop(nav):
    '''
    Wait until the walk is finished.
    '''
    if nav.firstFrame():
        if nav.brain.motion.isWalkActive():
            nav.setSpeed(0,0,0)
        nav.walkX = nav.walkY = nav.walkTheta = 0


    if not nav.brain.motion.isWalkActive():
        return nav.goNow('stopped')

    return nav.stay()

def stopped(nav):
    nav.walkX = nav.walkY = nav.walkTheta = 0
    return nav.stay()
