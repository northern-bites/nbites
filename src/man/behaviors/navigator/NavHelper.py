from math import fabs
from ..util import MyMath
import NavConstants as constants
from objects import RelLocation, RelRobotLocation, RobotLocation, Location
# TODO: Import CommandType properly.
#import PMotion_proto

def stand(nav):
    """
    Makes the motion engine stand
    Right now this is done by sending a (0, 0, 0) velocity vector
    TODO: make a command for stand
    """
    createAndSendWalkVector(nav, 0, 0, 0)

def getRelativeDestination(my, dest):
    """
    Takes in a dest that can be of various types and
    then returns a relative robot location to get to that location.

    If it's an absolute location, the it uses the robot location to compute
    the relative location of the point.

    If dest doesn't have a heading (Location, RelLocation), then it will
    use the bearing to the point.
    """

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
        field_dest = RobotLocation(field_dest.x, field_dest.y, 0)
        relLocation = my.relativeRobotLocationOf(field_dest)
        relLocation.relH = my.getRelativeBearing(field_dest)
        return relLocation

    elif (hasattr(field_dest, 'rel_x') and
          hasattr(field_dest, 'rel_y') and
          hasattr(field_dest, 'bearing_deg')):
        return RelRobotLocation(field_dest.rel_x,
                                field_dest.rel_y,
                                field_dest.bearing_deg)

    else:
        raise TypeError, "Navigator dest is not a Location type!" + str(dest)

def isDestinationRelative(dest):
    #TODO: test if this works for both RelLocation and RelRobotLocation
    # I think it does, but someone should try it and then note the result -O
    return isinstance(dest, RelLocation)

def adaptSpeed(distance, cutoffDistance, maxSpeed):
    return (distance / cutoffDistance) * maxSpeed
#    return MyMath.mapRange(distance, 0, cutoffDistance, 0, maxSpeed)

def setDestination(nav, dest, gain = 1.0):
    """
    Method to set the next destination walk command
    See MotionModule.h for more info on the odometry walk command
    """
    command = nav.brain.interface.bodyMotionCommand
    command.type = command.CommandType.DESTINATION_WALK #Destination Walk
    command.dest.rel_x = dest.relX
    command.dest.rel_y = dest.relY
    command.dest.rel_h = dest.relH
    # Mark this message for sending
    command.timestamp = int(nav.brain.time * 1000)

def setOdometryDestination(nav, dest, gain = 1.0):
    """
    Method to set the next walk command
    See MotionModule.h for more info on the odometry walk command
    """
    command = nav.brain.interface.bodyMotionCommand
    command.type = command.CommandType.ODOMETRY_WALK #Destination Walk
    command.odometry_dest.rel_x = dest.relX
    command.odometry_dest.rel_y = dest.relY
    command.odometry_dest.rel_h = dest.relH
    # Mark this message for sending
    command.timestamp = int(nav.brain.time * 1000)

def setSpeed(nav, speeds):
    """
    Wrapper method to easily change the walk vector of the robot
    """
    if speeds == constants.ZERO_SPEEDS:
        nav.printf("!!!!!! USE player.stopWalking() NOT walk(0,0,0)!!!!!")
        return

    createAndSendWalkVector(nav, *speeds)

def createAndSendWalkVector(nav, x, y, theta):
    """
    Method to set the next velocity walk command
    See MotionModule.h for more info on the velocity walk command
    """
    command = nav.brain.interface.bodyMotionCommand
    command.type = command.CommandType.WALK_COMMAND #Walk Command
    command.speed.x_percent = x
    command.speed.y_percent = y
    command.speed.h_percent = theta
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

        else:
            print("What kind of sweet ass-Move is this?")
