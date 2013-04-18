import HeadMoves

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

KICK_DICT = {"L_Side"           : HeadMoves.FIXED_PITCH_LOOK_RIGHT,
             "R_Side"           : HeadMoves.FIXED_PITCH_LOOK_LEFT,
             "L_Short_Side"     : HeadMoves.FIXED_PITCH_LOOK_RIGHT,
             "R_Short_Side"     : HeadMoves.FIXED_PITCH_LOOK_LEFT,
             "L_Straight"       : HeadMoves.FIXED_PITCH_LOOK_STRAIGHT,
             "R_Straight"       : HeadMoves.FIXED_PITCH_LOOK_STRAIGHT,
             "L_Short_Straight" : HeadMoves.FIXED_PITCH_LOOK_STRAIGHT,
             "R_Short_Straight" : HeadMoves.FIXED_PITCH_LOOK_STRAIGHT,
             "L_Quick_Straight" : HeadMoves.FIXED_PITCH_LOOK_STRAIGHT,
             "R_Quick_Straight" : HeadMoves.FIXED_PITCH_LOOK_STRAIGHT,
             "L_Big_Straight"   : HeadMoves.FIXED_PITCH_LOOK_STRAIGHT,
             "R_Big_Straight"   : HeadMoves.FIXED_PITCH_LOOK_STRAIGHT,
             "L_Long_Back"      : HeadMoves.FIXED_PITCH_LOOK_STRAIGHT,
             "R_Long_Back"      : HeadMoves.FIXED_PITCH_LOOK_STRAIGHT,
             "L_Short_Back"     : HeadMoves.FIXED_PITCH_LOOK_STRAIGHT,
             "R_Short_Back"     : HeadMoves.FIXED_PITCH_LOOK_STRAIGHT}
# TODO: Test and improve these pans.

