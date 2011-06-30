"""
File to hold all of the cross field constants for the cooperative behavior system
"""
from .. import NogginConstants
from ..typeDefs.Location import Location
from math import pi

# Test switches to force one role to always be given out
TEST_DEFENDER = False
TEST_OFFENDER = False
TEST_CHASER = False

# Print information as to how the chaser is determined
DEBUG_DET_CHASER = False
DEBUG_DET_SUPPORTER = False

USE_ZONE_STRATEGY = False
USE_DUB_D = True
PULL_THE_GOALIE = False
USE_FANCY_GOALIE = False

DEFAULT_GOALIE_NUMBER = 1
DEFAULT_DEFENDER_NUMBER = 2
DEFAULT_OFFENDER_NUMBER = 3
DEFAULT_CHASER_NUMBER = 4

KICKOFF_FORMATION_TIME = 10 # Length of time to spend in the kickoff play

NUM_TEAM_PLAYERS = NogginConstants.NUM_PLAYERS_PER_TEAM

####
#### Role Switching / Tie Breaking ####
####
# The following constants are in milliseconds
CALL_OFF_THRESH = 1500.             # how likely it is to be chaser
LISTEN_THRESH = 3000.               # how likely it is to stop being chaser
STOP_CALLING_THRESH = 3500.         # how likely you are to ignore teammates ideas.

GOALIE_CHASER_COUNT_THRESH = 25      # how long we wait before goalie calls us off.

# Special cases for waiting for the ball at half field
NEAR_LINE_THRESH = 25.
DEFENDER_SHOULD_CHASE_THRESH = 125.
# S_DEFENSIVE_MID strategy
S_MIDDIE_DEFENDER_THRESH = NogginConstants.CENTER_FIELD_X * 1.5
S_MIDDIE_OFFENDER_THRESH = NogginConstants.CENTER_FIELD_X * 0.5
S_TWO_ZONE_DEFENDER_THRESH = NogginConstants.CENTER_FIELD_X * 1.2


####
#### Information about the Strategies ####
####

NUM_STRATEGIES = 13

# dictionary of strategies
STRATEGIES = dict(zip(range(NUM_STRATEGIES), ("INIT_STRATEGY",
                                              "PENALTY_STRATEGY",
                                              "READY_STRATEGY",

                                              # 0 field player strats
                                              "NO_FIELD_PLAYERS",

                                              # 1 field player strats
                                              "ONE_FIELD_PLAYER",

                                              # 2 field player strats
                                              "TWO_FIELD_PLAYERS",
                                              "TWO_PLAYER_ZONE",

                                              # 3 field player strats
                                              "WIN",

                                              # 4 field player strats
                                              "PULL_GOALIE",

                                              # Test strategies
                                              "TEST_DEFENDER",
                                              "TEST_OFFENDER",
                                              "TEST_MIDDIE",
                                              "TEST_CHASER"
                                              )))

# tuple of strategies
(INIT_STRATEGY,
 PENALTY_STRATEGY,
 S_READY,

 S_NO_FIELD_PLAYERS,

 S_ONE_FIELD_PLAYER,

 S_TWO_FIELD_PLAYERS,
 S_TWO_ZONE,

 S_WIN,

 S_PULL_GOALIE,

 S_TEST_DEFENDER,
 S_TEST_OFFENDER,
 S_TESET_MIDDIE,
 S_TEST_CHASER) = range(NUM_STRATEGIES)


####
#### Information about the formations ####
####

NUM_FORMATIONS = 20
# dictionary of formations
FORMATIONS = dict(zip(range(NUM_FORMATIONS), ("INIT_FORMATION",
                                              "PENALTY_FORMATION",
                                              "NO_FIELD_PLAYERS",
                                              "ONE_FIELD",
                                              "ONE_FIELD_D",
                                              "TWO_FIELD",
                                              "NEUTRAL_TWO_FIELD",
                                              "TWO_DUB_D",
                                              "TWO_ZONE_D",
                                              "TWO_ZONE_O",
                                              "THREE_FIELD",
                                              "NEUTRAL_O_THREE_FIELD",
                                              "NEUTRAL_D_THREE_FIELD",
                                              "THREE_DUB_D",
                                              "FOUR_FIELD",
                                              "KICKOFF_FORMATION",
                                              "READY_FORMATION",
                                              "TEST_DEFEND",
                                              "TEST_OFFEND",
                                              "TEST_CHASE")))
