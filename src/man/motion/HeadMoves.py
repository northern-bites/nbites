# Constants file to store all head moves, including pans and simple moves

# TODO: update all pans to speeds that avoid motion blur. @summer 2012

import StiffnessModes as stiff

HEAD_MOVE_LENGTH = 4

ZERO_HEADS = (((0.0,0.0),1.0,0, stiff.LOW_HEAD_STIFFNESSES),)

OFF_HEADS = (((0.0,0.0),1.0,1, stiff.NO_HEAD_STIFFNESSES),)

NEUT_HEADS = (((0.,20.),2.0,0, stiff.LOW_HEAD_STIFFNESSES),)

LOOK_DOWN = ( ((0.0,30.0), 0.8, 1, stiff.LOW_HEAD_STIFFNESSES), )

LOOK_UP =  ( ((0.0,-30.0), 0.8, 1, stiff.LOW_HEAD_STIFFNESSES), )

LOOK_RIGHT = ( ((-70.0,25.0), 0.8, 1, stiff.LOW_HEAD_STIFFNESSES), )

LOOK_LEFT =  ( ((70.0,25.0), 0.8, 1, stiff.LOW_HEAD_STIFFNESSES), )

LOOK_UP_LEFT = (((50.0,-25.0), 0.8, 1, stiff.LOW_HEAD_STIFFNESSES), )

LOOK_UP_RIGHT = (((-50.0,-25.0), 0.8, 1, stiff.LOW_HEAD_STIFFNESSES), )

LOOK_FWD_UP_LEFT = (((30.0,-25.0), 0.8, 1, stiff.LOW_HEAD_STIFFNESSES), )

LOOK_FWD_UP_RIGHT = (((-30.0,-25.0), 0.8, 1, stiff.LOW_HEAD_STIFFNESSES), )

PENALIZED_HEADS = (((0.0,25.0),0.5,0,stiff.LOW_HEAD_STIFFNESSES),)

FIND_BALL_HEADS_LEFT = (((45.,-10.),0.8,0, stiff.LOW_HEAD_STIFFNESSES),
                        ((45.,20.),0.3,1, stiff.LOW_HEAD_STIFFNESSES))

FIND_BALL_HEADS_RIGHT =  (((-45.,-10.),0.8,0, stiff.LOW_HEAD_STIFFNESSES),
                          ((-45.,20.),0.3,1, stiff.LOW_HEAD_STIFFNESSES))

# #OVER_RIGHT_SHOULDER = ((PAN_RIGHT_SHOULDER_HEADS,
#                         0.5,1,stiff.LOW_HEAD_STIFFNESSES),)

# OVER_LEFT_SHOULDER = ((PAN_LEFT_SHOULDER_HEADS,
#                        0.5,1,stiff.LOW_HEAD_STIFFNESSES),)

# Distance that can be seen at a certain head pitch
# | degree | close | far |
# |     20 |    15 |  53 |
# |     -5 |    33 | 104 |
# |    -30 |   104 | inf |
#HEAD SCANS


#####################BASIC PANS##########################
LOC_PANS = (
    (( 65.0, 10.0),1.5, 1, stiff.LOW_HEAD_STIFFNESSES),
    ((65.,-20.),1.0,  1, stiff.LOW_HEAD_STIFFNESSES),
    ((-65.,-20.),2.5, 1, stiff.LOW_HEAD_STIFFNESSES),
    ((-65.0, 10.0) ,1., 1, stiff.LOW_HEAD_STIFFNESSES),
    (( 0.0, 10.0),1.5,  1, stiff.LOW_HEAD_STIFFNESSES),)

QUICK_PANS = (
    ((  0.0,-40.0),.3,  1, stiff.LOW_HEAD_STIFFNESSES),
    (( 30.0,-25.0),.3,  1, stiff.LOW_HEAD_STIFFNESSES),
    (( 65.0,-20.0),.4,  1, stiff.LOW_HEAD_STIFFNESSES),
    (( 30.0,-25.0),.4,  1, stiff.LOW_HEAD_STIFFNESSES),
    ((  0.0,-40.0),.3,  1, stiff.LOW_HEAD_STIFFNESSES),
    ((-30.0,-25.0),.3,  1, stiff.LOW_HEAD_STIFFNESSES),
    ((-65.0,-20.0),.4,  1, stiff.LOW_HEAD_STIFFNESSES),
    ((-30.0,-25.0),.4,  1, stiff.LOW_HEAD_STIFFNESSES))

