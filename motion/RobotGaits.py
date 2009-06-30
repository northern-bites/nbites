import man.motion as motion

WALKING = 1.0
NON_WALKING = 0.0

STANCE_CONFIG = (31.00, # CoM height
                 1.45,  # Forward displacement of CoM
                 10.0,  # Horizontal distance between feet
                 3.0,   # Body angle around y axis
                 0.0,   # Angle between feet
                 0.1)   # Time to transition to/from this stance


MARVIN_STANCE_CONFIG = (31.00, # CoM height
                        1.45,  # Forward displacement of CoM
                        10.0,  # Horizontal distance between feet
                        6.0,   # Body angle around y axis
                        0.0,   # Angle between feet
                        0.1)   # Time to transition to/from this stance

DUCK_STANCE_CONFIG = (31.00, # CoM height
                      1.45,  # Forward displacement of CoM
                      10.0,  # Horizontal distance between feet
                      5.0,   # Body angle around y axis
                      80.0,   # Angle between feet
                      2.0)   # Time to transition to/from this stance

STEP_CONFIG = (0.4, # step duration
               0.25,  # fraction in double support
               0.9,  # stepHeight
               0.0,  # step lift
               7.0,  # max x speed
               7.0,  # max y speed
               20.0, # max theta speed()
               7.0,  # max x accel
               7.0,  # max y accel
               20.0, # max theta speed()
               WALKING)  # walking gait = true


MARVIN_STEP_CONFIG = (0.4, # step duration
               0.25,  # fraction in double support
               0.9,  # stepHeight
               7.0,  # max x speed
               7.0,  # max y speed
               20.0)  # max theta speed()

STATIONARY_STEP_CONFIG = (0.4, # step duration
               0.25,  # fraction in double support
               0.0,  # stepHeight
               0.0,  # step lift
               0.0,  # max x speed
               0.0,  # max y speed
               0.0, # max theta speed()
               0.0,  # max x acc
               0.0,  # max y acc
               0.0, # max theta acc()
               NON_WALKING) # walking gait = false



ZMP_CONFIG = (0.0,  # footCenterLocX
              0.4,  # zmp static percentage
              0.4,  # left zmp off
              0.4,  # right zmp off
              0.01,  # strafe zmp offse
              6.6)   # turn zmp offset


MARVIN_ZMP_CONFIG = (0.0,  # footCenterLocX
              0.3,  # zmp static percentage
              0.5,  # left zmp off
              0.5,  # right zmp off
              0.01,  # strafe zmp offse
              6.6)   # turn zmp offset


JOINT_HACK_CONFIG = (5.5, # left swing hip roll addition
                     5.5) # right swing hip roll addition

MARVIN_JOINT_HACK_CONFIG = (5.5, # left swing hip roll addition
                            5.5) # right swing hip roll addition

NEW_SENSOR_CONFIG = (1.0,   # Feedback type (1.0 = spring, 0.0 = old)
                     0.1,  # angle X scale (gamma)
                     0.20,  # angle Y scale (gamma)
                     150.00,  # X spring constant k (kg/s^2)
                     250.00,  # Y spring constant k (kg/s^2)
                     15.0,   # max angle X (compensation)
                     15.0,   # max angle Y
                     75.0)   # max angle vel (change in compensation)

OLD_SENSOR_CONFIG = (0.0,   # Feedback type (1.0 = spring, 0.0 = old)
                     0.5,  # angle X scale (gamma)
                     0.5,  # angle Y scale (gamma)
                     0.00,  # spring constant k (kg/s^2)
                     0.00,  # spring constant k (kg/s^2)
                     15.0,   # max angle X (compensation)
                     15.0,   # max angle Y
                     75.0)   # max angle vel (change in compensation)


MARVIN_SENSOR_CONFIG = (0.0,   # Feedback type (1.0 = spring, 0.0 = old)
                        0.5,  # angle X scale (gamma)
                        0.3,  # angle Y scale (gamma)
                        0.00,  # spring constant k (kg/s^2)
                        0.00,  # spring constant k (kg/s^2)
                        7.0,   # max angle X (compensation)
                        3.0,   # max angle Y
                        45.0)   # max angle vel (change in compensation)


CUR_SENSOR_CONFIG = OLD_SENSOR_CONFIG

STIFFNESS_CONFIG = (0.95, #hipStiffness
                    0.3,  #kneeStiffness
                    0.4,  #anklePitchStiffness
                    0.3,  #ankleRollStiffness
                    0.1)  #armStiffness

