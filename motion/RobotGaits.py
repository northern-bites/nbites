import man.motion as motion
import math.pi

TO_RAD = pi/180.
CM_TO_MM = 10.

STANCE_CONFIG = (31.00, # CoM height
                 1.45,  # Forward displacement of CoM
                 10.0,  # Horizontal distance between feet
                 3.0*TO_RAD,   # Body angle around y axis
                 0.0)   # Angle between feet

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
                 0.0)   # angle XY sensor scale

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
NEW_GAIT = motion.WalkParameters(STANCE_CONFIG,
            STEP_CONFIG,
            ZMP_CONFIG,
            JOINT_HACK_CONFIG,
            SENSOR_CONFIG,
            STIFFNESS_CONFIG,
            ODO_CONFIG,
            ARM_CONFIG)


WEBOTS_GAIT = motion.GaitCommand(31.00, # com height
                                 1.40,  # hip offset x
                                 0.0,  # x-axis angle offset
                                 0.5,  # step duration
                                 0.2,   # fraction in double support
                                 1.5,  # stepHeight
                                 0.0,   # footLengthX
                                 0.3,   # zmp static percentage
                                 0,   # left swing hip roll addition
                                 0,   # right swing hip roll addition
                                 0.20,  # left zmp off
                                 0.20,  # right zmp off
                                 7.5,   # turn zmp offset
                                 0.01,  # strafe zmp offset
                                 8.0,  # max x speed
                                 4.0,   # max y speed
                                 30.0,   # max theta speed()
                                 0.0,   #sensorScale
                                 0.85,  #maxStiffness
                                 0.5,  #kneeStiffness
                                 0.2,  #anklePitchStiffness
                                 0.2,  #ankleRollStiffness
                                 0.2,   #armStiffness
                                      1.0,   #xOdoScale
                                      1.0,   #yOdoScale
                                      1.0,   #thetaOdoScale
                                      0.0)   #armAmplitude (degs)

SHORT_FAST_GAIT = motion.GaitCommand(31.00,# com height
                                      1.45, # hip offset x
                                      3.0,  # x-axis angle offset
                                      0.4, # step duration
                                      0.25,  # fraction in double support
                                      0.9,  # stepHeight
                                      0.0,  # footLengthX
                                      0.4,  # zmp static percentage
                                      5.5, # left swing hip roll addition
                                      5.5, # right swing hip roll addition
                                      0.4,  # left zmp off
                                      0.4,  # right zmp off
                                      6.6,   # turn zmp offset
                                      0.01,  # strafe zmp offset
                                      7.0,  # max x speed
                                      7.0,  # max y speed
                                      20.0,  # max theta speed()
                                      0.0,   #sensorScale
                                      0.85,  #maxStiffness
                                      0.3,  #kneeStiffness
                                      0.4,  #anklePitchStiffness
                                      0.3,  #ankleRollStiffness
                                      0.2,   #armStiffness
                                      1.0,   #xOdoScale
                                      1.0,   #yOdoScale
                                      1.0,   #thetaOdoScale
                                      0.0)   #armAmplitude (degs)
#DEFAULT GAIT ASSIGNMENTS

TRILLIAN_GAIT = SHORT_FAST_GAIT
ZAPHOD_GAIT   =   SHORT_FAST_GAIT
SLARTI_GAIT   =   SHORT_FAST_GAIT
MARVIN_GAIT   =   SHORT_FAST_GAIT

TRILLIAN_TURN_GAIT = SHORT_FAST_GAIT
ZAPHOD_TURN_GAIT   =   SHORT_FAST_GAIT
SLARTI_TURN_GAIT   =   SHORT_FAST_GAIT
MARVIN_TURN_GAIT   =   SHORT_FAST_GAIT


