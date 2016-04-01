import HeadMoves

USE_BOUNCE_TRACKING = False

MAX_PAN_SPEED = 125.0              # deg/sec

TRACKER_FRAMES_ON_TRACK_THRESH = 2
TRACKER_FRAMES_OFF_REFIND_THRESH = 10

####
#### Active tracking constants
####
ACTIVE_TRACK_DIST = 60
STARE_TRACK_DIST = 50
BALL_ON_ACTIVE_PAN_THRESH = 60
STARE_TRACK_THRESH = 10
LOOK_FIELD_OBJ_FRAMES = 10

#sharedball constants
LOOKTIME = 40
PANTIME = 1

# Calibrate Constants
TIME_OF_PAN = 0.2
NUMBER_OF_PANS = 7
TIME_PER_LOG = 1.0



FIXED_PITCH_VALUE = 20

KICK_DICT = {"L_Side"               : HeadMoves.FIXED_PITCH_LOOK_RIGHT,
             "R_Side"               : HeadMoves.FIXED_PITCH_LOOK_LEFT,
             "L_Short_Side"         : HeadMoves.FIXED_PITCH_LOOK_RIGHT,
             "R_Short_Side"         : HeadMoves.FIXED_PITCH_LOOK_LEFT,
             "L_Straight"           : HeadMoves.FIXED_PITCH_LOOK_STRAIGHT,
             "R_Straight"           : HeadMoves.FIXED_PITCH_LOOK_STRAIGHT,
             "L_Short_Straight"     : HeadMoves.FIXED_PITCH_LOOK_STRAIGHT,
             "R_Short_Straight"     : HeadMoves.FIXED_PITCH_LOOK_STRAIGHT,
             "L_Quick_Straight"     : HeadMoves.FIXED_PITCH_LOOK_STRAIGHT,
             "R_Quick_Straight"     : HeadMoves.FIXED_PITCH_LOOK_STRAIGHT,
             "L_Big_Straight"       : HeadMoves.FIXED_PITCH_LOOK_STRAIGHT,
             "R_Big_Straight"       : HeadMoves.FIXED_PITCH_LOOK_STRAIGHT,
             "L_Long_Back"          : HeadMoves.FIXED_PITCH_LOOK_STRAIGHT,
             "R_Long_Back"          : HeadMoves.FIXED_PITCH_LOOK_STRAIGHT,
             "L_Short_Back"         : HeadMoves.FIXED_PITCH_LOOK_STRAIGHT,
             "R_Short_Back"         : HeadMoves.FIXED_PITCH_LOOK_STRAIGHT,
             "Zellvarro_Left_Kick"  : HeadMoves.FIXED_PITCH_LOOK_STRAIGHT,
             "Zellvarro_Right_Kick" : HeadMoves.FIXED_PITCH_LOOK_STRAIGHT,
             "Left_Medium_Straight_Kick" : HeadMoves.FIXED_PITCH_LOOK_STRAIGHT,
             "Right_Medium_Straight_Kick" : HeadMoves.FIXED_PITCH_LOOK_STRAIGHT,

             "M_Left_Straight"      : HeadMoves.FIXED_PITCH_LOOK_STRAIGHT,
             "M_Right_Straight"     : HeadMoves.FIXED_PITCH_LOOK_STRAIGHT,
             "M_Left_Chip_Shot"     : HeadMoves.FIXED_PITCH_LOOK_LEAST_RIGHT,
             "M_Right_Chip_Shot"    : HeadMoves.FIXED_PITCH_LOOK_LEAST_LEFT,
             "M_Left_Side"          : HeadMoves.FIXED_PITCH_LOOK_RIGHT,
             "M_Right_Side"         : HeadMoves.FIXED_PITCH_LOOK_LEFT,
             "BH_L_FORWARD"         : HeadMoves.FIXED_PITCH_LOOK_STRAIGHT,
             "BH_R_FORWARD"         : HeadMoves.FIXED_PITCH_LOOK_STRAIGHT
             }
# TODO: Test and improve these pans.

OBSTACLE_DICT = { 1 : HeadMoves.FIXED_PITCH_LOOK_STRAIGHT,
                  2 : HeadMoves.FIXED_PITCH_LOOK_LESS_RIGHT,
                  3 : HeadMoves.FIXED_PITCH_LOOK_RIGHT,
                  4 : HeadMoves.FIXED_PITCH_LOOK_LESS_RIGHT,
                  5 : HeadMoves.FIXED_PITCH_LOOK_STRAIGHT,
                  6 : HeadMoves.FIXED_PITCH_LOOK_RIGHT,
                  7 : HeadMoves.FIXED_PITCH_LOOK_LESS_LEFT,
                  8 : HeadMoves.FIXED_PITCH_LOOK_LESS_LEFT
            }
