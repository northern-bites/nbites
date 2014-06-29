from ..navigator import BrunswickSpeeds as speeds

DEBUG_ORBIT = False

# Transitions' Constants
# Ball on and off frame thresholds
BALL_ON_THRESH = 1
# TODO was at 60
BALL_OFF_THRESH = 10
BALL_OFF_KICK_THRESH = 4 # 4 seconds, time threshold!

# Should position for kick
BALL_PFK_LEFT_Y = 16.0
PREPARE_FOR_KICK_DIST = 50.0
SETUP_DISTANCE_X = 5
SLOW_DOWN_TO_BALL_DIST = 20.
WAIT_COUNT = 10

# Should approach ball again
APPROACH_BALL_AGAIN_DIST = PREPARE_FOR_KICK_DIST + 25.0
BALL_MOVED_THR = 3

# Should spin to ball
SHOULD_SPIN_TO_BALL_Y = 40.0
SHOULD_SPIN_TO_BALL_DIST = 350.0
SHOULD_SPIN_TO_BALL_BEAR = 15.0
STOP_SPINNING_TO_BALL_Y = 20.0
CHANGE_SPEED_THRESH = 25.0 #Degrees

# Orbit
ORBIT_BALL_DISTANCE = 25
ORBIT_DISTANCE_FAR = 7
ORBIT_DISTANCE_CLOSE = 1
ORBIT_DISTANCE_GOOD = 2
ORBIT_GOOD_BEARING = 10
SHOULD_CANCEL_ORBIT_BALL_DIST = APPROACH_BALL_AGAIN_DIST
ORBIT_TOO_LONG_THR = 8

# Should kick again
SHOULD_KICK_AGAIN_CLOSE_X = 9.5
SHOULD_KICK_AGAIN_FAR_X = 25
SHOULD_KICK_AGAIN_Y = 15

# Ball in position
## NOTE: important that these values slightly overestimate the values in
##        navigator's PFK. Otherwise there can be bad looping.
BALL_X_OFFSET = 1.5
BALL_Y_OFFSET = 1.5
GOOD_ENOUGH_H = 6

# States' constants

# Spin To Ball
FIND_BALL_SPIN_SPEED = .7
FACING_BALL_ACCEPTABLE_BEARING = 1.5

# find ball
SPUN_ONCE_TIME_THRESH = 7
WALK_FIND_BALL_TIME_THRESH = SPUN_ONCE_TIME_THRESH * 3 # Roughly 3 spins
