import man.motion as motion

STANCE_CONFIG = (31.00, # CoM height
                 1.45,  # Forward displacement of CoM
                 10.0,  # Horizontal distance between feet
                 3.0,   # Body angle around y axis
                 0.0,   # Angle between feet
                 0.1)   # Time to transition to/from this stance


DUCK_STANCE_CONFIG = (31.00, # CoM height
                      1.45,  # Forward displacement of CoM
                      10.0,  # Horizontal distance between feet
                      0.0,   # Body angle around y axis
                      30.0,   # Angle between feet
                      1.0)   # Time to transition to/from this stance

STEP_CONFIG = (0.4, # step duration
               0.25,  # fraction in double support
               0.9,  # stepHeight
               7.0,  # max x speed
               7.0,  # max y speed
               20.0)  # max theta speed()



ZMP_CONFIG = (0.0,  # footCenterLocX
              0.4,  # zmp static percentage
              0.4,  # left zmp off
              0.4,  # right zmp off
              0.01,  # strafe zmp offse
              6.6)   # turn zmp offset



JOINT_HACK_CONFIG = (5.5, # left swing hip roll addition
                     5.5) # right swing hip roll addition

SENSOR_CONFIG = (0.0,   # observer sensor scale
                 0.50,  # angle XY sensor scale
                 15.0,   # max angle X (compensation)
                 15.0,   # max angle Y
                 75.0)   # max angle vel (change in compensation)
STIFFNESS_CONFIG = (0.85, #hipStiffness
                    0.3,  #kneeStiffness
                    0.4,  #anklePitchStiffness
                    0.3,  #ankleRollStiffness
                    0.2)  #armStiffness

ODO_CONFIG = (1.0,   #xOdoScale
              1.0,   #yOdoScale
              1.0)   #thetaOdoScale

ARM_CONFIG = (0.0,)   #armAmplitude (degs)


#Put together all the parts to make a gait - ORDER MATTERS!
NEW_GAIT = motion.GaitCommand(STANCE_CONFIG,
            STEP_CONFIG,
            ZMP_CONFIG,
            JOINT_HACK_CONFIG,
            SENSOR_CONFIG,
            STIFFNESS_CONFIG,
            ODO_CONFIG,
            ARM_CONFIG)


DUCK_GAIT = motion.GaitCommand(DUCK_STANCE_CONFIG,
            STEP_CONFIG,
            ZMP_CONFIG,
            JOINT_HACK_CONFIG,
            SENSOR_CONFIG,
            STIFFNESS_CONFIG,
            ODO_CONFIG,
            ARM_CONFIG)





FAST_STANCE_CONFIG = (31.00, # CoM height
                      1.45,  # Forward displacement of CoM
                      10.0,  # Horizontal distance between feet
                      5.0,   # Body angle around y axis
                      0.0,   # Angle between feet
                      0.1)   # Time to transition to/from this stance
FAST_STEP_CONFIG = (0.4, # step duration
               0.25,  # fraction in double support
               0.9,  # stepHeight
               10.0,  # max x speed
               10.0,  # max y speed
               20.0)  # max theta speed()

FAST_ZMP_CONFIG = (0.0,  # footCenterLocX
              0.4,  # zmp static percentage
              0.5,  # left zmp off
              0.5,  # right zmp off
              0.01,  # strafe zmp offse
              6.6)   # turn zmp offset

FAST_HACK_CONFIG = (6.5, # left swing hip roll addition
                    6.5) # right swing hip roll addition


FAST_GAIT=motion.GaitCommand(FAST_STANCE_CONFIG,
                             FAST_STEP_CONFIG,
                             FAST_ZMP_CONFIG,
                             FAST_HACK_CONFIG,
                             SENSOR_CONFIG,
                             STIFFNESS_CONFIG,
                             ODO_CONFIG,
                             ARM_CONFIG)



WEBOTS_ZMP_CONFIG =(0.0,  # footCenterLocX
                    0.4,  # zmp static percentage
                    0.0,  # left zmp off
                    0.0,  # right zmp off
                    0.01,  # strafe zmp offset (no units)
                    6.6)   # turn zmp off    ""
WEBOTS_HACK_CONFIG = (0.0, # left swing hip roll addition
                      0.0) # right swing hip roll addition

WEBOTS_GAIT=motion.GaitCommand(STANCE_CONFIG,
            STEP_CONFIG,
            WEBOTS_ZMP_CONFIG,
            WEBOTS_HACK_CONFIG,
            SENSOR_CONFIG,
            STIFFNESS_CONFIG,
            ODO_CONFIG,
            ARM_CONFIG)

WEBOTS_GAIT2=motion.GaitCommand(DUCK_STANCE_CONFIG,
            STEP_CONFIG,
            WEBOTS_ZMP_CONFIG,
            WEBOTS_HACK_CONFIG,
            SENSOR_CONFIG,
            STIFFNESS_CONFIG,
            ODO_CONFIG,
            ARM_CONFIG)

#DEFAULT GAIT ASSIGNMENTS

TRILLIAN_GAIT = NEW_GAIT
ZAPHOD_GAIT   =   FAST_GAIT
SLARTI_GAIT   =   NEW_GAIT
MARVIN_GAIT   =   NEW_GAIT

TRILLIAN_TURN_GAIT = NEW_GAIT
ZAPHOD_TURN_GAIT   =   FAST_GAIT
SLARTI_TURN_GAIT   =   NEW_GAIT
MARVIN_TURN_GAIT   =   NEW_GAIT


