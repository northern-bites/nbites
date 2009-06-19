SUPER_SAFE_KICKS = False # Only kick straight when we see the goal
MAX_FORWARD_KICK_ANGLE = 55
MIN_SIDEWASE_KICK_ANGLE = 15
DEBUG_KICKS = True
ALIGN_FOR_KICK = True

KICK_STRAIGHT_BEARING_THRESH = 20
ALIGN_FOR_KICK_BEARING_THRESH = 60
ALIGN_FOR_KICK_GAIN = 2.0

(LEFT_FOOT,
 RIGHT_FOOT,
 MID_LEFT,
 MID_RIGHT,
 INCORRECT_POS ) = range(5)

LEFT_FOOT_L_Y = 10
LEFT_FOOT_R_Y = 3
RIGHT_FOOT_L_Y = -LEFT_FOOT_R_Y
RIGHT_FOOT_R_Y = -LEFT_FOOT_L_Y

MAX_KICK_X = 15
MIN_KICK_X = 0

ACROSS_GOAL_BEARING_THRESH = 60
OUT_OF_GOAL_BEARING_THRESH = -10

# Kick objectives
NUM_OBJECTIVES = 4
(OBJECTIVE_CLEAR,
 OBJECTIVE_SHOOT,
 OBJECTIVE_CENTER,
 OBJECTIVE_UNCLEAR) = range(NUM_OBJECTIVES)

