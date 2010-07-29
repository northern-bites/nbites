# gait minimum and maximum tuples - defines space we optimize over
# if gaitMins[i] = gaitMaxs[i] then the variable won't be optimized, and
# is only a placeholder
# parameters we're optimizing are marked with **

def gaitToArray(gait):
    list = []
    for section in gait:
        for value in section:
            list.append(value)
    return list

# the opposite of gaitToArray
# parses a list of the correct arity
def arrayToGaitTuple(array):
    assert len(array) == 44

    stance = tuple(array[0:6])
    step = tuple(array[6:18])
    zmp = tuple(array[18:24])
    hack = (array[25], array[25]) # joint hack is always the same L/R!
    sensor = tuple(array[26:34])
    stiffness = tuple(array[34:40])
    odo = tuple(array[40:43])
    arm = tuple(array[43:44])

    return (stance, step, zmp, hack, sensor, stiffness, odo, arm)

stanceMin = (29.00,  #** CoM height
             0.0,    #** Forward displacement of CoM
             9.0,   #** Horizontal distance between feet
             0.0,    #** Body angle around y axis
             -10.0,  #** Angle between feet
             0.2)    # Time to transition to/from this stance

stanceMax = (33.00, #** CoM height
             5.0,   #** Forward displacement of CoM
             12.0,  #** Horizontal distance between feet
             10.0,  #** Body angle around y axis
             15.0,  #** Angle between feet
             0.2)   # Time to transition to/from this stance

stepMin = (0.25,   #** step duration
           0.2,   #** fraction in double support
           1.5,   #** stepHeight
           -15.0, #** step lift
           15.0,  #** max x speed
           -5.0,  # max x speed
           15.0,  # max y speed
           30.0,  # max theta speed()
           3.0,   # **max x accel
           7.0,   # max y accel
           20.0,  # max theta speed()
           1.0)   # walk gait = true

stepMax = (0.4,   #** step duration
           0.5,   #** fraction in double support
           5.0,   #** stepHeight
           15.0,   #** step lift
           20.0,  #** max x speed
           -5.0,  # max x speed
           15.0,  # max y speed
           30.0,  # max theta speed()
           7.0,   #** max x accel
           7.0,   # max y accel
           20.0,  # max theta speed()
           1.0)   # walk gait = true

zmpMin = (0.0,   # footCenterLocX
          0.1,   # **zmp static percentage
          0.2,  # ** left zmp off
          0.2,  # ** right zmp off
          0.01,  # strafe zmp offse
          6.6)   # turn zmp offset

zmpMax = (0.0,   # footCenterLocX
          0.5,   # ** zmp static percentage
          0.8,  # ** left zmp off
          0.8,  # ** right zmp off
          0.01,  # strafe zmp offset
          6.6)   # turn zmp offset

hackMin = (5.5,   # joint hack
           5.5)   # joint hack

hackMax =  (5.5,  # joint hack
           5.5)   # joint hack (L/R should be same)

sensorMin = sensorMax = (0.0,   # Feedback type (1.0 = spring, 0.0 = old)
                         0.5,  # angle X scale (gamma)
                         0.3,  # angle Y scale (gamma)
                         0.00,  # spring constant k (kg/s^2)
                         0.00,  # spring constant k (kg/s^2)
                         7.0,   # max angle X (compensation)
                         3.0,   # max angle Y
                         45.0)   # max angle vel (change in compensation)

stiffnessMin = stiffnessMax = (0.85,  # hipStiffness
                               0.3,   # kneeStiffness
                               0.4,   # anklePitchStiffness
                               0.3,   # ankleRollStiffness
                               0.1,   # armStiffness
                               0.5)   # arm pitch

odoMin = odoMax = (1.0,   # xOdoScale
                   1.0,   # yOdoScale
                   1.0)   # thetaOdoScale

armMin = armMax = (0.0,)  # arm config

gaitMins = (stanceMin,
            stepMin,
            zmpMin,
            hackMin,
            sensorMin,
            stiffnessMin,
            odoMin,
            armMin)

gaitMaxs = (stanceMax,
            stepMax,
            zmpMax,
            hackMax,
            sensorMax,
            stiffnessMax,
            odoMax,
            armMax)