######################PHOTO PANS########################

#pans full field inf to about 2/3m
HIGH_SNAPSHOT_PAN = (
    ((-65.0, -25.0), 0.6, 1, stiff.LOW_HEAD_STIFFNESSES),
    ((0.0, -35.0), 0.6, 1, stiff.LOW_HEAD_STIFFNESSES),
    ((65.0, -25.0), 0.6, 1, stiff.LOW_HEAD_STIFFNESSES),
    ((0.0, -35.0), 0.6, 1, stiff.LOW_HEAD_STIFFNESSES))

#pans full field inf to about .15m
HIGH_MID_SNAPSHOT_PAN = (
    ((-65.0, -25.0), 0.6, 1, stiff.LOW_HEAD_STIFFNESSES),
    ((0.0, -35.0), 0.6, 1, stiff.LOW_HEAD_STIFFNESSES),
    ((65.0, -25.0), 0.6, 1, stiff.LOW_HEAD_STIFFNESSES),
    ((65.0, -10.0), 0.4, 1, stiff.LOW_HEAD_STIFFNESSES),
    ((-65.0, -10.0), 1.2, 1, stiff.LOW_HEAD_STIFFNESSES))

#pans full field about 1m to about .15m
MID_SNAPSHOT_PAN = (
    ((-65.0, -10.0), 0.4, 1, stiff.LOW_HEAD_STIFFNESSES),
    ((65.0, -10.0), 1.2, 1, stiff.LOW_HEAD_STIFFNESSES),
    ((65.0, 0.0), 0.4, 1, stiff.LOW_HEAD_STIFFNESSES),
    ((-65.0, 0.0), 1.2, 1, stiff.LOW_HEAD_STIFFNESSES))

#pans full field about 2/3m to 0m
MID_LOW_SNAPSHOT_PAN = (
    ((-65.0, 0.0), 0.5, 1, stiff.LOW_HEAD_STIFFNESSES),
    ((65.0, 0.0), 1.2, 1, stiff.LOW_HEAD_STIFFNESSES),
    ((65.0, 20.0), 0.5, 1, stiff.LOW_HEAD_STIFFNESSES),
    ((-65.0, 20.0), 1.2, 1, stiff.LOW_HEAD_STIFFNESSES))

#pans full field about .5m to 0m
LOW_SNAPSHOT_PAN = (
    ((-65.0, 5.0), 0.4, 1, stiff.LOW_HEAD_STIFFNESSES),
    ((65.0, 5.0), 1.2, 1, stiff.LOW_HEAD_STIFFNESSES),
    ((65.0, 20.0), 0.4, 1, stiff.LOW_HEAD_STIFFNESSES),
    ((-65.0, 20.0), 1.2, 1, stiff.LOW_HEAD_STIFFNESSES))

#pans full field inf to 0m
COMB_SNAPSHOT_PAN = (
    ((-65.0, -25.0), 0.6, 1, stiff.LOW_HEAD_STIFFNESSES),
    ((0.0, -35.0), 0.6, 1, stiff.LOW_HEAD_STIFFNESSES),
    ((65.0, -25.0), 0.6, 1, stiff.LOW_HEAD_STIFFNESSES),
    ((65.0, -10.0), 0.4, 1, stiff.LOW_HEAD_STIFFNESSES),
    ((-65.0, -10.0), 1.2, 1, stiff.LOW_HEAD_STIFFNESSES),
    ((-65.0, 0.0), 0.4, 1, stiff.LOW_HEAD_STIFFNESSES),
    ((65.0, 0.0), 1.2, 1, stiff.LOW_HEAD_STIFFNESSES),
    ((65.0, 20.0), 0.4, 1, stiff.LOW_HEAD_STIFFNESSES),
    ((-65.0, 20.0), 1.2, 1, stiff.LOW_HEAD_STIFFNESSES))

