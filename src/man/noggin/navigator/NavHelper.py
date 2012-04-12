import man.motion as motion
from math import fabs
import NavConstants as constants
from objects import RelLocation, RelRobotLocation, RobotLocation, Location

# this will prevent us from rapidly switching back and forth between
# forwards & backwards gaits (happens in spinFindBall sometimes, bad)
BACKWARDS_GAIT_THRESH = -0.2
SPIN_GAIT_THRESH = 0.2

def stand(nav):
    nav.walkX = 0
    nav.walkY = 0
    nav.walkTheta = 0
        
    createAndSendWalkVector(nav, 0, 0, 0)
    

def getCurrentRelativeDestination(nav):
      
    my = nav.brain.my
    field_dest = nav.dest
    
    if isinstance(field_dest, RelRobotLocation):
        return field_dest
    
    elif isinstance(field_dest, RelLocation):
        return RelRobotLocation(field_dest.relX,
                                field_dest.relY,
                                field_dest.bearing)
          
    elif isinstance(field_dest, RobotLocation):
        return my.relativeRobotLocationOf(field_dest)
    
    elif isinstance(field_dest, Location):
        relLocation = my.relativeLocationOf(field_dest)
        return RelRobotLocation(relLocation.relX,
                                relLocation.relY,
                                relLocation.bearing)  
    
    else:
        raise TypeError, "nav.dest is not a Location type"

def setDestination(nav, x, y, theta, gain = 1.0):
    """
    Calls setDestination within the motion engine
    """
    nav.currentCommand = \
        motion.DestinationCommand(x=x, y=y, theta=theta, gain=gain)
    nav.brain.motion.sendDestCommand(nav.currentCommand)

    nav.updateDests(x, y, theta, gain)

def setSpeed(nav, x, y, theta):
    """
    Wrapper method to easily change the walk vector of the robot
    """
    if x == 0 and y == 0 and theta == 0 and \
            nav.brain.motion.isWalkActive():
        createAndSendWalkVector(nav, 0,0,0) # Ensure that STOP commands get sent

    # If our speed vector is already being used, don't bother changing
    # it. If we're not walking, however, we definitely want to send
    # the command, since it is certainly different.
    if nav.speedVectorsEqual(x, y, theta) and \
            nav.brain.motion.isWalkActive():
        return

    nav.updateSpeeds(x, y, theta)
    updateGait(nav, x, y, theta)

    x_cms, y_cms, theta_degs = convertWalkVector(nav.brain, x, y, theta)

    createAndSendWalkVector(nav, x_cms, y_cms, theta_degs)

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

    nav.updateSpeeds(x,y, theta)

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
    Convert the 0->1 values into actual cm values for the WalkCommand
    NOTE: x_abs means that x is bound on [-1,1] (not an absolute value)
    """
    checkWalkVector(x_abs, y_abs, theta_abs)

    gait = brain.CoA.current_gait

    x_mms = y_mms = theta_rads = 0

    #TODO: get these values from somewhere else or
    #just pass floats to motion and let it convert it to max speeds and such
    if x_abs > 0:
        x_mms = x_abs * 120 #gait.getStepValue(4) # max fwd X speed
    elif x_abs < 0:
        x_mms = x_abs * 100 #fabs(gait.getStepValue(5)) # max rev X speed

    # max Y speed (same in both directions)
    y_mms = y_abs * 50 #gait.getStepValue(6)

    # max theta speed (same in both directions)
    theta_rads = theta_abs * 0.8 #gait.getStepValue(7)

    x_cms = x_mms * constants.TO_CMS  # convert back from motion engine's units
    y_cms = y_mms * constants.TO_CMS
    theta_degs = theta_rads * constants.TO_DEGS

    return x_cms, y_cms, theta_degs

def checkWalkVector(x, y, theta):
    assert fabs(x) <= 1
    assert fabs(y) <= 1
    assert fabs(theta) <= 1

def createAndSendWalkVector(nav, x, y, theta):
    walk = motion.WalkCommand(x=x,y=y,theta=theta)
    nav.brain.motion.setNextWalkCommand(walk)

def updateGait(nav, x, y, theta):
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
