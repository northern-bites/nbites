from ..navigator import BrunswickSpeeds as speeds

# Transitions' Constants
# Ball on and off frame thresholds
BALL_ON_THRESH = 2
BALL_OFF_THRESH = 60
BALL_OFF_KICK_THRESH = 120

# Should position for kick
BALL_PFK_LEFT_Y = 16.0
PREPARE_FOR_KICK_DIST = 35.0

# Should approach ball again
APPROACH_BALL_AGAIN_DIST = PREPARE_FOR_KICK_DIST + 25.0

# Should spin to ball
SHOULD_SPIN_TO_BALL_Y = 30.0
SHOULD_SPIN_TO_BALL_B = 14.0
STOP_SPINNING_TO_BALL_Y = 15.0
STOP_SPINNING_TO_BALL_B = 7.0

# Should kick again
SHOULD_KICK_AGAIN_CLOSE_X = 9.5
SHOULD_KICK_AGAIN_FAR_X = 16
SHOULD_KICK_AGAIN_Y = 9

# Ball in position
## NOTE: important that these values slightly overestimate the values in
##        navigator's PFK. Otherwise there can be bad looping.
BALL_X_OFFSET = 2
BALL_Y_OFFSET = 2
GOOD_ENOUGH_H = 6

# Should dribble should and should stop dribbling
SHOULD_DRIBBLE_X = 68.
SHOULD_DRIBBLE_Y = BALL_PFK_LEFT_Y
STOP_DRIBBLE_X = SHOULD_DRIBBLE_X + 20
STOP_DRIBBLE_Y = SHOULD_DRIBBLE_Y + 20
STOP_DRIBBLE_BEARING = 40.0

# States' constants
# turnToBall
FIND_BALL_SPIN_SPEED = .7

# find ball
WALK_FIND_BALL_FRAMES_THRESH = 720

STOP_PENALTY_DRIBBLE_COUNT = 120
