import man.motion as motion
from man.noggin.util import MyMath
from math import fabs
import NavConstants as constants

# this will prevent us from rapidly switching back and forth between
# forwards & backwards gaits (happens in spinFindBall sometimes, bad)
BACKWARDS_GAIT_THRESH = -0.2
SPIN_GAIT_THRESH = 0.2


def setSpeed(nav, x, y, theta):
    """
    Wrapper method to easily change the walk vector of the robot
    """

    #print (theta**2 + x**2 + y**2)
    # use backwards gait if appropriate
    if x < BACKWARDS_GAIT_THRESH:
        nav.brain.CoA.setRobotBackwardsGait(nav.brain.motion)
    elif fabs(theta) > SPIN_GAIT_THRESH and \
            fabs(x) < 0.1 and fabs(y) < 0.15 and \
            fabs(theta) > fabs(x) and \
            (theta**2 + x**2 + y**2) > 0.1:

        nav.brain.CoA.setRobotSlowGait(nav.brain.motion)
    else:
        nav.brain.CoA.setRobotGait(nav.brain.motion)

    x_cms, y_cms, theta_degs = convertWalkVector(nav.brain, x, y, theta)

    createAndSendWalkVector(nav, x_cms, y_cms, theta_degs)

    nav.walkX, nav.walkY, nav.walkTheta = x, y, theta
    nav.curSpinDir = MyMath.sign(theta)

def setDribbleSpeed(nav, x, y, theta):
    """
    Wrapper to set walk vector while using dribble gait
    """
    if x < BACKWARDS_GAIT_THRESH:
        nav.brain.CoA.setRobotBackwardsGait(nav.brain.motion)
    else:
        nav.brain.CoA.setDribbleGait(nav.brain.motion)

    x_cms, y_cms, theta_degs = convertWalkVector(nav.brain, x, y, theta)

    createAndSendWalkVector(nav, x_cms, y_cms, theta_degs)

    nav.walkX, nav.walkY, nav.walkTheta = x, y, theta
    nav.curSpinDir = MyMath.sign(theta)

def setSlowSpeed(nav, x, y, theta):
    """
    Wrapper to set walk vector while using slow gait
    TODO: dynamic gait so this is unnecessary
    """
    setSpeed(nav, x, y, theta)

#     if x < BACKWARDS_GAIT_THRESH:
#         nav.brain.CoA.setRobotBackwardsGait(nav.brain.motion)
#     else:
#         nav.brain.CoA.setRobotSlowGait(nav.brain.motion)

#     x_cms, y_cms, theta_degs = convertWalkVector(nav.brain, x, y, theta)

#     createAndSendWalkVector(nav, x_cms, y_cms, theta_degs)

#     nav.walkX, nav.walkY, nav.walkTheta = x, y, theta
#    nav.curSpinDir = MyMath.sign(theta)

def step(nav, x, y, theta, numSteps):
    """
    Wrapper method to easily change the walk vector of the robot
    """
    if x < BACKWARDS_GAIT_THRESH:
        nav.brain.CoA.setRobotBackwardsGait(nav.brain.motion)
    else:
        nav.brain.CoA.setRobotSlowGait(nav.brain.motion)

    x_cms, y_cms, theta_degs = convertWalkVector(nav.brain, x, y, theta)

    createAndSendStepsVector(nav, x_cms, y_cms, theta_degs)

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

def convertWalkVector(brain, x_abs, y_abs, theta_abs):
    """
    NOTE: x_abs means that x is bound on [-1,1] (not an absolute value)
    """

    checkWalkVector(x_abs, y_abs, theta_abs)

    gait = brain.CoA.current_gait

    x_mms = y_mms = theta_rads = 0

    if x_abs > 0:
        x_mms = x_abs * gait.getStepValue(4) # max fwd X speed
    elif x_abs < 0:
        x_mms = x_abs * fabs(gait.getStepValue(5)) # max rev X speed

    # max Y speed (same in both directions)
    y_mms = y_abs * gait.getStepValue(6)

    # max theta speed (same in both directions)
    theta_rads = theta_abs * gait.getStepValue(7)

    x_cms = x_mms * constants.TO_CMS  # convert back from motion engine's units
    y_cms = y_mms * constants.TO_CMS
    theta_degs = theta_rads * constants.TO_DEGS

    return x_cms, y_cms, theta_degs

def checkWalkVector(x, y, theta):
    assert fabs(x) <= 1
    assert fabs(y) <= 1
    assert fabs(theta) <= 1

def createAndSendStepsVector(nav, x, y, theta):
    steps = motion.StepCommand(x=x, y=y, theta=theta, numSteps=numSteps)
    nav.brain.motion.sendStepCommand(steps)

def createAndSendWalkVector(nav, x, y, theta):
    walk = motion.WalkCommand(x=x,y=y,theta=theta)
    nav.brain.motion.setNextWalkCommand(walk)
