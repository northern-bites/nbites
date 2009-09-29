"""
File to hold all of the cross field constants for the cooperative behavior system
"""

from .. import NogginConstants
from math import pi

# Test switches to force one role to always be given out
TEST_DEFENDER = False
TEST_OFFENDER = False
TEST_CHASER = False

# Print information as to how the chaser is determined
DEBUG_DET_CHASER = False
DEBUG_DET_SUPPORTER = False
DEBUG_SEARCHER = False
DEBUG_DETERMINE_CHASE_TIME = False

USE_ZONE_STRATEGY = False
USE_DUB_D = False
USE_FINDER = False
PULL_THE_GOALIE = False
USE_FANCY_GOALIE = True

DEFAULT_GOALIE_NUMBER = 1
DEFAULT_DEFENDER_NUMBER = 2
DEFAULT_CHASER_NUMBER = 3
# Length of time to spend in the kickoff play
KICKOFF_FORMATION_TIME = 3

# Time limit for moving into the finder routine
FINDER_TIME_THRESH = 5
NUM_TEAM_PLAYERS = NogginConstants.NUM_PLAYERS_PER_TEAM
PACKET_DEAD_PERIOD = 5 # TO-DO: look at shortening so it replaces penalized

####
#### Role Switching / Tie Breaking ####
####

# penalty is: (ball_dist*heading)/scale
PLAYER_HEADING_PENALTY_SCALE = 300.0 # max 60% of distance
# penalty is: (ball_dist*ball_bearing)/scale
BALL_BEARING_PENALTY_SCALE = 200.0 # max 90% of distance
NO_VISUAL_BALL_PENALTY = 2 # centimeter penalty for not seeing the ball
TEAMMATE_CHASING_PENALTY = 3 # adds standard penalty for teammates (buffer)
TEAMMATE_CHASER_USE_NUMBERS_BUFFER = 20.0 # cm
TEAMMATE_POSITIONING_USE_NUMBERS_BUFFER = 25.0 # cm
# role switching constants
CALL_OFF_THRESH = 125.
LISTEN_THRESH = 250.
STOP_CALLING_THRESH = 275.
BEARING_BONUS = 300.
BALL_NOT_SUPER_OFF_BONUS = 300.
BALL_CAPTURE_BONUS = 1000.
KICKOFF_PLAY_BONUS = 1000.
BEARING_SMOOTHNESS = 500.
BALL_ON_BONUS = 1000.

# Velocity bonus constants
VB_MIN_REL_VEL_Y = -100.
VB_MAX_REL_VEL_Y = -20.
VB_MAX_REL_Y = 200.
VB_X_THRESH = 30.
VELOCITY_BONUS = 250.

CHASE_SPEED = 7.00
CHASE_SPIN = 20.00
SUPPORT_SPEED = CHASE_SPEED
SUPPORT_SPIN = CHASE_SPIN
SEC_TO_MILLIS = 1000.0
NEAR_LINE_THRESH = 15.
# Determine chaser junk
BALL_DIVERGENCE_THRESH = 2000.0
# Special cases for waiting for the ball at half field
CENTER_FIELD_DIST_THRESH = 125.

####
#### Information about the roles ####
####

# number of roles
NUM_ROLES = 9
# dictionary of roles
ROLES = dict(zip(range(NUM_ROLES), ("INIT_ROLE",
                                    "PENALTY_ROLE",
                                    "CHASER",
                                    "MIDDIE",
                                    "OFFENDER",
                                    "DEFENDER",
                                    "SEARCHER",
                                    "GOALIE",
                                    "DEFENDER_DUB_D")))
# tuple of roles
(INIT_ROLE,
 PENALTY_ROLE,
 CHASER,
 MIDDIE,
 OFFENDER,
 DEFENDER,
 SEARCHER,
 GOALIE,
 DEFENDER_DUB_D) = range(NUM_ROLES)

