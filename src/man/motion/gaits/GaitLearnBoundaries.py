# gait minimum and maximum tuples - defines space we optimize over
# if gaitMins[i] = gaitMaxs[i] then the variable won't be optimized, and
# is only a placeholder
# parameters we're optimizing are marked with **
#
# NOTE: the optimizer will sometimes stray slightly out of these bounds if it
# thinks that we've got them wrong, so don't sweat the exact values here. It's more
# important to set which parameters we're optimizing on.

stanceMin = (29.00,  #** CoM height
             -1.0,    #** Forward displacement of CoM
             11.0,    #** Horizontal distance between feet
             0.0,    #** Body angle around y axis
             0.0,    # Angle between feet
             0.2)    # Time to transition to/from this stance

stanceMax = (32.00, #** CoM height
             3.0,   #** Forward displacement of CoM
             12.0,  #** Horizontal distance between feet
             8.0,  #** Body angle around y axis
             0.0,   # Angle between feet
             0.2)   # Time to transition to/from this stance

stepMin = (0.3,   #** step duration
           0.2,   #** fraction in double support
           1.0,   #** stepHeight
           -15.0, #** step lift
           15.0,  # max x speed
           -10.0,  # max x speed
           10.0,  #** max y speed
           30.0,  # max theta speed()
           4.0,   # max x accel
           4.0,   # max y accel
           20.0,  #** max theta speed()
           1.0)   # walk gait = true

stepMax = (0.5,   #** step duration
           0.5,   #** fraction in double support
           5.0,   #** stepHeight
           15.0,   #** step lift
           15.0,  # max x speed
           -10.0,  # max x speed
           15.0,  # max y speed
           30.0,  # max theta speed()
           4.0,   # max x accel
           4.0,   # max y accel
           40.0,  #** max theta speed()
           1.0)   # walk gait = true

zmpMin = (0.0,   # **footCenterLocX
          0.1,   # **zmp static percentage
          -0.5,   # **left zmp off ## like with hip hack, these are optimized in tandem
          0.4,   # right zmp off
          0.01,  # strafe zmp offse
          6.6)   # turn zmp offset

zmpMax = (3.0,   #** footCenterLocX
          0.5,   # ** zmp static percentage
          1.0,   # **left zmp off
          0.4,   # right zmp off
          0.01,  # strafe zmp offset
          6.6)   # turn zmp offset

# NOTE: only the second one of these gets optimized, since they have to be the same
hackMin = (4.0,   # joint hack
           3.0)   # **joint hack

hackMax =  (4.0,  # joint hack
            8.0)   # **joint hack (L/R should be same)

sensorMin = (0.0,   # Feedback type (1.0 = spring, 0.0 = old)
             0.0,  # **angle X scale (gamma)
             0.0,  # **angle Y scale (gamma)
             0.00,  # spring constant k (kg/s^2)
             0.00,  # spring constant k (kg/s^2)
             7.0,   # max angle X (compensation)
             3.0,   # max angle Y
             45.0)   # max angle vel (change in compensation)

sensorMax = (0.0,   # Feedback type (1.0 = spring, 0.0 = old)
             1.0,  # **angle X scale (gamma)
             1.0,  # **angle Y scale (gamma)
             0.00,  # spring constant k (kg/s^2)
             0.00,  # spring constant k (kg/s^2)
             7.0,   # max angle X (compensation)
             3.0,   # max angle Y
             45.0)   # max angle vel (change in compensation)

stiffnessMin  = (0.7,  #** hipStiffness
                 0.3,   #** kneeStiffness
                 0.4,   #**anklePitchStiffness
                 0.3,   #** ankleRollStiffness
                 0.4,   # armStiffness
                 0.4)   # arm pitch

stiffnessMax = (0.9,  #**hipStiffness
                0.9,  #**kneeStiffness
                0.9,  #**anklePitchStiffness
                0.9,  #**ankleRollStiffness
                0.4,  #armStiffness
                0.4)  #arm pitch

odoMin = odoMax = (1.0,   # xOdoScale
                   1.0,   # yOdoScale
                   1.0)   # thetaOdoScale

armMin = (0.0,) # * arm config

armMax = (15.0,)

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