ODO_CONFIG = (1.0,   #xOdoScale
              1.0,   #yOdoScale
              1.0)   #thetaOdoScale

ARM_CONFIG = (0.0,)   #armAmplitude (degs)


#Put together all the parts to make a gait - ORDER MATTERS!
NEW_GAIT = motion.GaitCommand(MARVIN_STANCE_CONFIG,
            STEP_CONFIG,
            ZMP_CONFIG,
            JOINT_HACK_CONFIG,
            MARVIN_SENSOR_CONFIG,
            STIFFNESS_CONFIG,
            ODO_CONFIG,
            ARM_CONFIG)
MARVIN_NEW_GAIT = motion.GaitCommand(MARVIN_STANCE_CONFIG,
                                     MARVIN_STEP_CONFIG,
                                     MARVIN_ZMP_CONFIG,
                                     MARVIN_JOINT_HACK_CONFIG,
                                     MARVIN_SENSOR_CONFIG,
                                     STIFFNESS_CONFIG,
                                     ODO_CONFIG,
                                     ARM_CONFIG)


DUCK_GAIT = motion.GaitCommand(DUCK_STANCE_CONFIG,
            STEP_CONFIG,
            ZMP_CONFIG,
            JOINT_HACK_CONFIG,
            CUR_SENSOR_CONFIG,
            STIFFNESS_CONFIG,
            ODO_CONFIG,
            ARM_CONFIG)





FAST_STANCE_CONFIG = (31.00, # CoM height
                      1.45,  # Forward displacement of CoM
                      10.0,  # Horizontal distance between feet
                      7.0,   # Body angle around y axis
                      0.0,   # Angle between feet
                      0.1)   # Time to transition to/from this stance
FAST_STEP_CONFIG = (0.5, # step duration
                    0.2,  # fraction in double support
                    1.1,  # stepHeight
                    -6.0,  # step lift
                    20.0,  # max x speed
                    20.0,  # max y speed
                    40.0,  # max theta speed()
                    7.0,  # max x speed
                    7.0,  # max y speed
                    20.0,  # max theta speed()
                    WALKING)

FAST_ZMP_CONFIG = (0.0,  # footCenterLocX
              0.4,  # zmp static percentage
              0.5,  # left zmp off
              0.5,  # right zmp off
              0.02,  # strafe zmp offse
              6.6)   # turn zmp offset

FAST_HACK_CONFIG = (5.5, # left swing hip roll addition
                    5.5) # right swing hip roll addition


FAST_GAIT=motion.GaitCommand(FAST_STANCE_CONFIG,
                             FAST_STEP_CONFIG,
                             FAST_ZMP_CONFIG,
                             FAST_HACK_CONFIG,
                             NEW_SENSOR_CONFIG,
                             STIFFNESS_CONFIG,
                             ODO_CONFIG,
                             ARM_CONFIG)


WEBOTS_STEP_CONFIG = (0.4, # step duration
                      0.25,  # fraction in double support
                      1.1,  # stepHeight
                      0.0,  # step lift
                      25.0,  # max x speed
                      25.0,  # max y speed
                      80.0,  # max theta speed()
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

WEBOTS_GAIT=motion.GaitCommand(STANCE_CONFIG,
            WEBOTS_STEP_CONFIG,
            WEBOTS_ZMP_CONFIG,
            WEBOTS_HACK_CONFIG,
            CUR_SENSOR_CONFIG,
            STIFFNESS_CONFIG,
            ODO_CONFIG,
            ARM_CONFIG)

WEBOTS_GAIT2=motion.GaitCommand(STANCE_CONFIG,
            STATIONARY_STEP_CONFIG,
            WEBOTS_ZMP_CONFIG,
            WEBOTS_HACK_CONFIG,
            CUR_SENSOR_CONFIG,
            STIFFNESS_CONFIG,
            ODO_CONFIG,
            ARM_CONFIG)

#DEFAULT GAIT ASSIGNMENTS

TRILLIAN_GAIT = NEW_GAIT
ZAPHOD_GAIT   =   FAST_GAIT
SLARTI_GAIT   =   NEW_GAIT
MARVIN_GAIT   =   MARVIN_NEW_GAIT

TRILLIAN_TURN_GAIT = NEW_GAIT
ZAPHOD_TURN_GAIT   =   FAST_GAIT
SLARTI_TURN_GAIT   =   NEW_GAIT
MARVIN_TURN_GAIT   =   MARVIN_NEW_GAIT


