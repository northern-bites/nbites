
"""
File to hold all of the cross fiel constants for the cooperative behavior system
"""

from .. import NogginConstants

# Test switches to force one role to always be given out
TEST_DEFENDER = False
TEST_OFFENDER = False
TEST_CHASER = False
TEST_MIDDIE = False
# Print information as to how the chaser is determined
DEBUG_DET_CHASER = False
DEBUG_DET_SUPPORTER = False
DEBUG_SEARCHER = False

USE_DEFENDER_CHASER = False # Has a single robot act as a hybrid defender-chaser
USE_DEEP_STOPPER = False # Fallback to a deep defensive position
KICKOFF_PLAY = True # Forces the more intelligent and restrictive kickoff play

GOALIE_NUMBER = 1
# Length of time to spend in the kickoff play
KICKOFF_FORMATION_TIME = 5000

# Time limit for moving into the finder routine
FINDER_TIME_THRESH = 5000
KICKOFF_PLAY_SWITCH_TIME = 2000. # Time around which we want player 4 to chase
NUM_TEAM_PLAYERS = 4
PACKET_DEAD_PERIOD = 5000
INACTIVE_THRESH = 5000

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
# Velocity bonus constants
VB_MIN_REL_VEL_Y = -100.
VB_MAX_REL_VEL_Y = -20.
VB_MAX_REL_Y = 200.
VB_X_THRESH = 30.
VELOCITY_BONUS = 250.

GOAL_BACK_X = NogginConstants.CENTER_FIELD_X
GOAL_BACK_Y = NogginConstants.FIELD_GREEN_HEIGHT

CHASE_SPEED = 20.00#max(NogginConstants.FORWARD_ACTUAL_SPEEDS)
CHASE_SPIN = 10.00#max(NogginConstants.SPIN_ACTUAL_SPEEDS)
SUPPORT_SPEED = CHASE_SPEED
SUPPORT_SPIN = CHASE_SPIN
SEC_TO_MILLIS = 1000.0
NEAR_LINE_THRESH = 15.

# Special cases for waiting for the ball at half field
CENTER_FIELD_DIST_THRESH = 125.

OFFENSIVE_ZONE_Y_LIMIT = NogginConstants.MIDFIELD_Y + 20.

####
#### Information about the roles ####
####

# number of roles
NUM_ROLES = 7
# dictionary of roles
ROLES = dict(zip(range(NUM_ROLES), ("INIT_ROLE",
                                    "CHASER",
                                    "OFFENDER",
                                    "MIDDIE",
                                    "DEFENDER",
                                    "SEARCHER",
                                    "GOALIE")))
# tuple of roles
(INIT_ROLE,
 CHASER,
 OFFENDER,
 MIDDIE,
 DEFENDER,
 SEARCHER,
 GOALIE) = range(NUM_ROLES)

#### SUB_ROLE CONSTANTS ####
SUB_ROLE_SWITCH_BUFFER = 10.
# dictionary of subRoles
NUM_SUB_ROLES = 36
SUB_ROLES = dict(zip(range(NUM_SUB_ROLES), ("INIT_SUB_ROLE",
                                            #OFFENDER SUB ROLES
                                            "LEFT_WING",
                                            "RIGHT_WING",
                                            "LEFT_STRIKER",
                                            "RIGHT_STRIKER",
                                            "FORWARD_MIDFIELD",
                                            "CENTER_STRIKER",
                                            "CENTER_O_MIDFIELD",
                                            "RIGHT_SIDELINE_O",
                                            "LEFT_SIDELINE_O",

                                            # MIDDIE SUB ROLES
                                            "LEFT_O_MIDDIE",
                                            "RIGHT_O_MIDDIE",
                                            "CENTER_O_MIDDIE",
                                            "LEFT_D_MIDDIE",
                                            "RIGHT_D_MIDDIE",
                                            "CENTER_D_MIDDIE",

                                            # DEFENDER SUB ROLES
                                            "STOPPER",
                                            "DEEP_STOPPER",
                                            "SWEEPER",
                                            "DEFENSIVE_MIDFIELD",
                                            "LEFT_DEEP_BACK",
                                            "RIGHT_DEEP_BACK",

                                            # CHASER SUB ROLES
                                            "CHASE_NORMAL",
                                            "CHASE_AROUND_BOX",

                                            # FINDER SUB ROLES
                                            "FRONT_FINDER",
                                            "LEFT_FINDER",
                                            "RIGHT_FINDER",
                                            "OTHER_FINDER",

                                            # GOALIE SUB ROLE
                                            "GOALIE_SUB_ROLE",

                                            # KICKOFF SUB ROLES
                                            "KICKOFF_SWEEPER",
                                            "KICKOFF_STRIKER",
                                            "KICKOFF_MIDDIE",

                                            # READY SUB ROLES
                                            "READY_CHASER",
                                            "READY_DEFENDER",
                                            "READY_OFFENDER",
                                            "READY_MIDDIE")))
