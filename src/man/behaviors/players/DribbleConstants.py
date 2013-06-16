from ..navigator import BrunswickSpeeds as speeds

# Ball on and off frame thresholds.
BALL_ON_THRESH = 2
BALL_OFF_THRESH = 30

# Ball distance thresholds.
BALL_CLOSE_DISTANCE = 35.0
BALL_FAR_AWAY = BALL_CLOSE_DISTANCE + 25.0
BALL_MOVED_THR = 3

# Bearings towards goal and ball thresholds.
FACING_FORWARD_DEG = 50

# Heat map (distance to something blocking open field) thresholds.
CROWDED_DIST = 300
OPEN_LANE_DIST = 100
