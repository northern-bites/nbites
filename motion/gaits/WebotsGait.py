import man.motion as motion
from GaitConstants import *

STANCE_CONFIG = (31.00, # CoM height
                 1.45,  # Forward displacement of CoM
                 10.0,  # Horizontal distance between feet
                 3.0,   # Body angle around y axis
                 0.0,   # Angle between feet
                 0.1)   # Time to transition to/from this stance

STATIONARY_STEP_CONFIG = (0.4, # step duration
               0.25,  # fraction in double support
               0.0,  # stepHeight
               0.0,  # step lift
               0.0,  # max x speed
               0.0,  # max x speed
               0.0,  # max y speed
               0.0, # max theta speed()
               0.0,  # max x acc
               0.0,  # max y acc
               0.0, # max theta acc()
               NON_WALKING) # walking gait = false

WEBOTS_STEP_CONFIG = (0.4, # step duration
                      0.25,  # fraction in double support
                      1.1,  # stepHeight
                      0.0,  # step lift
                      10.0,  # max x speed
                      -6.0,  # max x speed
                      10.0,  # max y speed
                      30.0,  # max theta speed()
                      5.0,  # max x acc
                      5.0,  # max y acc
                      20.0,  # max theta acc()
                      WALKING)

WEBOTS_ZMP_CONFIG =(0.0,  # footCenterLocX
                    0.4,  # zmp static percentage
                    0.0,  # left zmp off
                    0.0,  # right zmp off
                    0.01,  # strafe zmp offset (no units)
                    6.6)   # turn zmp off    ""
WEBOTS_HACK_CONFIG = (0.0, # left swing hip roll addition
                      0.0) # right swing hip roll addition

SENSOR_CONFIG = (0.0,   # Feedback type (1.0 = spring, 0.0 = old)
                     0.5,  # angle X scale (gamma)
                     0.5,  # angle Y scale (gamma)
                     0.00,  # spring constant k (kg/s^2)
                     0.00,  # spring constant k (kg/s^2)
                     15.0,   # max angle X (compensation)
                     15.0,   # max angle Y
                     75.0)   # max angle vel (change in compensation)

STIFFNESS_CONFIG = (0.85, #hipStiffness
                    0.3,  #kneeStiffness
                    0.4,  #anklePitchStiffness
                    0.3,  #ankleRollStiffness
                    0.1,  #armStiffness
                    0.1)  #arm pitch

WEBOTS_GAIT=motion.GaitCommand(STANCE_CONFIG,
            WEBOTS_STEP_CONFIG,
            WEBOTS_ZMP_CONFIG,
            WEBOTS_HACK_CONFIG,
            SENSOR_CONFIG,
            STIFFNESS_CONFIG,
            ODO_CONFIG,
            ARM_CONFIG)

WEBOTS_GAIT2=motion.GaitCommand(STANCE_CONFIG,
            STATIONARY_STEP_CONFIG,
            WEBOTS_ZMP_CONFIG,
            WEBOTS_HACK_CONFIG,
            SENSOR_CONFIG,
            STIFFNESS_CONFIG,
            ODO_CONFIG,
            ARM_CONFIG)