# tuple of subRoles
(INIT_SUB_ROLE,

 LEFT_WING,
 RIGHT_WING,
 LEFT_STRIKER,
 RIGHT_STRIKER,
 FORWARD_MIDFIELD,
 CENTER_STRIKER,
 CENTER_O_MIDFIELD,
 RIGHT_SIDELINE_O,
 LEFT_SIDELINE_O,

 LEFT_O_MIDDIE,
 RIGHT_O_MIDDIE,
 CENTER_O_MIDDIE,
 LEFT_D_MIDDIE,
 RIGHT_D_MIDDIE,
 CENTER_D_MIDDIE,
 STOPPER,
 DEEP_STOPPER,
 SWEEPER,
 DEFENSIVE_MIDFIELD,
 LEFT_DEEP_BACK,
 RIGHT_DEEP_BACK,

 CHASE_NORMAL,
 CHASE_AROUND_BOX,

 FRONT_FINDER,
 LEFT_FINDER,
 RIGHT_FINDER,
 OTHER_FINDER,
 GOALIE_SUB_ROLE,
 KICKOFF_SWEEPER,
 KICKOFF_STRIKER,
 KICKOFF_MIDDIE,
 READY_CHASER,
 READY_DEFENDER,
 READY_OFFENDER,
 READY_MIDDIE) = range(NUM_SUB_ROLES)


## POSITION CONSTANTS ##

# READY_KICKOFF: two back, one forward
READY_KICKOFF_DEFENDER_0 = [NogginConstants.CENTER_FIELD_X * 1./2.,
                          NogginConstants.FIELD_HEIGHT * 1./5.,
                          NogginConstants.OPP_GOAL_HEADING] # left

READY_KICKOFF_DEFENDER_1 = [NogginConstants.CENTER_FIELD_X * 3./2.,
                          NogginConstants.FIELD_HEIGHT * 1./5.,
                          NogginConstants.OPP_GOAL_HEADING] # right


READY_KICKOFF_CHASER = [NogginConstants.CENTER_FIELD_X - 20,
                        NogginConstants.CENTER_FIELD_Y - 40.,
                        -30.0] # center

READY_KICKOFF_NORMAL_CHASER = [NogginConstants.CENTER_FIELD_X,
                               NogginConstants.CENTER_FIELD_Y - 50.0,
                               NogginConstants.OPP_GOAL_HEADING]

READY_KICKOFF_OFFENDER_0 = [NogginConstants.CENTER_FIELD_X * 1./2.,
                          NogginConstants.CENTER_FIELD_Y - 30.0,
                          NogginConstants.OPP_GOAL_HEADING] # left

READY_KICKOFF_OFFENDER_1 = [NogginConstants.CENTER_FIELD_X * 3./2.,
                          NogginConstants.CENTER_FIELD_Y - 30.0,
                          NogginConstants.OPP_GOAL_HEADING] # right

