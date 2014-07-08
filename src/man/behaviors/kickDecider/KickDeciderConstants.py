# Should we kick directly at our target ignoring SHOT_PRECISON and PASS_PRECISION?
NO_SAMPLING = True

# How finely should we search over possible kick setups?
# NOTE NUM_OF_SAMPLES should be an odd number, so that a perfectly-aimed kick 
#      is chosen as one of the sampled kicks, see sampleKickSetups for more info
NUM_OF_SAMPLES = 5

# What is the highest error (in cm) in y dir that we tolerate while kicking?
SHOT_PRECISION = 30
PASS_PRECISION = 50
