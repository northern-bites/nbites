import man.motion as motion
from GaitConstants import *

FAST_STANCE = (31.00, # CoM height
              1.45,  # Forward displacement of CoM
              10.0,  # Horizontal distance between feet
              3.0,   # Body angle around y axis
              0.0,   # Angle between feet
              0.1)   # Time to transition to/from this stance
FAST_STEP = (0.5, # step duration
            0.2,  # fraction in double support
            1.5,  # stepHeight
            -5.0,  # step lift
            15.0,  # max x speed
            -5.0,  # max x speed
            15.0,  # max y speed
            45.0, # max theta speed()
            7.0,  # max x accel
            7.0,  # max y accel
            20.0, # max theta speed()
            WALKING)#walk gait = true

FAST_ZMP = (0.0,  # footCenterLocX
           0.3,  # zmp static percentage
           0.45,  # left zmp off
           0.45,  # right zmp off
            0.01,  # strafe zmp offse
           6.6)   # turn zmp offset


FAST_SENSOR =  (1.0,   # Feedback type (1.0 = spring, 0.0 = old)
               0.06,  # angle X scale (gamma)
               0.08,  # angle Y scale (gamma)
               250.0,  # X spring constant k (kg/s^2)
               100.0,  # Y spring constant k (kg/s^2)
               7.0,   # max angle X (compensation)
               7.0,   # max angle Y
               45.0)   # max angle vel (change in compensation)

FAST_HACK = (6.5,6.5)

FAST_STIFFNESS = (0.85, #hipStiffness
                  0.3,  #kneeStiffness
                  0.4,  #anklePitchStiffness
                  0.3,  #ankleRollStiffness
                  0.1,  #armStiffness
                  0.5)  #arm pitch
FAST_ODO= ODO_CONFIG
FAST_ARM= (10.0,)

FAST_GAIT = motion.GaitCommand(FAST_STANCE,
                                 FAST_STEP,
                                 FAST_ZMP,
                                 FAST_HACK,
                                 FAST_SENSOR,
                                 FAST_STIFFNESS,
                                 FAST_ODO,
                                 FAST_ARM)