# Starting style just be a wing on the other side of the field
READY_KICKOFF_MIDDIE_0 = READY_KICKOFF_DEFENDER_1
READY_KICKOFF_MIDDIE_1 = READY_KICKOFF_DEFENDER_0


READY_KICKOFF_STOPPER = [NogginConstants.FIELD_WIDTH * 2./5.,
                         NogginConstants.FIELD_HEIGHT * 1./4.,
                         NogginConstants.OPP_GOAL_HEADING]  

# READY_NON_KICKOFF
# non kickoff positions: three in a row, one back in corner
# this was a really quick make, should be rethought out
# Stricker should maintain straight line with the ball

# Here we make the Y behind the halfway point of the y
NON_KICKOFF_Y = ((NogginConstants.CENTER_FIELD_Y - 
                  NogginConstants.FIELD_WHITE_BOTTOM_SIDELINE_Y) / 2.)
READY_NON_KICKOFF_MIDDIE = [NogginConstants.FIELD_WIDTH * 1./4.,
                              NON_KICKOFF_Y - 50.,
                              NogginConstants.OPP_GOAL_HEADING] # left back
READY_NON_KICKOFF_DEFENDER = [NogginConstants.FIELD_WIDTH * 3./4.,
                            NON_KICKOFF_Y - 50.,
                            NogginConstants.OPP_GOAL_HEADING] # right back
READY_NON_KICKOFF_CHASER = [NogginConstants.FIELD_WIDTH * 2./5.,
                            NON_KICKOFF_Y,
                            NogginConstants.OPP_GOAL_HEADING] # left
READY_NON_KICKOFF_OFFENDER = [NogginConstants.FIELD_WIDTH * 3./5.,
                              NON_KICKOFF_Y,
                              NogginConstants.OPP_GOAL_HEADING] # right

READY_NON_KICKOFF_LEFT_POSITION = [NogginConstants.FIELD_WIDTH * 2./5.,
                                   NON_KICKOFF_Y,
                                   NogginConstants.OPP_GOAL_HEADING]

READY_NON_KICKOFF_RIGHT_POSITION = [NogginConstants.FIELD_WIDTH * 3./5.,
                                    NON_KICKOFF_Y,
                                    NogginConstants.OPP_GOAL_HEADING]

# KICK OFF POSITIONS (right after kickoff, rather)
KICKOFF_OFFENDER_0 = [NogginConstants.FIELD_WIDTH * 3./4.,
                      NogginConstants.FIELD_HEIGHT * 2./3.]
KICKOFF_OFFENDER_1 = [NogginConstants.FIELD_WIDTH * 1./4.,
                      NogginConstants.FIELD_HEIGHT * 2./3.]

# Have the midie swap with the defender
KICKOFF_MIDDIE_0 = KICKOFF_OFFENDER_1
KICKOFF_MIDDIE_1 = KICKOFF_OFFENDER_0

KICKOFF_DEFENDER_0 = [NogginConstants.FIELD_WIDTH * 1./2.,
                      NogginConstants.FIELD_HEIGHT * 1./4.]
KICKOFF_DEFENDER_1 = [NogginConstants.FIELD_WIDTH * 1./2.,
                      NogginConstants.FIELD_HEIGHT * 1./4.]

# KICK OFF POSITIONS (right after kickoff, rather)
KICKOFF_PLAY_OFFENDER = [NogginConstants.FIELD_WIDTH * 4./5.,
                    NogginConstants.FIELD_HEIGHT * 2./3.]
KICKOFF_PLAY_DEFENDER = [NogginConstants.FIELD_WIDTH * 1./2.,
                    NogginConstants.FIELD_HEIGHT * 1./4.]
KICKOFF_PLAY_MIDDIE = [NogginConstants.FIELD_WIDTH * 1./5.,
                       NogginConstants.FIELD_HEIGHT / 2.]

