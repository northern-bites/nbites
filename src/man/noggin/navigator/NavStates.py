""" States for finding our way on the field """
from . import NavConstants as constants
from . import NavHelper as helper
from . import WalkHelper as walker
from . import NavTransitions as navTrans
from objects import RobotLocation

from math import fabs

DEBUG = False

def doingSweetMove(nav):
    '''State that we stay in while doing sweet moves'''
    if nav.firstFrame():
        return nav.stay()

    if not nav.brain.motion.isBodyActive():
        return nav.goNow('stopped')

    return nav.stay()

def goToPosition(nav):
    """
    Go to a position set in the navigator. General go to state.  Goes
    towards a x, y position on the field stored in dest.
    If there is an h specified in the destination, that will be the 
    selected heading. Otherwise, the bearing to the object is used.
    """
    
    if nav.firstFrame():
        navTrans.atDestination.count = 0
        
    if nav.counter % 10 is 0:
        print "going to " + str(relDest)
        print "trans counter " + str(navTrans.atDestination.count)

    relDest = helper.getCurrentRelativeDestination(nav) 
    helper.setDestination(nav, relDest.relX, relDest.relY, relDest.relH)

    if navTrans.shouldAvoidObstacle(nav):
        return nav.goLater('avoidObstacle')

    return navTrans.transition(nav, navTrans.atDestination, 'atPosition', 
                               constants.FRAMES_THRESHOLD_TO_AT_POSITION)

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
        return nav.goLater(nav.lastDiffState)


def avoidFrontObstacle(nav):
    # Backup
    # strafe away from the closer one?
    # strafe towards dest?

    # ever a good time to backup?
    # we'll probably want to go forward again and most obstacle
    # are moving, so pausing might make more sense

    # TODO figure this out maybe potential field will fix this for us???
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
        return nav.goLater('avoidRightObstacle')

    elif avoidLeft:
        return nav.goLater('avoidLeftObstacle')

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
        return nav.goLater('avoidFrontObstacle')
    elif avoidRight:
        return nav.goLater('avoidRightObstacle')
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
        return nav.goLater('avoidFrontObstacle')
    elif avoidLeft:
        return nav.goLater('avoidLeftObstacle')
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
    helper.setSpeed(nav, nav.walkX, nav.walkY, nav.walkTheta)

    return nav.stay()
    
def standing(nav):
    return nav.stay()
    

def destWalking(nav):
    """
    State to be used when we are walking to a destination
    """
    if nav.firstFrame():
        nav.nearDestination = False

        helper.setDestination(nav,
                              nav.destX,
                              nav.destY,
                              nav.destTheta)

    # the frames remaining counter is sometimes set to -1 initially
    elif -1 != nav.currentCommand.framesRemaining() < 40:
        nav.nearDestination = True

    if DEBUG and nav.counter % 3 is 0:
        print "destCommand in progress: {0} frames, {1} X {2} Y remaining"\
              .format(nav.currentCommand.framesRemaining(),
                      nav.currentCommand.remainingX(),
                      nav.currentCommand.remainingY())

    if nav.counter > 1 and \
           (nav.currentCommand.isDone() or
            not nav.brain.motion.isWalkActive()):
        if DEBUG:
            print "isWalkActive? {0}, commandDone? {1}, counter? {2}"\
                  .format(nav.brain.motion.isWalkActive(),
                          nav.currentCommand.isDone(),
                          nav.counter)
        nav.nearDestination = True
        return nav.goNow('atPosition')

    return nav.stay()

def orbitPointThruAngle(nav):
    """
    Circles around a point in front of robot, for a certain angle
    """
    if fabs(nav.angleToOrbit) < constants.MIN_ORBIT_ANGLE:
        return nav.goNow('stop')

    if nav.angleToOrbit < 0:
        orbitDir = constants.ORBIT_LEFT
    else:
        orbitDir = constants.ORBIT_RIGHT

    if nav.counter % 8 == 0:
        #determine speeds for orbit
        ball = nav.brain.ball

        #want x to keep a radius of 17 from the ball, increase and
        #decrease x velocity as we move farther away from that dist
        walkX = (ball.loc.relX - 15) * .037
        if walkX > 1:
            walkX = 1
        elif walkX < -1:
            walkX = -1

        #keep constant y velocity, let x and theta change
        walkY = orbitDir * .85
        if walkY > 1:
            walkY = 1
        elif walkY < -1:
            walkY = -1

        #Vary theta based on ball bearing.  increase theta velocity as
        #we get farther away from facing the ball
        walkTheta = orbitDir * ball.bearing * .052
        if walkTheta > 1:
            walkTheta = 1
        elif walkTheta < -1:
            walkTheta = -1

        #set speed for orbit
        helper.setSpeed(nav, walkX, walkY, walkTheta )

    #Funny enough, we orbit about 1 degree per two frames,
    #So the angle can be used as a thresh

    if nav.counter >= nav.angleToOrbit:
        return nav.goLater('stop')
    return nav.stay()

### Stopping States ###
def stop(nav):
    """
    Wait until the walk is finished.
    """
    if nav.firstFrame():
        # stop walk vectors
        helper.setSpeed(nav, 0, 0, 0)
        nav.destType = None
        nav.resetDestMemory()
        nav.resetSpeedMemory()

    if not nav.brain.motion.isWalkActive():
        return nav.goNow('stopped')

    return nav.stay()

def stopped(nav):
    if nav.firstFrame():
        nav.destType = None
    return nav.stay()

# TODO: make this a stopping state elsewhere in the code.
def atPosition(nav):
    if nav.firstFrame():
        nav.brain.speech.say("At Position")
        navTrans.notAtDestination.count = 0
        helper.setSpeed(nav, 0, 0, 0)

    relDest = helper.getCurrentRelativeDestination(nav)
    
    return navTrans.transition(nav, navTrans.notAtDestination, 'goToPosition',
                               constants.FRAMES_THRESHOLD_TO_AT_POSITION)