#pans in front of robot inf to about 2/3m
FORWARD_HIGH_PAN = (
    ((-30.0, -30.0), 0.4, 1, stiff.LOW_HEAD_STIFFNESSES),
    ((0.0, -35.0), 0.4, 1, stiff.LOW_HEAD_STIFFNESSES),
    ((30.0, -30.0), 0.4, 1, stiff.LOW_HEAD_STIFFNESSES),
    ((0.0, -35.0), 0.4, 1, stiff.LOW_HEAD_STIFFNESSES))

#pans in front of robot inf to about .15m
FORWARD_HIGH_MID_PAN = (
    ((-30.0, -30.0), 0.3, 1, stiff.LOW_HEAD_STIFFNESSES),
    ((0.0, -35.0), 0.4, 1, stiff.LOW_HEAD_STIFFNESSES),
    ((30.0, -30.0), 0.4, 1, stiff.LOW_HEAD_STIFFNESSES),
    ((30.0, -10.0), 0.3, 1, stiff.LOW_HEAD_STIFFNESSES),
    ((-30.0, -10.0), .8, 1, stiff.LOW_HEAD_STIFFNESSES))

#pans in front of robot about 1m to about .15m
FORWARD_MID_PAN = (
    ((-30.0, -10.0), 0.3, 1, stiff.LOW_HEAD_STIFFNESSES),
    ((30.0, -10.0), .8, 1, stiff.LOW_HEAD_STIFFNESSES),
    ((30.0, 0.0), 0.3, 1, stiff.LOW_HEAD_STIFFNESSES),
    ((-30.0, 0.0), 0.8, 1, stiff.LOW_HEAD_STIFFNESSES))

#pans in front of robot about 2/3m to 0m
FORWARD_MID_LOW_PAN = (
    ((-30.0, 0.0), 0.3, 1, stiff.LOW_HEAD_STIFFNESSES),
    ((30.0, 0.0), .8, 1, stiff.LOW_HEAD_STIFFNESSES),
    ((30.0, 20.0), 0.3, 1, stiff.LOW_HEAD_STIFFNESSES),
    ((-30.0, 20.0), 0.8, 1, stiff.LOW_HEAD_STIFFNESSES))

#pans in front of robot about .5m to 0m
FORWARD_LOW_PAN = (
    ((-30.0, 5.0), 0.3, 1, stiff.LOW_HEAD_STIFFNESSES),
    ((30.0, 5.0), 0.8, 1, stiff.LOW_HEAD_STIFFNESSES),
    ((30.0, 20.0), 0.3, 1, stiff.LOW_HEAD_STIFFNESSES),
    ((-30.0, 20.0), 0.8, 1, stiff.LOW_HEAD_STIFFNESSES))

#pans in front of robot inf to 0m
FORWARD_COMB_PAN = (
    ((-30.0, -30.0), 0.6, 1, stiff.LOW_HEAD_STIFFNESSES),
    ((0.0, -35.0), 0.4, 1, stiff.LOW_HEAD_STIFFNESSES),
    ((30.0, -30.0), 0.4, 1, stiff.LOW_HEAD_STIFFNESSES),
    ((30.0, -10.0), 0.3, 1, stiff.LOW_HEAD_STIFFNESSES),
    ((-30.0, -10.0), 0.8, 1, stiff.LOW_HEAD_STIFFNESSES),
    ((-30.0, 0.0), 0.3, 1, stiff.LOW_HEAD_STIFFNESSES),
    ((30.0, 0.0), 0.8, 1, stiff.LOW_HEAD_STIFFNESSES),
    ((30.0, 20.0), 0.3, 1, stiff.LOW_HEAD_STIFFNESSES),
    ((-30.0, 20.0), 0.8, 1, stiff.LOW_HEAD_STIFFNESSES))

########################SCANS###########################

