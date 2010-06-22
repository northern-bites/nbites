from .. import NogginConstants
from ..typeDefs.Location import Location

SUPER_SAFE_KICKS = False # Only kick straight when we see the goal
MAX_FORWARD_KICK_ANGLE = 55
MIN_SIDEWASE_KICK_ANGLE = 15
DEBUG_KICKS = False
ALIGN_FOR_KICK = False

ALIGN_FOR_KICK_BEARING_THRESH = 60
ALIGN_FOR_KICK_GAIN = 2.0

(LEFT_FOOT,
 RIGHT_FOOT,
 MID_LEFT,
 MID_RIGHT,
 INCORRECT_POS ) = range(5)

SIDE_STEP_MAX_SPEED = 5.0
SIDE_STEP_MIN_SPEED = -SIDE_STEP_MAX_SPEED
SIDE_STEP_MIN_MAGNITUDE = 2.0

LEFT_FOOT_CENTER_Y = 6.5
RIGHT_FOOT_CENTER_Y = -LEFT_FOOT_CENTER_Y
LEFT_FOOT_L_Y = 8
LEFT_FOOT_R_Y = 2
RIGHT_FOOT_L_Y = -LEFT_FOOT_R_Y
RIGHT_FOOT_R_Y = -LEFT_FOOT_L_Y

DEFAULT_KICK_X_OFFSET = 13

MAX_KICK_X = 22
MIN_KICK_X = 0

ACROSS_GOAL_BEARING_THRESH = 60
OUT_OF_GOAL_BEARING_THRESH = -10

# Penalty Kick
ALIGN_FOR_KICK_MIN_ANGLE = 10

LOOK_POST_KICK_FRAMES_OFF = 10

# clearBall constants
CLEAR_CENTER_FIELD_STRAIGHT_ANGLE = 60

# Kick objectives
NUM_OBJECTIVES = 5
(OBJECTIVE_CLEAR,
 OBJECTIVE_CENTER,
 OBJECTIVE_SHOOT,
 OBJECTIVE_KICKOFF,
 OBJECTIVE_UNCLEAR) = range(NUM_OBJECTIVES)

SIDELINE_CLEAR_OFFSET = 1.0
LEFT_CLEAR_POINT = Location(NogginConstants.OPP_GOALBOX_LEFT_X,
                            NogginConstants.FIELD_WHITE_BOTTOM_SIDELINE_Y +
                            SIDELINE_CLEAR_OFFSET)

RIGHT_CLEAR_POINT = Location(NogginConstants.OPP_GOALBOX_LEFT_X,
                             NogginConstants.FIELD_WHITE_TOP_SIDELINE_Y -
                             SIDELINE_CLEAR_OFFSET)

SHOOT_AT_LEFT_AIM_POINT = Location(NogginConstants.LANDMARK_OPP_GOAL_LEFT_POST_X,
                                   NogginConstants.LANDMARK_OPP_GOAL_RIGHT_POST_Y +
                                   NogginConstants.GOAL_WIDTH *3.0/4.0)

SHOOT_AT_RIGHT_AIM_POINT = Location(NogginConstants.LANDMARK_OPP_GOAL_LEFT_POST_X,
                                    NogginConstants.LANDMARK_OPP_GOAL_RIGHT_POST_Y +
                                    NogginConstants.GOAL_WIDTH/4.0)

CENTER_BALL_POINT = Location(NogginConstants.OPP_GOALBOX_LEFT_X,
                             NogginConstants.CENTER_FIELD_Y)

LEFT_KICKOFF_POINT = Location(NogginConstants.CENTER_FIELD_X +
                              NogginConstants.CENTER_CIRCLE_RADIUS,
                              NogginConstants.CENTER_FIELD_Y -
                              NogginConstants.CENTER_CIRCLE_RADIUS)

RIGHT_KICKOFF_POINT = Location(NogginConstants.CENTER_FIELD_X +
                              NogginConstants.CENTER_CIRCLE_RADIUS,
                              NogginConstants.CENTER_FIELD_Y +
                              NogginConstants.CENTER_CIRCLE_RADIUS)

SHOOT_BALL_LOC_ALIGN_ANGLE = 5
SHOOT_BALL_SIDE_KICK_ANGLE = 45

SHOOT_BALL_FAR_LOC_ALIGN_ANGLE = 15
SHOOT_BALL_FAR_SIDE_KICK_ANGLE = 45

NUM_ALIGN_KICK_STEPS = 1

ORBIT_OWN_GOAL_ANGLE_THRESH = 30
ORBIT_OWN_GOAL_HEADING_THRESH = 150

KICK_STRAIGHT_POST_BEARING = 10
KICK_STRAIGHT_BEARING_THRESH = 30

## KickInformation constants
GPOST_NEAR_THRESHOLD = 200
