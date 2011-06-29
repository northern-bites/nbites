#
# Goalie constants
#

from .. import NogginConstants as NogCon

# *****************
#     Position    *
# *****************

# Distance at which we use active localization
ACTIVE_LOC_THRESH = 150.

# *****************
#       Save      *
# *****************

# We dont use STRAFE RIGHT NOW
STRAFE_ONLY = True
STRAFE_SPEED = 0.3

#Save Waits
TEST_SAVE_WAIT = 50
TIME_ON_GROUND = 5
SQUAT_WAIT = 25
DIVE_WAIT = 150

#Save decision
HEAT_BUFFER = 10

#Saving limits
CENTER_SAVE_THRESH = 15
DONT_SAVE_LIMIT = 120


# *****************
#       Chase     *
# *****************

# The chasing box
CHASE_BUFFER = 10
CHASE_RIGHT_X_LIMIT = NogCon.MY_GOALBOX_RIGHT_X + 80
CHASE_UPPER_Y_LIMIT = NogCon.MY_GOALBOX_BOTTOM_Y - CHASE_BUFFER
CHASE_LOWER_Y_LIMIT = NogCon.MY_GOALBOX_TOP_Y + CHASE_BUFFER

# Sanity checks for chasing
CHASE_RELX_BUFFER = 200
STOP_CHASE_RELX_BUFFER = 160

# *****************
#       Other     *
# *****************

BALL_LOST = 30
CHANGE_THRESH = 3
