from ..navigator import BrunswickSpeeds as speeds

# Ball on and off frame thresholds.
BALL_ON_THRESH = 2
BALL_OFF_THRESH = 30

# Ball distance thresholds.
BALL_CLOSE_DISTANCE = 35.0
BALL_FAR_AWAY = BALL_CLOSE_DISTANCE + 25.0
BALL_TOO_FAR_TO_SIDE = 8
BALL_MOVED_THR = 3

# Bearings towards goal and ball thresholds.
FACING_FORWARD_DEG = 50
FACING_GOAL_ON_WING = 10

# Heat map (distance to something blocking open field) thresholds.
OPEN_LANE_DIST = 100

# Distances and setups.
BACKUP_WHEN_LOST = -30
SETUP_POSITION = -15

# Time.
ROTATE_FC = 20
ENOUGH_TIME_FOR_NORMAL_BEHAVIOR = 30
SWITCH_TO_DRIBBLE_IF_IN_GOALBOX = 15
