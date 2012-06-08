import man.motion as motion
from math import fabs
from ..util import MyMath
import NavConstants as constants
from objects import RelLocation, RelRobotLocation, RobotLocation, Location

def stand(nav):    
    createAndSendWalkVector(nav, 0, 0, 0)


def getRelativeDestination(my, dest):
      
    field_dest = dest
    
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
        raise TypeError, "dest is not a Location type!" + str(dest) 
    
def isDestinationRelative(dest):
    return isinstance(dest, RelLocation)

def adaptSpeed(distance, cutoffDistance, maxSpeed):
    return MyMath.mapRange(distance, 0, cutoffDistance, 0, maxSpeed)

def getStrafelessDest(dest):
    if (dest.relX > 150 and dest.relY < 50) or (dest.relX <= 150 and dest.relX > 50 and dest.relY < 20) \
            or (dest.relX <= 50 and dest.relX > 20 and dest.relY < 10):
        #print "old dest: " + str(dest)
        return RelRobotLocation(dest.relX, 0, dest.relH)
    else:
        return dest
    
def setDestination(nav, dest, gain = 1.0):
    """
    Calls setDestination within the motion engine
    """
    nav.currentCommand = \
        motion.DestinationCommand(x=dest.relX, y=0, theta=dest.relH, gain=gain)
    nav.brain.motion.sendDestCommand(nav.currentCommand)

def getDeltaOdometry(loc, startingOdo):
    """
    returns a RelRobotLocation representing the RelRobotLocation of the current
    odometry relative to a starting odometry
    """
    (x1, y1, h1) = startingOdo
    deltaOdometry = RelRobotLocation(loc.lastOdoX - x1,
                                     loc.lastOdoY - y1,
                                     loc.lastOdoTheta)
    deltaOdometry.rotate(-h1)
    return deltaOdometry
   
def getOrbitLocation(radius, angle):
    """
    Returns the RelRobotLocation destination of an orbit
    """
    #@todo: pretty good aproximation for small radiuses and angles
    if angle > 0:
        return RelRobotLocation(0.1, radius, -angle)
    else:
        return RelRobotLocation(0.1, -radius, -angle)
    

def setSpeed(nav, speeds):
    """
    Wrapper method to easily change the walk vector of the robot
    """
    if speeds == constants.ZERO_SPEEDS:
        nav.printf("!!!!!! USE player.stopWalking() NOT walk(0,0,0)!!!!!")
        return

    createAndSendWalkVector(nav, *speeds)
    
def createAndSendWalkVector(nav, x, y, theta):
    walk = motion.WalkCommand(x=x,y=y,theta=theta)
    nav.brain.motion.setNextWalkCommand(walk)

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
