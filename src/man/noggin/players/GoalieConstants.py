#
# All of the constants used for the Goalie
# are kept here.
#

import noggin_constants as constants
from math import atan2, degrees

# Lizzie's:
INITIAL_X = constants.MY_GOALBOX_RIGHT_X
INITIAL_Y = constants.FIELD_WHITE_BOTTOM_SIDELINE_Y
INITIAL_ANGLE = degrees(atan2(
        INITIAL_X-constants.LANDMARK_MY_GOAL_RIGHT_POST_X,
        constants.LANDMARK_MY_GOAL_RIGHT_POST_Y-INITIAL_Y))

# Dani's stuff below:

# *****************
#     Position    *
# *****************

# Distance from the ball at which we use active localization
ACTIVE_LOC_THRESH = (constants.FIELD_WIDTH/ 4)

# Move up so that in center of box
MOVE_IN_KICKOFF = 18

# Stop tracking ball until uncertainty is down
UNCERT_TOO_HIGH = 100

# *****************
#       Save      *
# *****************

# NOTE: A lot of saving decisions right now are
# based on extensive testing for values that seemed
# to offer good results in terms of saving.  When
# an improved ball system is put in place you will
# not need a lot of the weird numbers.

# We dont use STRAFE RIGHT NOW
STRAFE_ONLY = True
STRAFE_SPEED = 0.3

#Save Waits
TEST_SAVE_WAIT = 50
TIME_ON_GROUND = 5
SQUAT_WAIT = 25
DIVE_WAIT = 150

#Save decision
HEAT_BUFFER = 10 # Based on testing

# Distance from center that can squat save
CENTER_SAVE_THRESH = 15

# Too far away to save distance
DONT_SAVE_LIMIT = 250

# Needs to be accelerating my direction
ACCEL_SAVE_THRESH = -20

# Accel value below which "Not Moving"
BALL_NO_MOVEMENT = 0.5

# Should save thresholds
HEAT_LOW = 5
VEL_HIGH = -40

# Number of frames before leave saving
OUT_OF_SAVE = 30


# *****************
#       Chase     *
# *****************
# Right now chasing is turned down to a conservative
# level because the goalie keeps getting lost

# The chasing box
CHASE_BUFFER = 10
CHASE_RIGHT_X_LIMIT = constants.MY_GOALBOX_RIGHT_X + 80

# Sanity checks for chasing
CHASE_RELX_BUFFER = CHASE_RIGHT_X_LIMIT
STOP_CHASE_RELX_BUFFER = CHASE_RIGHT_X_LIMIT

# *****************
#       Other     *
# *****************

BALL_LOST = 80

# Goalie changes after 3 true calls to
# a new state.  It is so fast because
# we want a goalie that is quick to react
# when necessary
CHANGE_THRESH = 3