#### SUB_ROLE CONSTANTS ####
SUB_ROLE_SWITCH_BUFFER = 10.
# dictionary of subRoles
NUM_SUB_ROLES = 26
SUB_ROLES = dict(zip(range(NUM_SUB_ROLES), ("INIT_SUB_ROLE",
                                            "PENALTY_SUB_ROLE",
                                            #OFFENDER SUB ROLES 2-4
                                            "LEFT_WING",
                                            "RIGHT_WING",
                                            "DUBD_OFFENDER",

                                            # MIDDIE SUB ROLES 5-6
                                            "DEFENSIVE_MIDDIE",
                                            "OFFENSIVE_MIDDIE",

                                            # DEFENDER SUB ROLES 7-12
                                            "STOPPER",
                                            "BOTTOM_STOPPER",
                                            "TOP_STOPPER",
                                            "SWEEPER",
                                            "LEFT_DEEP_BACK",
                                            "RIGHT_DEEP_BACK",

                                            # CHASER SUB ROLES 13
                                            "CHASE_NORMAL",

                                            # GOALIE SUB ROLE 14-15
                                            "GOALIE_NORMAL",
                                            "GOALIE_CHASER",

                                            # FINDER SUB ROLES 16-19
                                            "FRONT_FINDER",
                                            "LEFT_FINDER",
                                            "RIGHT_FINDER",
                                            "OTHER_FINDER",

                                            # KICKOFF SUB ROLES 20-21
                                            "KICKOFF_SWEEPER",
                                            "KICKOFF_STRIKER",

                                            # READY SUB ROLES 22-25
                                            "READY_GOALIE",
                                            "READY_CHASER",
                                            "READY_DEFENDER",
                                            "READY_OFFENDER" )))
# tuple of subRoles
(INIT_SUB_ROLE,
 PENALTY_SUB_ROLE,

 LEFT_WING,
 RIGHT_WING,
 DUBD_OFFENDER,

 DEFENSIVE_MIDDIE,
 OFFENSIVE_MIDDIE,

 STOPPER,
 BOTTOM_STOPPER,
 TOP_STOPPER,
 SWEEPER,
 LEFT_DEEP_BACK,
 RIGHT_DEEP_BACK,

 CHASE_NORMAL,

 GOALIE_NORMAL,
 GOALIE_CHASER,

 FRONT_FINDER,
 LEFT_FINDER,
 RIGHT_FINDER,
 OTHER_FINDER,

 KICKOFF_SWEEPER,
 KICKOFF_STRIKER,

 READY_GOALIE,
 READY_CHASER,
 READY_DEFENDER,
 READY_OFFENDER
) = range(NUM_SUB_ROLES)


## POSITION CONSTANTS ##


READY_KICKOFF_DEFENDER_X = NogginConstants.CENTER_FIELD_X * 0.5
READY_KICKOFF_DEFENDER_CENTER_OFFSET = NogginConstants.FIELD_WHITE_HEIGHT/10.

READY_KICKOFF_DEFENDER_0_POS = [READY_KICKOFF_DEFENDER_X,
                                NogginConstants.CENTER_FIELD_Y -
                                READY_KICKOFF_DEFENDER_CENTER_OFFSET] # left

READY_KICKOFF_DEFENDER_1_POS = [READY_KICKOFF_DEFENDER_X,
                                NogginConstants.CENTER_FIELD_Y +
                                READY_KICKOFF_DEFENDER_CENTER_OFFSET] # right

READY_KICKOFF_OFFENDER_X = READY_KICKOFF_DEFENDER_X #keeps positions level
READY_KICKOFF_OFFENDER_CENTER_OFFSET = READY_KICKOFF_DEFENDER_CENTER_OFFSET

READY_KICKOFF_OFFENDER_0_POS = [READY_KICKOFF_OFFENDER_X,
                                NogginConstants.CENTER_FIELD_Y +
                                READY_KICKOFF_OFFENDER_CENTER_OFFSET] # right

READY_KICKOFF_OFFENDER_1_POS = [READY_KICKOFF_OFFENDER_X,
                                NogginConstants.CENTER_FIELD_Y -
                                READY_KICKOFF_OFFENDER_CENTER_OFFSET] # left

READY_KICKOFF_CHASER_POS = [NogginConstants.CENTER_FIELD_X -
                        NogginConstants.CENTER_CIRCLE_RADIUS/2.0,
                        NogginConstants.CENTER_FIELD_Y] # near center

# READY_NON_KICKOFF

READY_NON_KICKOFF_MAX_X = (NogginConstants.GREEN_PAD_X +
                                NogginConstants.FIELD_GREEN_WIDTH * 1./4.)

