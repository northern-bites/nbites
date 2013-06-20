"""
File to hold all of the cross field constants for the cooperative behavior system
"""
import noggin_constants as NogginConstants
from objects import RobotLocation

# Test switches to force one role to always be given out
TEST_DEFENDER = False
TEST_OFFENDER = False
TEST_MIDDIE = False
TEST_CHASER = False

# Print information as to how the chaser is determined
DEBUG_DET_CHASER = False
DEBUG_DET_SUPPORTER = False

USE_DUB_D = True
USE_FANCY_GOALIE = False

KICKOFF_FORMATION_TIME = 10 # Length of time to spend in the kickoff play

####
#### Role Switching / Tie Breaking ####
####
# The following constants are in seconds
CALL_OFF_THRESH = 0.5             # how likely it is to be chaser
STOP_CALLING_THRESH = 2.0         # how likely you are to ignore teammates ideas.

GOALIE_CHASER_COUNT_THRESH = 25      # how long we wait before goalie calls us off.

# Constants for building playbook table
TABLE_GRID_SIZE = 100
TABLE_GRID_WIDTH = int(NogginConstants.FIELD_WHITE_WIDTH / TABLE_GRID_SIZE) + 1
TABLE_GRID_HEIGHT = int(NogginConstants.FIELD_WHITE_HEIGHT / TABLE_GRID_SIZE) + 1

WALKING_SPEED = 20 # cm/second
TURNING_RATE = 60  # degrees/second

####
#### Information about the roles ####
####

NUM_ROLES = 7
# dictionary of roles
ROLES = dict(zip(range(NUM_ROLES), ("CHASER",
                                    "DEFENDER",
                                    "OFFENDER",
                                    "MIDDIE",
                                    "GOALIE",
                                    "PENALTY_ROLE",
                                    "INIT_ROLE")))

# tuple of roles DO NOT CHANGE THIS ORDER
(CHASER,
 DEFENDER,
 OFFENDER,
 MIDDIE,
 GOALIE,
 PENALTY_ROLE,
 INIT_ROLE) = range(NUM_ROLES)


## POSITION CONSTANTS ##

# READY_KICKOFF
#  ___________________________
# |                           |
# |                           |
# |                           |
# |                    M      |
# |__                         |
# |  |                        |
# 0  |   D                   _|
# |  |                      / |
# |G |      +              | C|
# |  |                      \_|
# 0  |                        |
# |__|                        |
# |                           |
# |                    O      |
# |                           |
# |                           |
# |___________________________|
#
#
# Have Middie and Offender mirrored, because they might both decide to
#  be offenders given slight variation of the ball (since there is a
#  grid line at midfield)

"""DEFENDER"""
READY_O_DEFENDER_X = NogginConstants.LANDMARK_BLUE_GOAL_CROSS_X - 30
READY_O_DEFENDER_Y = NogginConstants.LANDMARK_MY_GOAL_LEFT_POST_Y
READY_O_DEFENDER_LOCATION = RobotLocation(READY_O_DEFENDER_X,
                                          READY_O_DEFENDER_Y,
                                          -10)
"""MIDDIE"""
READY_O_MIDDIE_X = (NogginConstants.CENTER_FIELD_X -
                    NogginConstants.CENTER_CIRCLE_RADIUS * 2)
READY_O_MIDDIE_Y = (NogginConstants.CENTER_FIELD_Y +
                    NogginConstants.CENTER_FIELD_Y * 0.5)
READY_O_MIDDIE_LOCATION = RobotLocation(READY_O_DEFENDER_X,
                                        READY_O_DEFENDER_Y,
                                        -30) # Face toward opponent cross
"""OFFENDER"""
READY_O_OFFENDER_X = READY_O_MIDDIE_X
READY_O_OFFENDER_Y = (NogginConstants.CENTER_FIELD_Y -
                      NogginConstants.CENTER_FIELD_Y * 0.5)
READY_O_OFFENDER_LOCATION = RobotLocation(READY_O_OFFENDER_X,
                                          READY_O_OFFENDER_Y,
                                          30)
"""CHASER"""
# Use offset to leave room for chaser's feet/ room to position on the kick.
READY_O_CHASER_X = (NogginConstants.CENTER_FIELD_X -
                    NogginConstants.CENTER_CIRCLE_RADIUS * 0.5)
READY_O_CHASER_Y = NogginConstants.CENTER_FIELD_Y
READY_O_CHASER_LOCATION = RobotLocation(READY_O_CHASER_X,
                                        READY_O_CHASER_Y,
                                        0)
# READY_NON_KICKOFF
#  ___________________________
# |                           |
# |                           |
# |                           |
# |                           |
# |__                         |
# |  |             M          |
# 0  |                       _|
# |  |   D                  / |
# |G |      +          C   |  |
# |  |                      \_|
# 0  |                        |
# |__|             O          |
# |                           |
# |                           |
# |                           |
# |                           |
# |___________________________|
#
#
# Have Middie and Offender mirrored, because they might both decide to
#  be offenders given slight variation of the ball (since there is a
#  grid line at midfield)

"""DEFENDER"""
READY_D_DEFENDER_X = NogginConstants.LANDMARK_BLUE_GOAL_CROSS_X - 30
READY_D_DEFENDER_Y = NogginConstants.LANDMARK_MY_GOAL_LEFT_POST_Y - 10
READY_D_DEFENDER_LOCATION = RobotLocation(READY_D_DEFENDER_X,
                                          READY_D_DEFENDER_Y,
                                          -10)
"""MIDDIE"""
READY_D_MIDDIE_X = (NogginConstants.CENTER_FIELD_X -
                    NogginConstants.CENTER_CIRCLE_RADIUS * 2)
READY_D_MIDDIE_Y = (NogginConstants.MY_GOALBOX_TOP_Y)
READY_D_MIDDIE_LOCATION = RobotLocation(READY_D_DEFENDER_X,
                                        READY_D_DEFENDER_Y,
                                        -30) # Face toward opponent cross
"""OFFENDER"""
READY_D_OFFENDER_X = READY_D_MIDDIE_X
READY_D_OFFENDER_Y = (NogginConstants.MY_GOALBOX_BOTTOM_Y)
READY_D_OFFENDER_LOCATION = RobotLocation(READY_D_OFFENDER_X,
                                          READY_D_OFFENDER_Y,
                                          30)
"""CHASER"""
# Use offset to leave room for chaser's feet/ room to position on the kick.
READY_D_CHASER_X = (NogginConstants.CENTER_FIELD_X -
                    NogginConstants.CENTER_CIRCLE_RADIUS * 1.5)
READY_D_CHASER_Y = NogginConstants.CENTER_FIELD_Y
READY_D_CHASER_LOCATION = RobotLocation(READY_D_CHASER_X,
                                        READY_D_CHASER_Y,
                                        0)

# Goalie Home positioning
GOALIE_HOME_X = NogginConstants.MY_GOALBOX_LEFT_X + 30
GOALIE_HOME_Y = NogginConstants.CENTER_FIELD_Y