FULL_SCAN_BALL = (
    ((-65.,20.),0.6,1, stiff.LOW_HEAD_STIFFNESSES),
    ((65.,20.),1.2, 1, stiff.LOW_HEAD_STIFFNESSES),
    ((65.0, -5.0) ,0.2, 1, stiff.LOW_HEAD_STIFFNESSES),
    (( -118.0, -5.0),1.7,  1, stiff.LOW_HEAD_STIFFNESSES),
    (( -65.0, -25.0),0.4,  1, stiff.LOW_HEAD_STIFFNESSES),
    (( 0.0, -35.0),0.6,  1, stiff.LOW_HEAD_STIFFNESSES),
    (( 65.0, -25.0),0.6, 1, stiff.LOW_HEAD_STIFFNESSES),
    ((118.,-5.),0.4,  1, stiff.LOW_HEAD_STIFFNESSES),
    ((65.,-5.),.5, 1, stiff.LOW_HEAD_STIFFNESSES),)

HIGH_SCAN_CLOSE_BOUND = 104
HIGH_WIDE_SCAN_BALL = (
    (( -65.0, -25.0),0.4,  1, stiff.LOW_HEAD_STIFFNESSES),
    (( 0.0, -35.0),0.6,  1, stiff.LOW_HEAD_STIFFNESSES),
    (( 65.0, -25.0),0.6, 1, stiff.LOW_HEAD_STIFFNESSES),
    (( 118.0, -5.0), .3, 1, stiff.LOW_HEAD_STIFFNESSES),
    (( -118.0, -5.0),1.7,  1, stiff.LOW_HEAD_STIFFNESSES),
    ((-65.,20.),0.3,1, stiff.LOW_HEAD_STIFFNESSES),
    ((65.,20.),1.2, 1, stiff.LOW_HEAD_STIFFNESSES),)

HIGH_SCAN_BALL = (
    (( -65.0, -25.0),0.6,  1, stiff.LOW_HEAD_STIFFNESSES),
    (( 0.0, -35.0),0.5, 1, stiff.LOW_HEAD_STIFFNESSES),
    (( 65.0, -25.0),0.5, 1, stiff.LOW_HEAD_STIFFNESSES),
    ((65.,-5.),0.2,  1, stiff.LOW_HEAD_STIFFNESSES),
    ((-65.,-5.),1.0, 1, stiff.LOW_HEAD_STIFFNESSES),
    ((-65.,20.),0.2,1, stiff.LOW_HEAD_STIFFNESSES),
    ((65.,20.),1.0, 1, stiff.LOW_HEAD_STIFFNESSES),
    ((65.0, -5.0) ,0.2, 1, stiff.LOW_HEAD_STIFFNESSES),
    (( -65.0, -5.0),1.0,  1, stiff.LOW_HEAD_STIFFNESSES),)

MID_SCAN_CLOSE_BOUND = 40
MID_SCAN_FAR_BOUND = 104

MID_DOWN_WIDE_SCAN_BALL = (
    ((65.0, -5.0) ,0.4, 1, stiff.LOW_HEAD_STIFFNESSES),
    (( 118.0, -5.0),0.3,  1, stiff.LOW_HEAD_STIFFNESSES),
    (( -118.0, -5.0),1.7,  1, stiff.LOW_HEAD_STIFFNESSES),
    (( -65.0, -25.0),0.3,  1, stiff.LOW_HEAD_STIFFNESSES),
    (( 0.0, -35.0),0.6,  1, stiff.LOW_HEAD_STIFFNESSES),
    (( 65.0, -25.0),0.6, 1, stiff.LOW_HEAD_STIFFNESSES),
    (( 65.0, 20.), .3, 1, stiff.LOW_HEAD_STIFFNESSES),
    (( -65.0, 20.), 1.2, 1, stiff.LOW_HEAD_STIFFNESSES),)

MID_DOWN_SCAN_BALL = (
    ((65.,-5.),0.6,  1, stiff.LOW_HEAD_STIFFNESSES),
    ((-65.,-5.),1.0, 1, stiff.LOW_HEAD_STIFFNESSES),
    ((-65.,20.),0.2,1, stiff.LOW_HEAD_STIFFNESSES),
    ((65.,20.),1.0, 1, stiff.LOW_HEAD_STIFFNESSES),
    ((65.0, -5.0) ,0.2, 1, stiff.LOW_HEAD_STIFFNESSES),
    (( -65.0, -5.0),1.0,  1, stiff.LOW_HEAD_STIFFNESSES),
    (( -65.0, -25.0),0.2,  1, stiff.LOW_HEAD_STIFFNESSES),
    (( 0.0, -35.0),0.5,  1, stiff.LOW_HEAD_STIFFNESSES),
    (( 65.0, -25.0),0.5, 1, stiff.LOW_HEAD_STIFFNESSES),)

