from ..navigator import BrunswickSpeeds as speeds

# Ball on and off frame thresholds.
BALL_ON_THRESH = 2
BALL_OFF_THRESH = 60

# Ball distance thresholds.
BALL_CLOSE_DISTANCE = 35.0
BALL_FAR_AWAY = BALL_CLOSE_DISTANCE + 25.0
BALL_MOVED_THR = 3

# Bearings towards goal and ball thresholds.
FACING_FORWARD_DEG = 35

# Heat map thresholds.
CROWDED_DIST = 50
OPEN_LANE_DIST = 20

# Spinning to the ball.
# SHOULD_SPIN_TO_BALL_Y = 40.0
# SHOULD_SPIN_TO_BALL_DIST = 350.0
# SHOULD_SPIN_TO_BALL_BEAR = 15.0
# STOP_SPINNING_TO_BALL_Y = 20.0