# Defender
DEFENDER_BALL_DIST = 100
SWEEPER_X = NogginConstants.CENTER_FIELD_X
SWEEPER_Y = NogginConstants.MY_GOALBOX_TOP_Y + 50.
DEEP_BACK_Y = SWEEPER_Y
LEFT_DEEP_BACK_X = NogginConstants.MY_GOALBOX_LEFT_X - 40.
RIGHT_DEEP_BACK_X = NogginConstants.MY_GOALBOX_RIGHT_X + 40.
DEEP_STOPPER_Y = NogginConstants.CENTER_FIELD_Y * 1./2.
STOPPER_MAX_Y = NogginConstants.CENTER_FIELD_Y - 150.

# Midfield
MIDDIE_SUB_ROLE_BUFFER_SIZE = 15.
DEFENSIVE_CENTER_MIDFIELD_Y = NogginConstants.CENTER_FIELD_Y - 60.
DEFENSIVE_MIDFIELD_Y = NogginConstants.CENTER_FIELD_Y - 100.
OFFENSIVE_CENTER_MIDFIELD_Y = NogginConstants.CENTER_FIELD_Y + 60.
FORWARD_MIDFIELD_Y = NogginConstants.CENTER_FIELD_Y + 125.
FORWARD_MID_LEFT_LIMIT = NogginConstants.CENTER_FIELD_X - 60.
FORWARD_MID_RIGHT_LIMIT = NogginConstants.CENTER_FIELD_X + 60.
MIDFIELD_ZONE_MAX_Y = NogginConstants.CENTER_FIELD_Y + 60.
MIDFIELD_ZONE_MIN_Y = NogginConstants.CENTER_FIELD_Y - 60.


# Sub Role Boundries for offender
OPP_CORNERS_Y = NogginConstants.OPP_GOALBOX_BOTTOM_Y
OFFENDER_SIDELINE_LIMIT_OFFSET = 70.
RIGHT_SIDELINE_OFFENDER_BALL_THRESH_X = (NogginConstants.FIELD_WHITE_RIGHT_SIDELINE_X-
                                         OFFENDER_SIDELINE_LIMIT_OFFSET)
LEFT_SIDELINE_OFFENDER_BALL_THRESH_X = (NogginConstants.FIELD_WHITE_LEFT_SIDELINE_X+
                                        OFFENDER_SIDELINE_LIMIT_OFFSET)
FORWARD_MIDFIELD_BALL_THRESH_Y = NogginConstants.CENTER_FIELD_Y + 25.
CENTER_MID_DEFENDER_LIMIT = NogginConstants.CENTER_FIELD_Y + 70.
CENTER_MID_OFFENDER_LIMIT = NogginConstants.CENTER_FIELD_Y - 40.
CENTER_MID_LEFT_LIMIT = NogginConstants.CENTER_FIELD_X - 100.
CENTER_MID_RIGHT_LIMIT = NogginConstants.CENTER_FIELD_X + 100.
OFFENDER_SUB_ROLE_BUFFER_SIZE = 25.
SIDELINE_BUFFER_SIZE = 35.

# Offender
WING_X_OFFSET = 100.
WING_Y_OFFSET = 100.
WING_MIN_Y = FORWARD_MIDFIELD_Y
WING_MAX_Y = NogginConstants.OPP_GOALBOX_BOTTOM_Y - 50.
WING_SIDELINE_BUFFER = 45.
WING_MID_X_BUFFER = 75.
LEFT_WING_MIN_X = NogginConstants.GREEN_PAD_X + WING_SIDELINE_BUFFER
LEFT_WING_MAX_X = NogginConstants.CENTER_FIELD_X - WING_MID_X_BUFFER
RIGHT_WING_MIN_X = NogginConstants.CENTER_FIELD_X + WING_MID_X_BUFFER
RIGHT_WING_MAX_X = (NogginConstants.FIELD_WIDTH - NogginConstants.GREEN_PAD_X - 
                    WING_SIDELINE_BUFFER)
