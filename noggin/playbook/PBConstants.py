
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
USE_DUB_D = True
USE_FINDER = False
USE_DEEP_STOPPER = False # Fallback to a deep defensive position
KICKOFF_PLAY = False # Forces the more intelligent and restrictive kickoff play
PULL_THE_GOALIE = False
USE_FANCY_GOALIE = True

GOALIE_NUMBER = 1
DEFAULT_CHASER_NUMBER = 3
# Length of time to spend in the kickoff play
KICKOFF_FORMATION_TIME = 3

# Time limit for moving into the finder routine
FINDER_TIME_THRESH = 5
KICKOFF_PLAY_SWITCH_TIME = 2. # Time around which we want player 4 to chase
NUM_TEAM_PLAYERS = NogginConstants.NUM_PLAYERS_PER_TEAM
PACKET_DEAD_PERIOD = 5
INACTIVE_THRESH = 5

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
BALL_DIVERGENCE_THRESH = 20000.0
# Special cases for waiting for the ball at half field
CENTER_FIELD_DIST_THRESH = 125.

####
#### Information about the roles ####
####

# number of roles
NUM_ROLES = 8
# dictionary of roles
ROLES = dict(zip(range(NUM_ROLES), ("INIT_ROLE",
                                    "PENALTY_ROLE",
                                    "CHASER",
                                    "MIDDIE",
                                    "OFFENDER",
                                    "DEFENDER",
                                    "SEARCHER",
                                    "GOALIE")))
# tuple of roles
(INIT_ROLE,
 PENALTY_ROLE,
 CHASER,
 MIDDIE,
 OFFENDER,
 DEFENDER,
 SEARCHER,
 GOALIE) = range(NUM_ROLES)

