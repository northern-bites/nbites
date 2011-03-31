import man.motion as motion
import GaitConstants as constants

ODO_CONFIG = constants.ODO_CONFIG
ARM_CONFIG = constants.USE_ARMS
WALKING = constants.WALKING
NON_WALKING = constants.NON_WALKING

MARVIN_STANCE_CONFIG = (31.0, # CoM height
                        1.0,  # Forward displacement of CoM
                        11.5,  # Horizontal distance between feet
                        0.0,   # Body angle around y axis
                        0.0,   # Angle between feet
                        0.1)   # Time to transition to/from this stance

STEP_CONFIG = (0.375, # step duration
               0.3,  # fraction in double support
               1.95,  # stepHeight
               -3.25,  # step lift
               18.0,  # max x speed
               -9.0,  # max x speed (backwards)
               11.0,  # max y speed
               25.0, # max theta speed()
               4.0,  # max x accel
               4.0,  # max y accel
               20.0, # max theta speed()
               WALKING)  # walking gait = true

ZMP_CONFIG = (0.0,  # footCenterLocX
              0.4,  # zmp static percentage
              0.4,  # left zmp off
              0.4,  # right zmp off
              0.01,  # strafe zmp offset
              6.6)   # turn zmp offset

JOINT_HACK_CONFIG = (6.5, # left swing hip roll addition
                     6.5) # right swing hip roll addition

MARVIN_SENSOR_CONFIG = (0.0,   # Feedback type (1.0 = spring, 0.0 = old)
                        0.5,  # angle X scale (gamma)
                        0.3,  # angle Y scale (gamma)
                        0.00,  # spring constant k (kg/s^2)
                        0.00,  # spring constant k (kg/s^2)
                        7.0,   # max angle X (compensation)
                        3.0,   # max angle Y
                        45.0)   # max angle vel (change in compensation)

STIFFNESS_CONFIG = (0.9, #hipStiffness
                    0.8,  #kneeStiffness
                    0.6,  #anklePitchStiffness
                    0.7,  #ankleRollStiffness
                    0.4,  #armStiffness
                    0.4)  #arm pitch

#Put together all the parts to make a gait - ORDER MATTERS!
ZMP_GAIT = motion.GaitCommand(MARVIN_STANCE_CONFIG,
                              STEP_CONFIG,
                              ZMP_CONFIG,
                              JOINT_HACK_CONFIG,
                              MARVIN_SENSOR_CONFIG,
                              STIFFNESS_CONFIG,
                              ODO_CONFIG,
                              ARM_CONFIG)