# tuple of formations
(INIT_FORMATION,
 PENALTY_FORMATION,
 NO_FIELD_PLAYERS,
 ONE_FIELD,
 ONE_FIELD_D,
 TWO_FIELD,
 NEUTRAL_TWO_FIELD,
 TWO_DUB_D,
 TWO_ZONE_D,
 TWO_ZONE_O,
 THREE_FIELD,
 NEUTRAL_O_THREE_FIELD,
 NEUTRAL_D_THREE_FIELD,
 THREE_DUB_D,
 FOUR_FIELD,
 KICKOFF_FORMATION,
 READY_FORMATION,
 TEST_DEFEND,
 TEST_OFFEND,
 TEST_CHASE) = range(NUM_FORMATIONS)


####
#### Information about the roles ####
####

NUM_ROLES = 8
# dictionary of roles
ROLES = dict(zip(range(NUM_ROLES), ("INIT_ROLE",
                                    "PENALTY_ROLE",
                                    "CHASER",
                                    "MIDDIE",
                                    "OFFENDER",
                                    "DEFENDER",
                                    "GOALIE",
                                    "DEFENDER_DUB_D")))
# tuple of roles
(INIT_ROLE,
 PENALTY_ROLE,
 CHASER,
 MIDDIE,
 OFFENDER,
 DEFENDER,
 GOALIE,
 DEFENDER_DUB_D) = range(NUM_ROLES)


####
#### Information about the sub-roles ####
####

SUB_ROLE_SWITCH_BUFFER = 10.
# dictionary of subRoles
NUM_SUB_ROLES = 29
SUB_ROLES = dict(zip(range(NUM_SUB_ROLES), ("INIT_SUB_ROLE",
                                            "PENALTY_SUB_ROLE",

                                            #OFFENDER SUB ROLES 2-6
                                            "LEFT_WING",
                                            "RIGHT_WING",
                                            "STRIKER",
                                            "FORWARD",
                                            "PICKER",

                                            # MIDDIE SUB ROLES 7-9
                                            "DEFENSIVE_MIDDIE",
                                            "OFFENSIVE_MIDDIE",
                                            "DUB_D_MIDDIE",

                                            # DEFENDER SUB ROLES 10-15
                                            "STOPPER",
                                            "SWEEPER",
                                            "CENTER_BACK",
                                            "SWEEPER",
                                            "LEFT_DEEP_BACK",
                                            "RIGHT_DEEP_BACK",

                                            # CHASER SUB ROLES 16
                                            "CHASE_NORMAL",

                                            # GOALIE SUB ROLE 17-22
                                            "GOALIE_CENTER",
                                            "GOALIE_RIGHT",
                                            "GOALIE_LEFT",
                                            "GOALIE_SAVE",
                                            "GOALIE_CHASER",
                                            "GOALIE_KICKOFF",

                                            # KICKOFF SUB ROLES 23-24
                                            "KICKOFF_SWEEPER",
                                            "KICKOFF_STRIKER",

                                            # READY SUB ROLES 25-28
                                            "READY_GOALIE",
                                            "READY_CHASER",
                                            "READY_DEFENDER",
                                            "READY_OFFENDER" )))
# tuple of subRoles
(INIT_SUB_ROLE,
 PENALTY_SUB_ROLE,

 LEFT_WING,
 RIGHT_WING,
 STRIKER,
 FORWARD,
 PICKER,

 DEFENSIVE_MIDDIE,
 OFFENSIVE_MIDDIE,
 DUB_D_MIDDIE,

 STOPPER,
 SWEEPER,
 CENTER_BACK,
 SWEEPER,
 LEFT_DEEP_BACK,
 RIGHT_DEEP_BACK,

 CHASE_NORMAL,

 GOALIE_CENTER,
 GOALIE_RIGHT,
 GOALIE_LEFT,
 GOALIE_SAVE,
 GOALIE_CHASER,
 GOALIE_KICKOFF,

 KICKOFF_SWEEPER,
 KICKOFF_STRIKER,

 READY_GOALIE,
 READY_CHASER,
 READY_DEFENDER,
 READY_OFFENDER
) = range(NUM_SUB_ROLES)


## POSITION CONSTANTS ##

# READY_KICKOFF
# ____________________
# |__                |
# |  |             O |
# 0  |             __|
# |  |            /  |
# |G |      +    |  C+
# |  |            \__|
# 0  |  D            |
# |__|               |
# |__________________|
#
# Above is our ready position_0 when kicking-off. Can mirror for position_1
# Chaser: In optimal position to kick.
# Offender: On the wing ready to recieve a pass if chaser is crowded.
# Defender: Back in case something goes wrong. Also can potentially block opposing players' view of goalpost
# Goalie: At home.

