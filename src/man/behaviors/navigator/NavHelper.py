from math import fabs
from ..util import MyMath
import NavConstants as constants
from objects import RelLocation, RelRobotLocation, RobotLocation, Location
# TODO: Import CommandType properly.
#import PMotion_proto

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
        raise TypeError, "Navigator dest is not a Location type!" + str(dest)

def isDestinationRelative(dest):
    return isinstance(dest, RelLocation)

def adaptSpeed(distance, cutoffDistance, maxSpeed):
    return MyMath.mapRange(distance, 0, cutoffDistance, 0, maxSpeed)

def getStrafelessDest(dest):
    if ((dest.relX > 150 and dest.relY < 50) or
        (dest.relX <= 150 and dest.relX > 50 and dest.relY < 20) or
        (dest.relX <= 50 and dest.relX > 20 and dest.relY < 10)):
        #print "old dest: " + str(dest)
        return RelRobotLocation(dest.relX, 0, dest.relH)
    else:
        return dest

def setDestination(nav, dest, gain = 1.0):
    """
    Calls setDestination within the motion engine
    """
    # TODO: distinguish from setOdometryDestination method
    #       this method should overwrite motion commands.
    #       or, deprecate this method and use speed commands
    #       via the createAndSendWalkVector method.
    command = nav.brain.interface.bodyMotionCommand
    command.type = command.CommandType.DESTINATION_WALK #Destination Walk
    command.dest.rel_x = dest.relX
    command.dest.rel_y = dest.relY
    command.dest.rel_h = dest.relH
    # Mark this message for sending
    command.timestamp = int(nav.brain.time * 1000)

def setOdometryDestination(nav, dest, gain = 1.0):
    # TODO: distinguish from setDestination method
    #       this method should enqueue motion commands.
    command = nav.brain.interface.bodyMotionCommand
    command.type = command.CommandType.DESTINATION_WALK #Destination Walk
    command.dest.rel_x = dest.relX
    command.dest.rel_y = dest.relY
    command.dest.rel_h = dest.relH
    # Mark this message for sending
    command.timestamp = int(nav.brain.time * 1000)

#not used!
def getOrbitLocation(radius, angle):
    """
    Returns the RelRobotLocation destination of an orbit
    """
    if angle > 0:
        return RelRobotLocation(0.0, radius / 2, -angle)
    else:
        return RelRobotLocation(0.0, -radius / 2, -angle)

    dest = RelRobotLocation(radius, 0, 0)
    dest.rotate(-angle)
    return RelRobotLocation(0.0, -dest.relY, -angle)

def setSpeed(nav, speeds):
    """
    Wrapper method to easily change the walk vector of the robot
    """
    if speeds == constants.ZERO_SPEEDS:
        nav.printf("!!!!!! USE player.stopWalking() NOT walk(0,0,0)!!!!!")
        return

    createAndSendWalkVector(nav, *speeds)

def createAndSendWalkVector(nav, x, y, theta):
    command = nav.brain.interface.bodyMotionCommand
    command.type = command.CommandType.WALK_COMMAND #Walk Command
    command.speed.x = x
    command.speed.y = y
    command.speed.h = theta
    # Mark this message for sending
    command.timestamp = int(nav.brain.time * 1000)

def executeMove(nav, sweetMove):
    """
    Method to enqueue a SweetMove
    Can either take in a head move or a body command
    (see SweetMove files for descriptions of command tuples)
    """
    command = nav.brain.interface.bodyMotionCommand
    command.type = command.CommandType.SCRIPTED_MOVE #Scripted Move

    for position in sweetMove:
        if len(position) == 7:
            move = command.script.add_command() # Current body joint command

            move.time = position[4]
            if position[5] == 1:
                move.interpolation = move.InterpolationType.LINEAR
            else:
                move.interpolation = move.InterpolationType.SMOOTH

            move.angles.l_shoulder_pitch = position[0][0]
            move.angles.l_shoulder_roll =  position[0][1]
            move.angles.l_elbow_yaw =      position[0][2]
            move.angles.l_elbow_roll =     position[0][3]
            move.angles.l_hip_yaw_pitch =  position[1][0]
            move.angles.l_hip_roll =       position[1][1]
            move.angles.l_hip_pitch =      position[1][2]
            move.angles.l_knee_pitch =     position[1][3]
            move.angles.l_ankle_pitch =    position[1][4]
            move.angles.l_ankle_roll =     position[1][5]
            move.angles.r_hip_yaw_pitch =  position[2][0]
            move.angles.r_hip_roll =       position[2][1]
            move.angles.r_hip_pitch =      position[2][2]
            move.angles.r_knee_pitch =     position[2][3]
            move.angles.r_ankle_pitch =    position[2][4]
            move.angles.r_ankle_roll =     position[2][5]
            move.angles.r_shoulder_pitch = position[3][0]
            move.angles.r_shoulder_roll =  position[3][1]
            move.angles.r_elbow_yaw =      position[3][2]
            move.angles.r_elbow_roll =     position[3][3]

            # Set all stiffnesses, since this command specifies them all
            move.stiffness.head_yaw =         position[6][0]
            move.stiffness.head_pitch =       position[6][1]

            move.stiffness.l_shoulder_pitch = position[6][2]
            move.stiffness.l_shoulder_roll =  position[6][3]
            move.stiffness.l_elbow_yaw =      position[6][4]
            move.stiffness.l_elbow_roll =     position[6][5]

            move.stiffness.r_shoulder_pitch = position[6][18]
            move.stiffness.r_shoulder_roll =  position[6][19]
            move.stiffness.r_elbow_yaw =      position[6][20]
            move.stiffness.r_elbow_roll =     position[6][21]

            move.stiffness.l_hip_yaw_pitch =  position[6][6]
            move.stiffness.l_hip_roll =       position[6][7]
            move.stiffness.l_hip_pitch =      position[6][8]
            move.stiffness.l_knee_pitch =     position[6][9]
            move.stiffness.l_ankle_pitch =    position[6][10]
            move.stiffness.l_ankle_roll =     position[6][11]

            move.stiffness.r_hip_yaw_pitch =  position[6][12]
            move.stiffness.r_hip_roll =       position[6][13]
            move.stiffness.r_hip_pitch =      position[6][14]
            move.stiffness.r_knee_pitch =     position[6][15]
            move.stiffness.r_ankle_pitch =    position[6][16]
            move.stiffness.r_ankle_roll =     position[6][17]

            # Mark this message for sending
            command.timestamp = int(nav.brain.time * 1000)
            print("Sent a scripted move.")

        else:
            print("What kind of sweet ass-Move is this?")
