from man.motion import HeadMoves

MAX_PAN_SPEED = 125.0              # deg/sec

TRACKER_FRAMES_ON_TRACK_THRESH = 1
TRACKER_FRAMES_OFF_REFIND_THRESH = 5

ACTIVE_LOC_STARE_THRESH = 45
ACTIVE_LOC_STARE_GOALIE_THRESH = 25
ACTIVE_LOC_OFF_REFIND_THRESH = 40

LOOK_TO_TIME_TO_FIND = 45

TRACKER_FRAMES_SEARCH_THRESH = 15 # ** # debugging values
TRACKER_FRAMES_STARE_THRESH = 5 # ** # debugging values
TRACKER_BALL_STARE_THRESH = 20 # ** # debugging values

####
#### Active tracking constants
####
ACTIVE_TRACK_DIST = 70
STARE_TRACK_DIST = 40
BALL_ON_ACTIVE_PAN_THRESH = 30
STARE_TRACK_THRESH = 10
LOOK_FIELD_OBJ_FRAMES = 10

NUM_ACTIVE_PANS = 2
(PAN_LEFT,
 PAN_RIGHT) = range(NUM_ACTIVE_PANS)

PAN_UP_PITCH_THRESH = 10

NUM_LOOK_DIRS = 4
(LOOK_LEFT,
 LOOK_UP,
 LOOK_RIGHT,
 LOOK_DOWN) = range(NUM_LOOK_DIRS)

KICK_DICT = {"L_Side"          : HeadMoves.SIDEKICK_PAN_RIGHT,
             "R_Side"          : HeadMoves.SIDEKICK_PAN_LEFT,
             "L_D_Straight"    : HeadMoves.FRONTKICK_PAN_LEFT,
             "R_D_Straight"    : HeadMoves.FRONTKICK_PAN_RIGHT,
             "L_ShortStraight" : HeadMoves.FRONTKICK_PAN_LEFT,
             "R_ShortStraight" : HeadMoves.FRONTKICK_PAN_RIGHT,
             "L_LongBack"      : HeadMoves.LOOK_DOWN,
             "R_LongBack"      : HeadMoves.LOOK_DOWN,
             "L_ShortBack"     : HeadMoves.LOOK_DOWN,
             "R_ShortBack"     : HeadMoves.LOOK_DOWN}
