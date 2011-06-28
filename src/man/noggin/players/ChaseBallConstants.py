from ..navigator import BrunswickSpeeds as speeds

# Component Switches
USE_DRIBBLE = False

# Transitions' Constants
# Ball on and off frame thresholds
BALL_ON_THRESH = 2
BALL_OFF_THRESH = 30
BALL_OFF_KICK_THRESH = 80

# Should position for kick
BALL_PFK_LEFT_Y = 16.0
BALL_PFK_RIGHT_Y = -BALL_PFK_LEFT_Y
BALL_PFK_DIST = 68.

# Should claim ball
SHOULD_STOP_BEFORE_KICK_DIST = 40.

# Should spin to ball
SHOULD_SPIN_TO_KICK_DIST = SHOULD_STOP_BEFORE_KICK_DIST
SHOULD_SPIN_TO_KICK_X = 9.5

# Should kick again
SHOULD_KICK_AGAIN_CLOSE_X = SHOULD_SPIN_TO_KICK_X
SHOULD_KICK_AGAIN_FAR_X = 16
SHOULD_KICK_AGAIN_Y = 9

# Ball in position
## NOTE: important that these values slightly overestimate the values in
##        navigator's PFK. Otherwise there can be bad looping.
BALL_X_OFFSET = 5
BALL_Y_OFFSET = 5

# Should dribble should and should stop dribbling
SHOULD_DRIBBLE_X = 68.
SHOULD_DRIBBLE_Y = BALL_PFK_LEFT_Y
SHOULD_DRIBBLE_BEARING = 30.0
STOP_DRIBBLE_X = SHOULD_DRIBBLE_X + 20
STOP_DRIBBLE_Y = SHOULD_DRIBBLE_Y + 20
STOP_DRIBBLE_BEARING = 40.0

# States' constants
# turnToBall
MAX_SPIN_SCAN_PERCENT = .89
FIND_BALL_SPIN_SPEED = speeds.MAX_SPIN_MAGNITUDE*MAX_SPIN_SCAN_PERCENT
BALL_SPIN_SPEED = speeds.MAX_SPIN_MAGNITUDE

# find ball
WALK_FIND_BALL_FRAMES_THRESH = 720
SCAN_FIND_BEARING_THRESH = 50

STOP_PENALTY_DRIBBLE_COUNT = 120
