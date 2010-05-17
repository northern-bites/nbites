from . import NavConstants as constants
from math import fabs
from man.noggin.util import MyMath
import man.motion as motion

def setSpeed(nav, x, y, theta):
    """
    Wrapper method to easily change the walk vector of the robot
    """
    walk = motion.WalkCommand(x=x,y=y,theta=theta)
    nav.brain.motion.setNextWalkCommand(walk)

    nav.walkX, nav.walkY, nav.walkTheta = x, y, theta
    nav.curSpinDir = MyMath.sign(theta)


def step(nav, x, y, theta, numSteps):
    """
    Wrapper method to easily change the walk vector of the robot
    """
    steps = motion.StepCommand(x=x,y=y,theta=theta,numSteps=numSteps)
    nav.brain.motion.sendStepCommand(steps)

    nav.walkX, nav.walkY, nav.walkTheta = x, y, theta
    nav.curSpinDir = MyMath.sign(theta)

def executeMove(motionInst, sweetMove):
    """
    Method to enqueue a SweetMove
    Can either take in a head move or a body command
    (see SweetMove files for descriptions of command tuples)
    """

    for position in sweetMove:
        if len(position) == 7:
            move = motion.BodyJointCommand(position[4], #time
                                           position[0], #larm
                                           position[1], #lleg
                                           position[2], #rleg
                                           position[3], #rarm
                                           position[6], # Chain Stiffnesses
                                           position[5], #interpolation type
                                           )

        elif len(position) == 5:
            move = motion.BodyJointCommand(position[2], # time
                                           position[0], # chainID
                                           position[1], # chain angles
                                           position[4], # chain stiffnesses
                                           position[3], # interpolation type
                                           )

        else:
            print("What kind of sweet ass-Move is this?")

        motionInst.enqueue(move)


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