STRIKER_LEFT_X = NogginConstants.CENTER_FIELD_X - NogginConstants.GOALBOX_WIDTH / 2.
STRIKER_RIGHT_X = NogginConstants.CENTER_FIELD_X + NogginConstants.GOALBOX_WIDTH / 2.
STRIKER_Y = NogginConstants.OPP_GOALBOX_BOTTOM_Y - 50.
SIDELINE_O_Y_OFFSET = 50.
SIDELINE_O_MIN_Y = NogginConstants.CENTER_FIELD_Y + 25.
SIDELINE_O_MAX_Y = (NogginConstants.FIELD_HEIGHT - NogginConstants.GREEN_PAD_Y -
                    140.)
RIGHT_SIDELINE_O_X = NogginConstants.CENTER_FIELD_X + 50.
LEFT_SIDELINE_O_X = NogginConstants.CENTER_FIELD_X - 50.

# Finder
FOUR_DOG_FINDER_POSITIONS = (
    (NogginConstants.FIELD_WHITE_LEFT_SIDELINE_X + 75.0, 
     NogginConstants.FIELD_HEIGHT * 1/2),
    (NogginConstants.FIELD_WHITE_RIGHT_SIDELINE_X - 75.0, 
     NogginConstants.FIELD_HEIGHT * 1/2),
    (NogginConstants.MIDFIELD_X, 
     NogginConstants.FIELD_HEIGHT * 1./4.),
    (NogginConstants.MIDFIELD_X,
     NogginConstants.FIELD_HEIGHT * 3./4.))
THREE_DOG_FINDER_POSITIONS = (
    (NogginConstants.FIELD_WHITE_LEFT_SIDELINE_X + 75.0, 
     NogginConstants.FIELD_HEIGHT * 1/2),
    (NogginConstants.FIELD_WHITE_RIGHT_SIDELINE_X - 75.0, 
     NogginConstants.FIELD_HEIGHT * 1/2),
    (NogginConstants.MIDFIELD_X, 
     NogginConstants.FIELD_HEIGHT * 1./4.))
TWO_DOG_FINDER_POSITIONS = (
    (NogginConstants.CENTER_FIELD_X, 
     NogginConstants.CENTER_FIELD_Y / 2.0),
    (NogginConstants.FIELD_WIDTH * 1./3.,
     NogginConstants.FIELD_HEIGHT * 1./2.))

# Dub_d
CENTER_MIDFIELD_OFFENDER_POS = (NogginConstants.CENTER_FIELD_X, 
                                OFFENSIVE_CENTER_MIDFIELD_Y)
LEFT_DEEP_BACK_POS =  (LEFT_DEEP_BACK_X, DEEP_BACK_Y)
RIGHT_DEEP_BACK_POS = (RIGHT_DEEP_BACK_X, DEEP_BACK_Y)


# number of formations
NUM_FORMATIONS = 13
# dictionary of formations
FORMATIONS = dict(zip(range(NUM_FORMATIONS), ("INIT_FORMATION",
                                              "ONE_DOWN",
                                              "TWO_DOWN"
                                              "THREE_DOWN",
                                              "SPREAD",
                                              "DUB_D",
                                              "FINDER",
                                              "KICKOFF_PLAY",
                                              "KICKOFF",
                                              "THREE_KICKOFF",
                                              "TWO_KICKOFF",
                                              "ONE_KICKOFF",
                                              "READY")))
# tuple of formations
(INIT_FORMATION,
 ONE_DOWN,
 TWO_DOWN,
 THREE_DOWN,
 SPREAD,
 DUB_D,
 FINDER,
 KICKOFF_PLAY,
 KICKOFF,
 THREE_KICKOFF,
 TWO_KICKOFF,
 ONE_KICKOFF,
 READY) = range(NUM_FORMATIONS)