MID_UP_SCAN_BALL = (
    ((65.0, -5.0) ,0.6, 1, stiff.LOW_HEAD_STIFFNESSES),
    (( -65.0, -5.0),1.0,  1, stiff.LOW_HEAD_STIFFNESSES),
    (( -65.0, -25.0),0.2,  1, stiff.LOW_HEAD_STIFFNESSES),
    (( 0.0, -35.0),0.5,  1, stiff.LOW_HEAD_STIFFNESSES),
    (( 65.0, -25.0),0.5, 1, stiff.LOW_HEAD_STIFFNESSES),
    ((65.,-5.),0.2,  1, stiff.LOW_HEAD_STIFFNESSES),
    ((-65.,-5.),1.0, 1, stiff.LOW_HEAD_STIFFNESSES),
    ((-65.,20.),0.2,1, stiff.LOW_HEAD_STIFFNESSES),
    ((65.,20.),1.0, 1, stiff.LOW_HEAD_STIFFNESSES),)

LOW_SCAN_CLOSE_BOUND = 0
LOW_SCAN_FAR_BOUND = 40
LOW_SCAN_BALL = (
    ((-65.,20.),0.6,1, stiff.LOW_HEAD_STIFFNESSES),
    ((65.,20.),1.0, 1, stiff.LOW_HEAD_STIFFNESSES),
    ((65.0, -5.0) ,0.2, 1, stiff.LOW_HEAD_STIFFNESSES),
    (( -65.0, -5.0),1.0,  1, stiff.LOW_HEAD_STIFFNESSES),
    (( -65.0, -25.0),0.4,  1, stiff.LOW_HEAD_STIFFNESSES),
    (( 0.0, -35.0),0.5,  1, stiff.LOW_HEAD_STIFFNESSES),
    (( 65.0, -25.0),0.5, 1, stiff.LOW_HEAD_STIFFNESSES),
    ((65.,-5.),0.2,  1, stiff.LOW_HEAD_STIFFNESSES),
    ((-65.,-5.),1.0, 1, stiff.LOW_HEAD_STIFFNESSES),)

SPIN_RIGHT_SCAN_BALL = (
    ((-55.,20.),0.4,1, stiff.LOW_HEAD_STIFFNESSES),
    ((50.,20.),0.6,1, stiff.LOW_HEAD_STIFFNESSES),
    ((-60.,-25.),0.8,1, stiff.LOW_HEAD_STIFFNESSES))

RIGHT_EDGE_SCAN_BALL = (
    ((-55.,20.),0.71,1, stiff.LOW_HEAD_STIFFNESSES),
    ((-60.,-32),0.71,1, stiff.LOW_HEAD_STIFFNESSES))

SPIN_LEFT_SCAN_BALL = (
    ((55.,20.),0.4,1, stiff.LOW_HEAD_STIFFNESSES),
    ((-50.,20.),0.6,1, stiff.LOW_HEAD_STIFFNESSES),
    ((60.,-25.),0.8,1, stiff.LOW_HEAD_STIFFNESSES))

LEFT_EDGE_SCAN_BALL = (
    ((55.,20.),0.71,1, stiff.LOW_HEAD_STIFFNESSES),
    ((60.,-32.),0.71,1, stiff.LOW_HEAD_STIFFNESSES))

SQUAT_LOW_SCAN_BALL = (
    ((-65.,20.),0.6,1, stiff.LOW_HEAD_STIFFNESSES),
    ((0.,42.),0.5, 1,  stiff.LOW_HEAD_STIFFNESSES),
    ((65.,20.),0.5, 1, stiff.LOW_HEAD_STIFFNESSES),
    ((65.0, -5.0) ,0.2, 1, stiff.LOW_HEAD_STIFFNESSES),
    (( -65.0, -5.0),1.0,  1, stiff.LOW_HEAD_STIFFNESSES),
    (( -65.0, -25.0),0.4,  1, stiff.LOW_HEAD_STIFFNESSES),
    (( 0.0, -35.0),0.5,  1, stiff.LOW_HEAD_STIFFNESSES),
    (( 65.0, -25.0),0.5, 1, stiff.LOW_HEAD_STIFFNESSES),
    ((65.,-5.),0.2,  1, stiff.LOW_HEAD_STIFFNESSES),
    ((-65.,-5.),1.0, 1, stiff.LOW_HEAD_STIFFNESSES),)


