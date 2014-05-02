# TODO parameters to the kickDecider?
# Controls the exponential function used to score kicking accuracy
ALPHA = 1
# Controls the exponential function used to score kicking speed
BETA = 1

# Controls how we weight the cost function that we minimize to balance
# kicking accuracy with kicking speed
GAMMA = 0  # Higher gamma -> kick accuracy is more important  
DELTA = -1 # Lower delta -> speed of setting up for kick is more important

# How finely should we search over possible kick setups?
# NOTE NUM_OF_SAMPLES should be an odd number, so that a perfectly-aimed kick 
#      is chosen as one of the sampled kicks, see sampleKickSetups for more info
NUM_OF_SAMPLES = 5

# What is the highest error (in cm) in y dir that we tolerate while kicking?
SHOT_PRECISION = 30
PASS_PRECISION = 100

# When is an attempted kick too difficult to perform? When is the demand for
# precision greater than we can supply?
MIN_PRECISION = 15

# How forward in the x direction (in cm) does a kick need to go to be acceptable?
FORWARD_ENOUGH_THRES = 20
