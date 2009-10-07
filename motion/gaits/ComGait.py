import man.motion as motion
from GaitConstants import *

########## COM GAIT #################
COM_STANCE = (31.00, # CoM height
              0.0,  # Forward displacement of CoM
              10.0,  # Horizontal distance between feet
              3.0,   # Body angle around y axis
              0.0,   # Angle between feet
              0.1)   # Time to transition to/from this stance


COM_STEP = (0.5, # step duration
            0.2,  # fraction in double support
            2.0,  # stepHeight
            0.0,  # step lift
            20.0,  # max x speed
            -4.0,  # max x speed
            10.0,  # max y speed
            30.0, # max theta speed()
            5.0,  # max x accel
            5.0,  # max y accel
            20.0, # max theta speed()
            WALKING)#walk gait = true


COM_ZMP = (2.0,  # footCenterLocX
           0.2,  # zmp static percentage
           0.1,  # left zmp off
           0.1,  # right zmp off
           0.01,  # strafe zmp offse
           7.6)   # turn zmp offset


COM_SENSOR =  (1.0,   # Feedback type (1.0 = spring, 0.0 = old)
               0.00,  # angle X scale (gamma)
               0.00,  # angle Y scale (gamma)
               100.0,  # X spring constant k (kg/s^2)
               100.0,  # Y spring constant k (kg/s^2)
               7.0,   # max angle X (compensation)
               7.0,   # max angle Y
               45.0)   # max angle vel (change in compensation)

COM_HACK = (5.5,5.5,)#JOINT_HACK_CONFIG
COM_STIFFNESS = (0.85, #hipStiffness
                 0.6,  #kneeStiffness
                 0.6,  #anklePitchStiffness
                 0.6,  #ankleRollStiffness
                 0.1,  #armStiffness
                 0.5)  #arm pitch
COM_ODO=ODO_CONFIG
COM_ARM=ARM_CONFIG

# COM_GAIT = motion.GaitCommand(COM_STANCE,
#                               COM_STEP,
#                               COM_ZMP,
#                               COM_HACK,
#                               COM_SENSOR,
#                               COM_STIFFNESS,
#                               COM_ODO,
#                               COM_ARM)