READY_NON_KICKOFF_DEFENDER_POS = [READY_NON_KICKOFF_MAX_X,
                              NogginConstants.GREEN_PAD_Y +
                              NogginConstants.FIELD_GREEN_HEIGHT * 1./5.]

READY_NON_KICKOFF_CHASER_POS = [READY_NON_KICKOFF_MAX_X,
                            NogginConstants.CENTER_FIELD_Y]

READY_NON_KICKOFF_OFFENDER_POS = [READY_NON_KICKOFF_MAX_X,
                              NogginConstants.GREEN_PAD_Y +
                              NogginConstants.FIELD_WHITE_WIDTH * 3./5.,
                              NogginConstants.OPP_GOAL_HEADING]

#TODO: reconsider where players should move after kickoff
# KICK OFF POSITIONS (right after kickoff, rather)
KICKOFF_OFFENDER_0_POS = [NogginConstants.CENTER_FIELD_X * 1./2.,
                      NogginConstants.FIELD_HEIGHT * 1./4.]

KICKOFF_OFFENDER_1_POS = [NogginConstants.CENTER_FIELD_X * 1./2.,
                      NogginConstants.FIELD_HEIGHT * 3./4.]

KICKOFF_DEFENDER_0_POS = [NogginConstants.CENTER_FIELD_X * 1./2.,
                      NogginConstants.FIELD_HEIGHT * 1./4.]

KICKOFF_DEFENDER_1_POS = [NogginConstants.CENTER_FIELD_X * 1./2.,
                      NogginConstants.FIELD_HEIGHT * 3./4.]

# Defender
DEFENDER_BALL_DIST = 100
SWEEPER_X = NogginConstants.MY_GOALBOX_RIGHT_X + 50.
SWEEPER_Y = NogginConstants.CENTER_FIELD_Y
STOPPER_MAX_X = NogginConstants.CENTER_FIELD_X - 75.
STOPPER_X = NogginConstants.CENTER_FIELD_X - 150.
TOP_STOPPER_Y = NogginConstants.CENTER_FIELD_Y + 75.
BOTTOM_STOPPER_Y = NogginConstants.CENTER_FIELD_Y - 75.

# Offender
WING_X_OFFSET = 100.
WING_Y_OFFSET = 100.
WING_MIN_X = NogginConstants.CENTER_FIELD_X
WING_MAX_X = NogginConstants.OPP_GOALBOX_LEFT_X
LEFT_WING_MIN_Y = NogginConstants.GREEN_PAD_Y
LEFT_WING_MAX_Y = NogginConstants.CENTER_FIELD_Y
RIGHT_WING_MIN_Y = NogginConstants.CENTER_FIELD_Y
RIGHT_WING_MAX_Y = (NogginConstants.FIELD_WIDTH - NogginConstants.GREEN_PAD_Y)

#GOALIE
BALL_LOC_LIMIT = 220 # Dist at which we stop active localization and just track
# elliptical positioning
GOAL_CENTER_X = NogginConstants.FIELD_WHITE_LEFT_SIDELINE_X
GOAL_CENTER_Y = NogginConstants.CENTER_FIELD_Y
ELLIPSE_X_SHIFT = 5. # Increase this to account for the goalposts
LARGE_ELLIPSE_HEIGHT = NogginConstants.GOALBOX_DEPTH * 0.65 #radius
LARGE_ELLIPSE_WIDTH = NogginConstants.CROSSBAR_CM_WIDTH / 2.0 #radius
LARGE_ELLIPSE_CENTER_Y = NogginConstants.CENTER_FIELD_Y
LARGE_ELLIPSE_CENTER_X = NogginConstants.FIELD_WHITE_LEFT_SIDELINE_X
GOALIE_HOME_X = NogginConstants.MY_GOALBOX_LEFT_X + LARGE_ELLIPSE_HEIGHT
GOALIE_HOME_Y = NogginConstants.CENTER_FIELD_Y
ELLIPSE_POSITION_LIMIT = BALL_LOC_LIMIT
# Angle limits for moving about ellipse
ELLIPSE_ANGLE_MAX = 80
ELLIPSE_ANGLE_MIN = -80.0
RAD_TO_DEG = 180. / pi
DEG_TO_RAD = pi / 180.
BALL_FOCUS_UNCERT_THRESH = 100.


