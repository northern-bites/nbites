"""
Constants to be used across classes in the motion system
"""

# Import all hard-coded C++ constants from extension module
#from _motion import (WALK_STRAIGHT,
#                     WALK_SIDEWAYS,
#                     WALK_TURN,
#                     WALK_ARC,
#                     DEFAULT_SAMPLES_PER_STEP,
#                    )


#currently, there are 40 ms time steps
TIME_STEP = 40.0 #milliseconds per frames
SECONDS_PER_FRAME = TIME_STEP/1000.

CYCLES_PER_STEP = 30

# interpolation types
LINEAR_INTERPOLATION = 0
SMOOTH_INTERPOLATION = 1


#
WALK_FOREVER = None

# Convert our units for localization
MM_TO_CM = .10

###
### Chain Constants
###
#import Kinematics
#Chain order is clockwise about nao's x axis:
#HEAD, L_ARM, L_LEG, R_LEG, R_ARM
NUM_JOINT_CHAINS = 5
#dictionary for joint chains
JOINT_CHAIN_DICT = dict(zip(range(NUM_JOINT_CHAINS),\
                      ("HEAD","LEFT_ARM","LEFT_LEG",\
                       "RIGHT_LEG","RIGHT_ARM")))

(HEAD_CHAIN,LEFT_ARM_CHAIN,LEFT_LEG_CHAIN,RIGHT_LEG_CHAIN,RIGHT_ARM_CHAIN) =\
    range(NUM_JOINT_CHAINS)

NUM_JOINT_CHAINS = 5

KINEMATICS_SUCCESS = 0
KINEMATICS_STUCK = 1

###
### Joints
###

NUM_JOINTS = 22

#keep the number of joints in each chain in a list: order must be the same as above!
NUM_HEAD_JOINTS = 2
NUM_ARM_JOINTS = 4
NUM_LEG_JOINTS = 6
NUM_CHAIN_JOINTS = (NUM_HEAD_JOINTS,NUM_ARM_JOINTS,NUM_LEG_JOINTS,NUM_LEG_JOINTS,NUM_ARM_JOINTS)


### Joint Name constants ###
(HeadYaw,
 HeadPitch,
 #LArm,
 LShoulderPitch,
 LShoulderRoll,
 LElbowYaw,
 LElbowRoll,
 #LLeg,
 LHipYawPitch,
 LHipRoll,
 LHipPitch,
 LKneePitch,
 LAnklePitch,
 LAnkleRoll,
 #RLeg,
 RHipYawPitch,
 RHipRoll,
 RHipPitch,
 RKneePitch,
 RAnklePitch,
 RAnkleRoll,
 #RArm,
 RShoulderPitch,
 RShoulderRoll,
 RElbowYaw,
 RElbowRoll) = range(NUM_JOINTS)

"""
HOLD_IN_PLACE is a constant to identify whether a particular command is
meant to just hold the position of a chain constant for some time.
Basically we want to be able to fill time with nothing, so that a
command can be executed after that time.
"""
HOLD_IN_PLACE = 1
(JOINTS_INDEX, TIME_INDEX) = range(2)


#Masks

FREE = True

FIXED = False

FIXED_ANKLE_MASK = (FREE,FREE,FREE,FREE,FIXED,FIXED)

FIXED_ANKLE_HYP_MASK = (FIXED,FREE,FREE,FREE,FIXED,FIXED)

FREE_LEG_PITCH_MASK = (FIXED,FREE,FIXED,FREE,FIXED,FIXED)

NUM_STEP_TYPES = 3
(START,FULL,END) = range(NUM_STEP_TYPES)

# A few of the robot's dimensions that are usually defined in C++, but we need
# them in python. Some day I might add them as attributes of the C++ module.
HIP_OFFSET_Y = 50.
HIP_OFFSET_Z = 80.

#maximum speeds
M_TYPE_1_1 = 188.67 * SECONDS_PER_FRAME #degrees /second -> deg/frame
M_TYPE_1_2 = 290.25 * SECONDS_PER_FRAME #degrees /second
M_TYPE_2_1 = 351.77 * SECONDS_PER_FRAME #degrees /second #100.00 * SECONDS_PER_FRAME
M_TYPE_2_2 = 305.16 * SECONDS_PER_FRAME #degrees /second #100.00 * SECONDS_PER_FRAME



MAX_ANGLE_SPEED = (
    #head
    M_TYPE_2_2,
    M_TYPE_2_1,
    #left arm
    M_TYPE_2_1,
    M_TYPE_2_2,
    M_TYPE_2_1,
    M_TYPE_2_2,
    #left leg
    M_TYPE_1_1,
    M_TYPE_1_1,
    M_TYPE_1_2,
    M_TYPE_1_2,
    M_TYPE_1_2,
    M_TYPE_1_1,
    #right leg
    M_TYPE_1_1,
    M_TYPE_1_1,
    M_TYPE_1_2,
    M_TYPE_1_2,
    M_TYPE_1_2,
    M_TYPE_1_1,
    #right arm
    M_TYPE_2_1,
    M_TYPE_2_2,
    M_TYPE_2_1,
    M_TYPE_2_2)

#in degrees
#ANGLE_LIMITS = ((-120.0,120.0),
#                (-45.0,45.0),
#                (-120.0,120.0),
#                (0.0,95.0),
#                (-120.0,120.0),
#                (-90.0,0.0),
#                (-90.0,0.0),
#                (-100.0,25.0),
#                (-25.0,45.0),
#                (0.0,130.0),
#                (-75.0,45.0),
#                (-45.0,25.0),
#                (-90.0,0.0),
#                (-100.0,25.0),
#                (-45.0,25.0),
#                (0.0,130.0),
#                (-75.0,45.0),
#                (-25.0,45.0),
#                (-120.0,120.0),
#                (-95.0,0.0),
#                (-120.0,120.0),
#                (0.0,90.0))


# FSR position constants for the sim.
# in mm (note in meters on the AL doc site): (x,y)
#relative to the 'frame' of the foot
#Note the order presented is L-FL, L-FR, L-BL, L-BR
# R-FL, R-FR, R-BL, R-BR

LEFT_FSR_POS = (( 69.93,  29.98),
                ( 69.91, -23.17),
                (-30.02, -19.11),
                (-30.62,  29.96))

RIGHT_FSR_POS = (( 69.91,  23.17),
                 ( 69.93, -29.98),
                 (-30.62, -29.96),
                 (-30.02,  19.11))

FSR_POS = (LEFT_FSR_POS,RIGHT_FSR_POS)

WALK_STRAIGHT_CONFIGS = ((.04,.015,.04,.1,.018,.025),(5.85,-5.85,0.19,5.0))

WALK_TURN_BIG_CONFIGS = ((.04,.015,.04,.4,.018,.025),(5.85,-5.85,0.19,5.0))

WALK_TURN_SMALL_CONFIGS = ((.04,.015,.04,.1,.018,.025),(5.85,-5.85,0.19,5.0))

WALK_SIDEWAYS_CONFIGS = ((.02,.015,.02,.3,.018,0.015),(4.2,-4.2,0.19,5.0))