POST_SCAN = (
    ((65.,-25.),2.0,  1, stiff.LOW_HEAD_STIFFNESSES),
    ((-65.,-25.),2.0, 1, stiff.LOW_HEAD_STIFFNESSES))

POST_LEFT_SCAN = ((( 65.,-25.), 0.4, 1, stiff.LOW_HEAD_STIFFNESSES),
                  (( 35.,-35.), 0.2, 1, stiff.LOW_HEAD_STIFFNESSES),
                  ((-35.,-35.), 0.8, 1, stiff.LOW_HEAD_STIFFNESSES),
                  ((-65.,-25.), 0.2, 1, stiff.LOW_HEAD_STIFFNESSES))

POST_RIGHT_SCAN = (((-65.,-25.), 0.4, 1, stiff.LOW_HEAD_STIFFNESSES),
                   ((-35.,-35.), 0.2, 1, stiff.LOW_HEAD_STIFFNESSES),
                   (( 35.,-35.), 0.8, 1, stiff.LOW_HEAD_STIFFNESSES),
                   (( 65.,-25.), 0.2, 1, stiff.LOW_HEAD_STIFFNESSES))

GOALIE_POST_LEFT_SCAN = ((( 115.,10.), 0.6, 1, stiff.LOW_HEAD_STIFFNESSES),
                         (( 35.,-35.), 0.4, 1, stiff.LOW_HEAD_STIFFNESSES),
                         ((-35.,-35.), 0.9, 1, stiff.LOW_HEAD_STIFFNESSES),
                         ((-115.,10.), 0.4, 1, stiff.LOW_HEAD_STIFFNESSES),
                         ((-115.,10.), 0.2, 1, stiff.LOW_HEAD_STIFFNESSES))

GOALIE_POST_RIGHT_SCAN = (((-115.,10.), 0.6, 1, stiff.LOW_HEAD_STIFFNESSES),
                          ((-35.,-35.), 0.4, 1, stiff.LOW_HEAD_STIFFNESSES),
                          (( 35.,-35.), 0.9, 1, stiff.LOW_HEAD_STIFFNESSES),
                          (( 115.,10.), 0.4, 1, stiff.LOW_HEAD_STIFFNESSES),
                          (( 115.,10.), 0.2, 1, stiff.LOW_HEAD_STIFFNESSES))

KICK_SCAN = (
    ((  0.0,-40.0),.5,  1, stiff.LOW_HEAD_STIFFNESSES),
    (( 30.0,-25.0),.5,  1, stiff.LOW_HEAD_STIFFNESSES),
    (( 65.0,-20.0),.5,  1, stiff.LOW_HEAD_STIFFNESSES),
    (( 30.0,-25.0),.5,  1, stiff.LOW_HEAD_STIFFNESSES),
    ((  0.0,-40.0),.5,  1, stiff.LOW_HEAD_STIFFNESSES),
    ((-30.0,-25.0),.5,  1, stiff.LOW_HEAD_STIFFNESSES),
    ((-65.0,-20.0),.5,  1, stiff.LOW_HEAD_STIFFNESSES),
    ((-30.0,-25.0),.5,  1, stiff.LOW_HEAD_STIFFNESSES))

SIDEKICK_PAN_LEFT = (
    ((0.0,30.0),0.3,1,stiff.LOW_HEAD_STIFFNESSES),
    ((28.0,7.0),0.7,1,stiff.LOW_HEAD_STIFFNESSES),
    ((62.0,-20.0),0.8,1,stiff.LOW_HEAD_STIFFNESSES))