# Finder
TWO_DOG_FINDER_POSITIONS = (
    (NogginConstants.FIELD_WIDTH * 1./2.,
     NogginConstants.CENTER_FIELD_Y),
    (NogginConstants.FIELD_WIDTH * 1./3.,
     NogginConstants.CENTER_FIELD_Y))

# Dub_d
DEEP_BACK_X = SWEEPER_X
LEFT_DEEP_BACK_Y = NogginConstants.MY_GOALBOX_BOTTOM_Y - 40.
RIGHT_DEEP_BACK_Y = NogginConstants.MY_GOALBOX_TOP_Y + 40.
LEFT_DEEP_BACK_POS =  (DEEP_BACK_X, LEFT_DEEP_BACK_Y)
RIGHT_DEEP_BACK_POS = (DEEP_BACK_X, RIGHT_DEEP_BACK_Y)

# number of formations
NUM_FORMATIONS = 16
# dictionary of formations
FORMATIONS = dict(zip(range(NUM_FORMATIONS), ("INIT_FORMATION",
                                              "PENALTY_FORMATION",
                                              "NO_FIELD_PLAYERS",
                                              "ONE_FIELD",
                                              "DEFENSIVE_TWO_FIELD",
                                              "NEUTRAL_DEFENSE_TWO_FIELD",
                                              "THREE_FIELD",
                                              "ONE_DUB_D",
                                              "TWO_DUB_D",
                                              "THREE_DUB_D",
                                              "FINDER",
                                              "KICKOFF",
                                              "READY_FORMATION",
                                              "TEST_DEFEND",
                                              "TEST_OFFEND",
                                              "TEST_CHASE")))
# tuple of formations
(INIT_FORMATION,
 PENALTY_FORMATION,
 NO_FIELD_PLAYERS,
 ONE_FIELD,
 DEFENSIVE_TWO_FIELD,
 NEUTRAL_DEFENSE_TWO_FIELD,
 THREE_FIELD,
 ONE_DUB_D,
 TWO_DUB_D,
 THREE_DUB_D,
 FINDER,
 KICKOFF,
 READY_FORMATION,
 TEST_DEFEND,
 TEST_OFFEND,
 TEST_CHASE) = range(NUM_FORMATIONS)

# Middie Positions
MIDDIE_X_OFFSET = 50.0
MIDDIE_Y_OFFSET = 75.0
MIN_MIDDIE_Y = NogginConstants.FIELD_WHITE_BOTTOM_SIDELINE_Y + MIDDIE_Y_OFFSET
MAX_MIDDIE_Y = NogginConstants.FIELD_WHITE_TOP_SIDELINE_Y - MIDDIE_Y_OFFSET
DEFENSIVE_MIDDIE_X = NogginConstants.CENTER_FIELD_X - MIDDIE_X_OFFSET
OFFENSIVE_MIDDIE_X = NogginConstants.CENTER_FIELD_X + MIDDIE_X_OFFSET

# S_DEFENSIVE_MID strategy
S_MIDDIE_DEFENDER_THRESH = NogginConstants.CENTER_FIELD_X * 1.5

NUM_STRATEGIES = 13

STRATEGIES = dict(zip(range(NUM_STRATEGIES), ("INIT_STRATEGY",
                                              "PENALTY_STRATEGY",
                                             "READY_STRATEGY",

                                             # field player number strategies
                                             "NO_FIELD_PLAYERS",
                                             "ONE_FIELD_PLAYER",
                                             "TWO_FIELD_PLAYERS",
                                             "THREE_FIELD_PLAYERS",

                                             # More 2 field player strats
                                             "TWO_PLAYER_ZONE",
                                             "sWIN",

                                             # Test strategies
                                             "TEST_DEFENDER",
                                             "TEST_OFFENDER",
                                             "TEST_MIDDIE",
                                             "TEST_CHASER"
                                             )))
(INIT_STRATEGY,
 PENALTY_STRATEGY,
 S_READY,
 S_NO_FIELD_PLAYERS,
 S_ONE_FIELD_PLAYER,
 S_TWO_FIELD_PLAYERS,
 S_THREE_FIELD_PLAYERS,

 S_TWO_ZONE,
 S_WIN,

 S_TEST_DEFENDER,
 S_TEST_OFFENDER,
 S_TESET_MIDDIE,
 S_TEST_CHASER) = range(NUM_STRATEGIES)
