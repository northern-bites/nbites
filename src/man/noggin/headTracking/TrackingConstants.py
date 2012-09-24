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

KICK_DICT = {"L_Side"           : 'lookRightFixedPitch',
             "R_Side"           : 'lookLeftFixedPitch',
             "L_Short_Side"     : 'lookRightFixedPitch',
             "R_Short_Side"     : 'lookLeftFixedPitch',
             "L_Straight"       : 'afterKickScan',
             "R_Straight"       : 'afterKickScan',
             "L_Short_Straight" : 'afterKickScan',
             "R_Short_Straight" : 'afterKickScan',
             "L_Quick_Straight" : 'afterKickScan',
             "R_Quick_Straight" : 'afterKickScan',
             "L_Big_Straight"   : 'afterKickScan',
             "R_Big_Straight"   : 'afterKickScan',
             "L_Long_Back"      : 'afterKickScan',
             "R_Long_Back"      : 'afterKickScan',
             "L_Short_Back"     : 'afterKickScan',
             "R_Short_Back"     : 'afterKickScan'}