CENTER_FIELD = Location(NogginConstants.CENTER_FIELD_X,
                        NogginConstants.CENTER_FIELD_Y)
"""DEFENDER"""
READY_KICKOFF_DEFENDER_X = (NogginConstants.CENTER_FIELD_X -
                            NogginConstants.GREEN_PAD_X) * 0.3 + 15
READY_KICKOFF_DEFENDER_0_Y = NogginConstants.LANDMARK_MY_GOAL_RIGHT_POST_Y
READY_KICKOFF_DEFENDER_1_Y = NogginConstants.LANDMARK_MY_GOAL_LEFT_POST_Y
"""OFFENDER"""
READY_KICKOFF_OFFENDER_X = (NogginConstants.CENTER_FIELD_X -
                            NogginConstants.CENTER_CIRCLE_RADIUS * 0.5)
READY_KICKOFF_OFFENDER_OFFSET = 165. # Can be as large as you want as long as robot can side-kick that distance
READY_KICKOFF_OFFENDER_0_Y = (NogginConstants.CENTER_FIELD_Y +
                              READY_KICKOFF_OFFENDER_OFFSET)
READY_KICKOFF_OFFENDER_1_Y = (NogginConstants.CENTER_FIELD_Y -
                              READY_KICKOFF_OFFENDER_OFFSET)
"""CHASER"""
# Use offset to leave room for chaser's feet/ room to position on the kick.
READY_KICKOFF_CHASER_OFFSET = NogginConstants.CENTER_CIRCLE_RADIUS * 0.5
READY_KICKOFF_CHASER_X = NogginConstants.CENTER_FIELD_X - READY_KICKOFF_CHASER_OFFSET
READY_KICKOFF_CHASER_Y = NogginConstants.CENTER_FIELD_Y - 15 # near center

# READY_NON_KICKOFF
# ____________________
# |__                |
# |  |               |
# 0  |             __|
# |  |      C     /  |
# |G |      +    |   +
# |  | D          \__|
# 0  |               |
# |__|     O         |
# |__________________|
#
# Above is our ready position_0 when not kicking-off. Can mirror for position_1
# Chaser: Blocking opponent chaser's view of one goalpost. Most forward position to ensure chasing responsibilities.
# Offener: On wing ready to charge up the field into game position. On opposite side of chaser to allow for easy passing.
# Defender: Blocking opponent chaser's view of other goalpost. Furthest back for defense.
# Goalie: At home.

# NOTE: Constants are to set up triangle blocking the line of sight to the goalposts

# Use the MAX_X to ensure we don't cross the line even with the tips of our feet.
READY_NON_KICKOFF_MAX_X = NogginConstants.LANDMARK_MY_FIELD_CROSS[1] - 25
"""DEFENDER"""
READY_NON_KICKOFF_DEFENDER_X = NogginConstants.GREEN_PAD_X + 80.
READY_NON_KICKOFF_DEFENDER_OFFSET = 51.3
READY_NON_KICKOFF_DEFENDER_0_Y = (NogginConstants.CENTER_FIELD_Y -
                                  READY_NON_KICKOFF_DEFENDER_OFFSET)
READY_NON_KICKOFF_DEFENDER_1_Y = (NogginConstants.CENTER_FIELD_Y +
                                  READY_NON_KICKOFF_DEFENDER_OFFSET)
"""OFFENDER"""
READY_NON_KICKOFF_OFFENDER_X = (NogginConstants.CENTER_FIELD_X -
                                NogginConstants.GREEN_PAD_X) * 0.5
READY_NON_KICKOFF_OFFENDER_OFFSET = 150. # How wide offender should be to run upfield
READY_NON_KICKOFF_OFFENDER_0_Y = (NogginConstants.CENTER_FIELD_Y -
                                  READY_NON_KICKOFF_OFFENDER_OFFSET)
READY_NON_KICKOFF_OFFENDER_1_Y = (NogginConstants.CENTER_FIELD_Y +
                                  READY_NON_KICKOFF_OFFENDER_OFFSET)
"""CHASER"""
READY_NON_KICKOFF_CHASER_X = READY_NON_KICKOFF_MAX_X
READY_NON_KICKOFF_CHASER_OFFSET = 28.
READY_NON_KICKOFF_CHASER_0_Y = (NogginConstants.CENTER_FIELD_Y +
                                READY_NON_KICKOFF_CHASER_OFFSET)
