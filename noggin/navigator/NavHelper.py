from . import NavConstants as constants
from ..players import ChaseBallConstants
from math import fabs, cos, sin, radians
from man.noggin.util import MyMath
import man.motion as motion

def setSpeed(motionInst, x, y, theta):
    """
    Wrapper method to easily change the walk vector of the robot
    """
    walk = motion.WalkCommand(x=x,y=y,theta=theta)
    motionInst.setNextWalkCommand(walk)

def step(motionInst, x, y, theta, numSteps):
    """
    Wrapper method to easily change the walk vector of the robot
    """
    steps = motion.StepCommand(x=x,y=y,theta=theta,numSteps=numSteps)
    motionInst.sendStepCommand(steps)

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

def getOmniWalkParam(my, dest):

    bearing = radians(my.getRelativeBearing(dest))

    distToDest = my.dist(dest)

    # calculate forward speed
    forwardGain = constants.OMNI_GOTO_X_GAIN * distToDest* \
        cos(bearing)
    sX = constants.OMNI_GOTO_FORWARD_SPEED * forwardGain
    sX = MyMath.clip(sX,
                     constants.OMNI_MIN_X_SPEED,
                     constants.OMNI_MAX_X_SPEED)
    if fabs(sX) < constants.OMNI_MIN_X_MAGNITUDE:
        sX = 0

    # calculate sideways speed
    strafeGain = constants.OMNI_GOTO_Y_GAIN * distToDest* \
        sin(bearing)
    sY = constants.OMNI_GOTO_STRAFE_SPEED  * strafeGain
    sY = MyMath.clip(sY,
                     constants.OMNI_MIN_Y_SPEED,
                     constants.OMNI_MAX_Y_SPEED,)
    if fabs(sY) < constants.OMNI_MIN_Y_MAGNITUDE:
        sY = 0

    if atDestinationCloser(my, dest):
        sX = sY = 0.0

    # calculate spin speed
    spinGain = constants.GOTO_SPIN_GAIN
    spinDir = my.spinDirToHeading(dest.h)
    sTheta = spinDir * fabs(my.h - dest.h) * spinGain
    sTheta = MyMath.clip(sTheta,
                         constants.OMNI_MIN_SPIN_SPEED,
                         constants.OMNI_MAX_SPIN_SPEED)
    if fabs(sTheta) < constants.OMNI_MIN_SPIN_MAGNITUDE:
        sTheta = 0.0

    if atHeading(my, dest.h):
        sTheta = 0.0

    return (sX, sY, sTheta)

def getWalkStraightParam(my, dest):

    bearing = my.getRelativeBearing(dest)
    distToDest = my.dist(dest)

    if distToDest < ChaseBallConstants.APPROACH_WITH_GAIN_DIST:
        gain = constants.GOTO_FORWARD_GAIN * distToDest
    else :
        gain = 1.0

    sTheta = MyMath.clip(MyMath.sign(bearing) *
                         constants.GOTO_STRAIGHT_SPIN_SPEED *
                         getRotScale(bearing),
                         -constants.GOTO_STRAIGHT_SPIN_SPEED,
                         constants.GOTO_STRAIGHT_SPIN_SPEED )

    if fabs(sTheta) < constants.MIN_SPIN_MAGNITUDE_WALK:
        sTheta = 0

    sX = MyMath.clip(constants.GOTO_FORWARD_SPEED*gain,
                     constants.WALK_TO_MIN_X_SPEED,
                     constants.WALK_TO_MAX_X_SPEED)
    sY = 0

    return (sX, sY, sTheta)


def atDestination(my, dest):
    """
    Returns true if we are at an (x, y) close enough to the one we want
    """
    return ( fabs(my.x - dest.x) < constants.CLOSE_ENOUGH_XY and
             fabs(my.y - dest.y) < constants.CLOSE_ENOUGH_XY)

def atDestinationCloser(my, dest):
    """
    Returns true if we are at an (x, y) close enough to the one we want
    """
    return (fabs(my.x - dest.x) < constants.CLOSER_XY and
            fabs(my.y - dest.y) < constants.CLOSER_XY)

def atDestinationGoalie(my, dest):
    return (fabs(my.x - dest.x) < constants.GOALIE_CLOSE_X and
            fabs(my.y - dest.y) < constants.GOALIE_CLOSE_Y)

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

def getRotScale(headingDiff):
    absHDiff = fabs(headingDiff)
    if absHDiff < constants.HEADING_NEAR_THRESH:
        return constants.HEADING_NEAR_SCALE
    elif absHDiff < constants.HEADING_MEDIUM_THRESH:
        return constants.HEADING_MEDIUM_SCALE
    else:
        return constants.HEADING_FAR_SCALE

def useFinalHeading(brain, position):
    if brain.gameController.currentState == 'gameReady':
        useFinalHeadingDist = constants.FINAL_HEADING_READY_DIST
    else:
        useFinalHeadingDist = constants.FINAL_HEADING_DIST

    distToPoint = brain.my.dist(position)

    return (distToPoint <= useFinalHeadingDist)