SIDEKICK_PAN_RIGHT = (
    ((0.0,30.0),0.3,1,stiff.LOW_HEAD_STIFFNESSES),
    ((-23.0,7.0),0.7,1,stiff.LOW_HEAD_STIFFNESSES),
    ((-62.0,-20.0),0.8,1,stiff.LOW_HEAD_STIFFNESSES))

FRONTKICK_PAN_LEFT = (
    ((0.0,30.0),0.3,1,stiff.LOW_HEAD_STIFFNESSES),
    ((35.0,-35.0),0.8,1,stiff.LOW_HEAD_STIFFNESSES),
    ((-35.0,-35.0),1.2,1,stiff.LOW_HEAD_STIFFNESSES),
    ((0.0,30.0),0.8,1,stiff.LOW_HEAD_STIFFNESSES))

FRONTKICK_PAN_RIGHT = (
    ((0.0,30.0),0.3,1,stiff.LOW_HEAD_STIFFNESSES),
    ((-35.0,-35.0),0.8,1,stiff.LOW_HEAD_STIFFNESSES),
    ((35.0,-35.0),1.2,1,stiff.LOW_HEAD_STIFFNESSES),
    ((0.0,30.0),0.8,1,stiff.LOW_HEAD_STIFFNESSES))

PAN_UP = (
    ((0.,-40.),0.3, 1, stiff.LOW_HEAD_STIFFNESSES),
    ((0.,-40.),0.3, 1, stiff.LOW_HEAD_STIFFNESSES))

PAN_LEFT_HEADS = ( 65.0, -30.0)

PAN_RIGHT_HEADS = ( -65.0, -30.0)

PAN_UP_HEADS = ( 0.0, -30.0)

PAN_DOWN_HEADS = (0.0, 35.0)

PAN_LEFT_DOWN_HEADS = (60.0, 25.0)

PAN_RIGHT_DOWN_HEADS = (-60.0, 25.0)

PAN_RIGHT_SHOULDER_HEADS = (115.0,10.0)

PAN_LEFT_SHOULDER_HEADS = (-115.0,10.0)

LOOK_HEADS = {'left' : PAN_LEFT_HEADS,
              'right' : PAN_RIGHT_HEADS,
              'up' : PAN_UP_HEADS,
              'down' : PAN_DOWN_HEADS,
              'leftDown' : PAN_LEFT_DOWN_HEADS,
              'rightDown' : PAN_RIGHT_DOWN_HEADS}

################### DATA COLLECTION PAN###############
DATA_PAN = (
    ((0.0,-45), 2.0, 0, stiff.LOW_HEAD_STIFFNESSES),
    ((0.0, 15), 2.0, 0, stiff.LOW_HEAD_STIFFNESSES))

################### FIXED PITCH PANS #################
FIXED_PITCH_PAN = (
    ((57.0,17.0), 2.0, 1, stiff.LOW_HEAD_STIFFNESSES),
    ((-57.0,17.0), 2.0, 1, stiff.LOW_HEAD_STIFFNESSES))

# Maximum search angle for top camera.
FIXED_PITCH_PAN_WIDE = (
    ((115.0,11.0), 3.0, 1, stiff.LOW_HEAD_STIFFNESSES),
    ((-115.0,11.0), 3.0, 1, stiff.LOW_HEAD_STIFFNESSES))

# Bottom camera is never significantly blocked by shoulders or arms.
FIXED_PITCH_PAN_NARROW = (
    ((55.0,17.0), 2.0, 1, stiff.LOW_HEAD_STIFFNESSES),
    ((-55.0,17.0), 2.0, 1, stiff.LOW_HEAD_STIFFNESSES))

# Needs some alteration re: pitch for the given yaw
FIXED_PITCH_LOOK_LEFT = ( ((70.0,18.0), 2.0, 1, stiff.LOW_HEAD_STIFFNESSES), )
# Needs some alteration re: pitch for the given yaw
FIXED_PITCH_LOOK_RIGHT = ( ((-70.0,18.0), 2.0, 1, stiff.LOW_HEAD_STIFFNESSES), )

FIXED_PITCH_LOOK_STRAIGHT = ( ((0.0,17.0), 2.0, 1, stiff.LOW_HEAD_STIFFNESSES), )
