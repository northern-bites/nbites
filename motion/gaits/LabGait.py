import man.motion as motion
from GaitConstants import *

LAB_GAIT = motion.GaitCommand(LAB_STANCE_CONFIG,
                              LAB_STEP_CONFIG,
                              ZMP_CONFIG,
                              JOINT_HACK_CONFIG,
                              MARVIN_SENSOR_CONFIG,
                              STIFFNESS_CONFIG,
                              ODO_CONFIG,
                              ARM_CONFIG)

LAB_STANCE_CONFIG = (31.00, # CoM height
                     1.1,  # Forward displacement of CoM
                     10.0,  # Horizontal distance between feet
                     3.0,   # Body angle around y axis
                     0.0,   # Angle between feet
                     0.1)   # Time to transition to/from this stance

LAB_STEP_CONFIG = (0.5, # step duration
                   0.5,  # fraction in double support
                   1.6,  # stepHeight
                   0.0,  # step lift
                   7.0,  # max x speed
                   -7.0,  # max x speed
                   7.0,  # max y speed
                   20.0, # max theta speed()
                   7.0,  # max x accel
                   7.0,  # max y accel
                   20.0, # max theta speed()
                   WALKING)  # walking gait = true

ZMP_CONFIG = (0.0,  # footCenterLocX
              0.4,  # zmp static percentage
              0.4,  # left zmp off
              0.4,  # right zmp off
              0.01,  # strafe zmp offse
              6.6)   # turn zmp offset

JOINT_HACK_CONFIG = (5.5, # left swing hip roll addition
                     5.5) # right swing hip roll addition

MARVIN_SENSOR_CONFIG = (0.0,   # Feedback type (1.0 = spring, 0.0 = old)
                        0.5,  # angle X scale (gamma)
                        0.3,  # angle Y scale (gamma)
                        0.00,  # spring constant k (kg/s^2)
                        0.00,  # spring constant k (kg/s^2)
                        7.0,   # max angle X (compensation)
                        3.0,   # max angle Y
                        45.0)   # max angle vel (change in compensation)

STIFFNESS_CONFIG = (0.85, #hipStiffness
                    0.3,  #kneeStiffness
                    0.4,  #anklePitchStiffness
                    0.3,  #ankleRollStiffness
                    0.1,  #armStiffness
                    0.1)  #arm pitch