#### SUB_ROLE CONSTANTS ####
SUB_ROLE_SWITCH_BUFFER = 10.
# dictionary of subRoles
NUM_SUB_ROLES = 24
SUB_ROLES = dict(zip(range(NUM_SUB_ROLES), ("INIT_SUB_ROLE",
                                            "PENALTY_SUB_ROLE",
                                            #OFFENDER SUB ROLES 2-4
                                            "LEFT_WING",
                                            "RIGHT_WING",
                                            "DUBD_OFFENDER",

                                            # MIDDIE SUB ROLES 5-6
                                            "DEFENSIVE_MIDDIE",
                                            "OFFENSIVE_MIDDIE",

                                            # DEFENDER SUB ROLES 7-11
                                            "STOPPER",
                                            "DEEP_STOPPER",
                                            "SWEEPER",
                                            "LEFT_DEEP_BACK",
                                            "RIGHT_DEEP_BACK",

                                            # CHASER SUB ROLES 12
                                            "CHASE_NORMAL",

                                            # GOALIE SUB ROLE 13-14
                                            "GOALIE_NORMAL",
                                            "GOALIE_CHASER",

                                            # FINDER SUB ROLES 15-18
                                            "FRONT_FINDER",
                                            "LEFT_FINDER",
                                            "RIGHT_FINDER",
                                            "OTHER_FINDER",

                                            # KICKOFF SUB ROLES 19-20
                                            "KICKOFF_SWEEPER",
                                            "KICKOFF_STRIKER",

                                            # READY SUB ROLES 21-23
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
 DEEP_STOPPER,
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

 READY_CHASER,
 READY_DEFENDER,
 READY_OFFENDER
) = range(NUM_SUB_ROLES)


## POSITION CONSTANTS ##

# READY_KICKOFF: two back, one forward
READY_KICKOFF_DEFENDER_0 = [NogginConstants.CENTER_FIELD_X * 1./2.,
                            NogginConstants.FIELD_HEIGHT * 1./4.,
                            NogginConstants.OPP_GOAL_HEADING] # right

READY_KICKOFF_DEFENDER_1 = [NogginConstants.CENTER_FIELD_X * 1./2.,
                            NogginConstants.FIELD_HEIGHT * 3./4.,
                            NogginConstants.OPP_GOAL_HEADING] # left


READY_KICKOFF_CHASER = [NogginConstants.CENTER_FIELD_X -
                        NogginConstants.CENTER_CIRCLE_RADIUS/2,
                        NogginConstants.CENTER_FIELD_Y,
                        NogginConstants.OPP_GOAL_HEADING] # center

READY_KICKOFF_NORMAL_CHASER = [NogginConstants.CENTER_FIELD_X -
                               NogginConstants.CENTER_CIRCLE_RADIUS/2,
                               NogginConstants.CENTER_FIELD_Y,
                               NogginConstants.OPP_GOAL_HEADING]

READY_KICKOFF_OFFENDER_0 = [NogginConstants.CENTER_FIELD_X * 1./2.,
                            NogginConstants.FIELD_HEIGHT * 3./4.,
                            NogginConstants.OPP_GOAL_HEADING] # left

READY_KICKOFF_OFFENDER_1 = [NogginConstants.CENTER_FIELD_X * 1./2.,
                            NogginConstants.FIELD_HEIGHT * 1./4.,
                            NogginConstants.OPP_GOAL_HEADING] # right


READY_KICKOFF_STOPPER = [NogginConstants.FIELD_WIDTH * 2./5.,
                         NogginConstants.FIELD_HEIGHT * 1./4.,
                         NogginConstants.OPP_GOAL_HEADING]

# READY_NON_KICKOFF
# non kickoff positions: three in a row, one back in corner
# this was a really quick make, should be rethought out
# Stricker should maintain straight line with the ball

# Here we make the Y behind the halfway point of the y
NON_KICKOFF_X = ((NogginConstants.CENTER_FIELD_X -
                  NogginConstants.FIELD_WHITE_LEFT_SIDELINE_X) / 2.)

READY_NON_KICKOFF_DEFENDER = [NON_KICKOFF_X - 50.,
                              NogginConstants.FIELD_HEIGHT * 3./4.,
                              NogginConstants.OPP_GOAL_HEADING] # right back

READY_NON_KICKOFF_CHASER = [NON_KICKOFF_X,
                            NogginConstants.FIELD_HEIGHT * 2./5.,
                            NogginConstants.OPP_GOAL_HEADING] # left

READY_NON_KICKOFF_OFFENDER = [NON_KICKOFF_X,
                              NogginConstants.FIELD_WIDTH * 3./5.,
                              NogginConstants.OPP_GOAL_HEADING] # right

# KICK OFF POSITIONS (right after kickoff, rather)
KICKOFF_OFFENDER_0 = [NogginConstants.CENTER_FIELD_X * 1./2.,
                      NogginConstants.FIELD_HEIGHT * 1./4.,
                      NogginConstants.OPP_GOAL_HEADING]
KICKOFF_OFFENDER_1 = [NogginConstants.CENTER_FIELD_X * 1./2.,
                      NogginConstants.FIELD_HEIGHT * 3./4.,
                      NogginConstants.OPP_GOAL_HEADING]

KICKOFF_DEFENDER_0 = [NogginConstants.CENTER_FIELD_X * 1./2.,
                      NogginConstants.FIELD_HEIGHT * 1./4.,
                      NogginConstants.OPP_GOAL_HEADING]
KICKOFF_DEFENDER_1 = [NogginConstants.CENTER_FIELD_X * 1./2.,
                      NogginConstants.FIELD_HEIGHT * 3./4.,
                      NogginConstants.OPP_GOAL_HEADING]

# KICK OFF POSITIONS (right after kickoff, rather)
KICKOFF_PLAY_OFFENDER = [NogginConstants.FIELD_WIDTH * 4./5.,
                         NogginConstants.FIELD_HEIGHT * 2./3.,
                         NogginConstants.OPP_GOAL_HEADING]
KICKOFF_PLAY_DEFENDER = [NogginConstants.FIELD_WIDTH * 1./2.,
                         NogginConstants.FIELD_HEIGHT * 1./4.,
                         NogginConstants.OPP_GOAL_HEADING]

DEFAULT_CHASER_NUMBER = 3

# Defender
DEFENDER_BALL_DIST = 100
SWEEPER_X = NogginConstants.MY_GOALBOX_RIGHT_X + 50.
SWEEPER_Y = NogginConstants.CENTER_FIELD_Y
DEEP_STOPPER_X = NogginConstants.CENTER_FIELD_X * 1./2.
STOPPER_MAX_X = NogginConstants.CENTER_FIELD_X - 75.

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
BALL_LOC_LIMIT = 270 # Dist at which we stop active localization and just track
GOALIE_HOME_X = NogginConstants.MY_GOALBOX_LEFT_X +\
    NogginConstants.GOALBOX_DEPTH * 0.5
GOALIE_HOME_Y = NogginConstants.CENTER_FIELD_Y
END_CLEAR_BUFFER = 20.
# elliptical positioning
GOAL_CENTER_X = NogginConstants.FIELD_WHITE_LEFT_SIDELINE_X
GOAL_CENTER_Y = NogginConstants.CENTER_FIELD_Y
ELLIPSE_X_SHIFT = 0. # Increase this to account for the goalposts
LARGE_ELLIPSE_HEIGHT = NogginConstants.GOALBOX_DEPTH / 4.0 #radius
LARGE_ELLIPSE_WIDTH = NogginConstants.CROSSBAR_CM_WIDTH / 2.0 #radius
LARGE_ELLIPSE_CENTER_Y = NogginConstants.CENTER_FIELD_Y
LARGE_ELLIPSE_CENTER_X = NogginConstants.FIELD_WHITE_LEFT_SIDELINE_X
# Angle limits for moving about ellipse
ELLIPSE_ANGLE_MAX = 170.
ELLIPSE_ANGLE_MIN = 10.
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
NUM_FORMATIONS = 19
# dictionary of formations
FORMATIONS = dict(zip(range(NUM_FORMATIONS), ("INIT_FORMATION",
                                              "PENALTY_FORMATION",
                                              "NO_FIELD_PLAYERS",
                                              "ONE_FIELD",
                                              "TWO_FIELD",
                                              "THREE_FIELD",
                                              "DEFENSIVE",
                                              "NEUTRAL_DEFENSE",
                                              "NEUTRAL_OFFENSE",
                                              "OFFENSIVE",
                                              "DUB_D",
                                              "FINDER",
                                              "KICKOFF_PLAY",
                                              "TWO_KICKOFF",
                                              "ONE_KICKOFF",
                                              "READY_FORMATION",
                                              "TEST_DEFEND",
                                              "TEST_OFFEND",
                                              "TEST_CHASE")))
# tuple of formations
(INIT_FORMATION,
 PENALTY_FORMATION,
 NO_FIELD_PLAYERS,
 ONE_FIELD,
 TWO_FIELD,
 THREE_FIELD,
 DEFENSIVE,
 NEUTRAL_DEFENSE,
 NEUTRAL_OFFENSE,
 OFFENSIVE,
 DUB_D,
 FINDER,
 KICKOFF_PLAY,
 TWO_KICKOFF,
 ONE_KICKOFF,
 READY_FORMATION,
 TEST_DEFEND,
 TEST_OFFEND,
 TEST_CHASE) = range(NUM_FORMATIONS)

# Middie Positions
MIDDIE_X_OFFSET = 75.0
MIDDIE_Y_OFFSET = 75.0
MIN_MIDDIE_Y = NogginConstants.FIELD_WHITE_BOTTOM_SIDELINE_Y + MIDDIE_Y_OFFSET
MAX_MIDDIE_Y = NogginConstants.FIELD_WHITE_TOP_SIDELINE_Y - MIDDIE_Y_OFFSET
DEFENSIVE_MIDDIE_X = NogginConstants.CENTER_FIELD_X - MIDDIE_X_OFFSET
OFFENSIVE_MIDDIE_X = NogginConstants.CENTER_FIELD_X + MIDDIE_X_OFFSET

# S_DEFENSIVE_MID strategy
S_MIDDIE_DEFENDER_THRESH = NogginConstants.CENTER_FIELD_X * 1.5

NUM_STRATEGIES = 12

STRATEGIES = dict(zip(range(NUM_STRATEGIES), ("INIT_STRATEGY",
                                             "READY_STRATEGY",

                                             # field player number strategies
                                             "NO_FIELD_PLAYERS",
                                             "ONE_FIELD_PLAYER",
                                             "TWO_FIELD_PLAYERS",
                                             "THREE_FIELD_PLAYERS",

                                             # More 2 field player strats
                                             "TWO_PLAYER_ZONE",
                                             "DEFENSIVE_MID",

                                             # Test strategies
                                             "TEST_DEFENDER",
                                             "TEST_OFFENDER",
                                             "TEST_MIDDIE",
                                             "TEST_CHASER"
                                             )))
(S_INIT,
 S_READY,
 S_NO_FIELD_PLAYERS,
 S_ONE_FIELD_PLAYER,
 S_TWO_FIELD_PLAYERS,
 S_THREE_FIELD_PLAYERS,

 S_TWO_ZONE,
 S_DEFENSIVE_MID,

 S_TEST_DEFENDER,
 S_TEST_OFFENDER,
 S_TESET_MIDDIE,
 S_TEST_CHASER) = range(NUM_STRATEGIES)
