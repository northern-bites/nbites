#
# Goalie constants
#

import noggin_constants as NogCon

# *****************
#     Position    *
# *****************

# Distance at which we use active localization
ACTIVE_LOC_THRESH = 150.

# Move up so that in center of box
MOVE_IN_KICKOFF = 18

# Stop tracking ball until uncertainty is down
UNCERT_TOO_HIGH = 100

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
DONT_SAVE_LIMIT = 250
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

# The chasing box
CHASE_BUFFER = 10
CHASE_RIGHT_X_LIMIT = NogCon.MY_GOALBOX_RIGHT_X + 80

# Sanity checks for chasing
CHASE_RELX_BUFFER = 80
STOP_CHASE_RELX_BUFFER = 80

# *****************
#       Other     *
# *****************

BALL_LOST = 80
CHANGE_THRESH = 3
