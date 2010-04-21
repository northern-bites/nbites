# gait constants that were left in RobotGaits.py
# after it was split into gaits/*Gait.py files
# keeping them around in case they're important

DUCK_STEP = (0.4, # step duration
             0.25,  # fraction in double support
             1.3,  # stepHeight
             -5.0,  # step lift
             7.0,  # max x speed
             -7.0,  # min x speed
             7.0,  # max y speed
             5.0, # max theta speed()
             7.0,  # max x accel
             7.0,  # max y accel
             20.0, # max theta speed()
             WALKING)  # walking gait = true

DUCK_ZMP = (0.0,  # footCenterLocX
            0.4,  # zmp static percentage
            0.4,  # left zmp off
            0.4,  # right zmp off
            0.01,  # strafe zmp offse
            6.6)   # turn zmp offset

OLD_SENSOR_CONFIG = (0.0,   # Feedback type (1.0 = spring, 0.0 = old)
                     0.5,  # angle X scale (gamma)
                     0.5,  # angle Y scale (gamma)
                     0.00,  # spring constant k (kg/s^2)
                     0.00,  # spring constant k (kg/s^2)
                     15.0,   # max angle X (compensation)
                     15.0,   # max angle Y
                     75.0)   # max angle vel (change in compensation)

NEW_SENSOR_CONFIG = (1.0,   # Feedback type (1.0 = spring, 0.0 = old)
                     0.1,  # angle X scale (gamma)
                     0.20,  # angle Y scale (gamma)
                     150.00,  # X spring constant k (kg/s^2)
                     250.00,  # Y spring constant k (kg/s^2)
                     15.0,   # max angle X (compensation)
                     15.0,   # max angle Y
                     75.0)   # max angle vel (change in compensation)


MARVIN_JOINT_HACK_CONFIG = (5.5, # left swing hip roll addition
                            5.5) # right swing hip roll addition


MARVIN_STEP_CONFIG = (0.4, # step duration
               0.25,  # fraction in double support
               0.9,  # stepHeight
               0.0,  # step lift
               7.0,  # max x speed
               -7.0,  # max x speed
               7.0,  # max y speed
               20.0, # max theta speed()
               7.0,  # max x accel
               7.0,  # max y accel
               20.0, # max theta speed()
               20.0,  # max theta speed()
                      WALKING)#walk gait = true