READY_NON_KICKOFF_CHASER_1_Y = (NogginConstants.CENTER_FIELD_Y -
                                READY_NON_KICKOFF_CHASER_OFFSET)


### KICK OFF POSITIONS (right after kickoff, rather)
#OFFENDER
KICKOFF_OFFENDER_X = NogginConstants.CENTER_FIELD_X
KICKOFF_OFFENDER_0_Y = READY_KICKOFF_OFFENDER_0_Y
KICKOFF_OFFENDER_1_Y = READY_KICKOFF_OFFENDER_1_Y


########### NORMAL POSITIONS ###########

###GOALIE
BALL_LOC_LIMIT = 220. # Dist at which we stop active localization and just track
#Elliptical positioning
GOAL_CENTER_X = NogginConstants.FIELD_WHITE_LEFT_SIDELINE_X
GOAL_CENTER_Y = NogginConstants.CENTER_FIELD_Y
ELLIPSE_X_SHIFT = 5. # Increase this to account for the goalposts
LARGE_ELLIPSE_HEIGHT = NogginConstants.GOALBOX_DEPTH * 0.65 #radius
LARGE_ELLIPSE_WIDTH = NogginConstants.CROSSBAR_CM_WIDTH * 0.5 #radius
LARGE_ELLIPSE_CENTER_Y = NogginConstants.CENTER_FIELD_Y
LARGE_ELLIPSE_CENTER_X = NogginConstants.FIELD_WHITE_LEFT_SIDELINE_X
ELLIPSE_POSITION_LIMIT = BALL_LOC_LIMIT
# Angle limits for moving about ellipse
ELLIPSE_ANGLE_MAX = 80
ELLIPSE_ANGLE_MIN = -80.0
RAD_TO_DEG = 180. / pi
DEG_TO_RAD = pi / 180.
BALL_FOCUS_UNCERT_THRESH = 100.

#Home positioning
GOALIE_HOME_X = NogginConstants.MY_GOALBOX_LEFT_X + 20 #LARGE_ELLIPSE_HEIGHT??
GOALIE_HOME_Y = NogginConstants.CENTER_FIELD_Y
GOALIE_RIGHT_X = NogginConstants.MY_GOALBOX_LEFT_X + 20
GOALIE_RIGHT_Y = NogginConstants.LANDMARK_MY_GOAL_RIGHT_POST_Y + 10
GOALIE_LEFT_X = NogginConstants.MY_GOALBOX_LEFT_X + 20
GOALIE_LEFT_Y = NogginConstants.LANDMARK_MY_GOAL_LEFT_POST_Y - 10

### Defender
"""
Defender has three subroles: Sweeper, Center Back, and Stopper.
A Sweeper's job is to sit in front of the goal box, but off to one side slightly,
            in order to provide more coverage of the goal. Ideally we would work
            with the goalie to ensure we don't block the ball from view. Useful
            when using saving goalie rather than aggressive goalie.
A Center Back will move in a large box in front of the goalbox in support of
            the chaser. They will stay between the ball and the goal.
A Stopper will move back and forth on a line behind the center circle,
            following the ball laterally (in the Y direction), but not going too
            far toward the edge of the field.
"""
DEFENDER_BALL_DIST = 100
SWEEPER_X_THRESH = NogginConstants.MY_GOALBOX_RIGHT_X + 90.
SWEEPER_X = NogginConstants.MY_GOALBOX_RIGHT_X + 25.
SWEEPER_Y = NogginConstants.CENTER_FIELD_Y
SWEEPER_Y_OFFSET = 20.
STOPPER_X = NogginConstants.LANDMARK_MY_FIELD_CROSS[0]
STOPPER_Y_OFFSET = 70.0
MIN_STOPPER_Y = NogginConstants.FIELD_WHITE_BOTTOM_SIDELINE_Y + STOPPER_Y_OFFSET
MAX_STOPPER_Y = NogginConstants.FIELD_WHITE_TOP_SIDELINE_Y - STOPPER_Y_OFFSET
CENTER_BACK_X_THRESH = NogginConstants.CENTER_FIELD_X
CENTER_BACK_MAX_X = STOPPER_X
CENTER_BACK_MIN_X = SWEEPER_X
MIN_CENTER_BACK_Y = NogginConstants.MY_GOALBOX_BOTTOM_Y
MAX_CENTER_BACK_Y = NogginConstants.MY_GOALBOX_TOP_Y

