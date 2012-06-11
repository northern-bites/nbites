from man.motion import HeadMoves

MAX_PAN_SPEED = 125.0              # deg/sec

TRACKER_FRAMES_ON_TRACK_THRESH = 3
TRACKER_FRAMES_OFF_REFIND_THRESH = 10

####
#### Active tracking constants
####
ACTIVE_TRACK_DIST = 60
STARE_TRACK_DIST = 50
BALL_ON_ACTIVE_PAN_THRESH = 60
STARE_TRACK_THRESH = 10
LOOK_FIELD_OBJ_FRAMES = 10

PAN_UP_PITCH_THRESH = 10

KICK_DICT = {"L_Side"          : HeadMoves.SIDEKICK_PAN_RIGHT,
             "R_Side"          : HeadMoves.SIDEKICK_PAN_LEFT,
             "L_Short_Side"    : HeadMoves.SIDEKICK_PAN_RIGHT,
             "R_Short_Side"    : HeadMoves.SIDEKICK_PAN_LEFT,
             "L_D_Straight"    : HeadMoves.FRONTKICK_PAN_LEFT,
             "R_D_Straight"    : HeadMoves.FRONTKICK_PAN_RIGHT,
             "L_ShortStraight" : HeadMoves.FRONTKICK_PAN_LEFT,
             "R_ShortStraight" : HeadMoves.FRONTKICK_PAN_RIGHT,
             "L_LongBack"      : HeadMoves.LOOK_DOWN,
             "R_LongBack"      : HeadMoves.LOOK_DOWN,
             "L_ShortBack"     : HeadMoves.LOOK_DOWN,
             "R_ShortBack"     : HeadMoves.LOOK_DOWN}