# Dub_d
DEEP_BACK_X = SWEEPER_X
RIGHT_DEEP_BACK_Y = NogginConstants.MY_GOALBOX_BOTTOM_Y - 40.
LEFT_DEEP_BACK_Y = NogginConstants.MY_GOALBOX_TOP_Y + 40.
RIGHT_DEEP_BACK_POS = Location(DEEP_BACK_X, RIGHT_DEEP_BACK_Y)
LEFT_DEEP_BACK_POS = Location(DEEP_BACK_X, LEFT_DEEP_BACK_Y)

### Offender
"""
Offender has four subroles: Forward, Striker, L/R wing, and Picker.
A Forward's job is to sit on the side of the field near the goal line on the
            opposite side of the field as the ball, both to provide support, and
            to catch a ball that was kicked out of the endline by the opposing team.
A Striker will sit on the opposite side of the field as the ball right outside the
            opponent's goalbox to provide a layup opportunity and to catch a shot
            that may have gone wide.
A L/R wing moves in a small line vertically (in the X) opposite the ball (ie. if
            the ball moves toward the opponent goal, we will move toward our own
            goal). This is to balance the offensive side of the field with the
            chaser. The Wing will also be on the opposite side of the field as
            ball.
A Picker hangs out dead center in front of the opponent goalbox waiting for a pass
            from a chaser who is in a coner. It's like a real coner-kick in soccer!
"""
FORWARD_X = (NogginConstants.CENTER_FIELD_X +
             NogginConstants.CENTER_CIRCLE_RADIUS + 30)
FORWARD_Y_OFFSET = 70.0
LEFT_FORWARD_Y = NogginConstants.FIELD_WHITE_TOP_SIDELINE_Y - FORWARD_Y_OFFSET
RIGHT_FORWARD_Y = NogginConstants.FIELD_WHITE_BOTTOM_SIDELINE_Y + FORWARD_Y_OFFSET
STRIKER_X_THRESH = (NogginConstants.CENTER_FIELD_X -
                    NogginConstants.CENTER_CIRCLE_RADIUS)
STRIKER_X = NogginConstants.OPP_GOALBOX_LEFT_X - 25
LEFT_STRIKER_Y = NogginConstants.LANDMARK_OPP_GOAL_LEFT_POST_Y
RIGHT_STRIKER_Y = NogginConstants.LANDMARK_OPP_GOAL_RIGHT_POST_Y
WING_X_THRESH = NogginConstants.LANDMARK_OPP_FIELD_CROSS[0]
WING_X_OFFSET = (NogginConstants.OPP_GOALBOX_LEFT_X - WING_X_THRESH)/3
WING_MIN_X = WING_X_THRESH + WING_X_OFFSET
WING_MAX_X = WING_MIN_X + WING_X_OFFSET
LEFT_WING_Y = NogginConstants.OPP_GOALBOX_TOP_Y
RIGHT_WING_Y = NogginConstants.OPP_GOALBOX_BOTTOM_Y
PICKER_X_THRESH = NogginConstants.OPP_GOALBOX_LEFT_X
PICKER_X = NogginConstants.OPP_GOALBOX_LEFT_X - 30.
PICKER_Y = NogginConstants.CENTER_FIELD_Y

### Middie
"""
Middie has two subroles: Defensive Middie and Offensive Middie.
A Defensive Middie will move laterally with the ball just behind the center line.
            This lets us provide a foward defense and lets us retrieve balls that
            we may have kicked off the opposing endline
An Offensive Middie's job is simply the opposite of a Defensive Middie.
Currently, an Offender will transition into an Offensive Middie when the ball is
            far down on our side, and the similarly the Defender will transition
            into a Defensive Middie, since we only have three field players.
"""
MIDDIE_X_OFFSET = 50.0
MIDDIE_Y_OFFSET = 45.0
MIN_MIDDIE_Y = NogginConstants.FIELD_WHITE_BOTTOM_SIDELINE_Y + MIDDIE_Y_OFFSET
MAX_MIDDIE_Y = NogginConstants.FIELD_WHITE_TOP_SIDELINE_Y - MIDDIE_Y_OFFSET
DEFENSIVE_MIDDIE_X = NogginConstants.CENTER_FIELD_X - MIDDIE_X_OFFSET
OFFENSIVE_MIDDIE_X = NogginConstants.CENTER_FIELD_X + MIDDIE_X_OFFSET
