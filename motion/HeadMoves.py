# Constants file to store all head moves, including pans and simple moves

import StiffnessModes as stiff

HEAD_MOVE_LENGTH = 4

ZERO_HEADS = (((0.0,0.0),1.0,0, stiff.LOW_HEAD_STIFFNESSES),)

NEUT_HEADS = (((0.,20.),2.0,0, stiff.LOW_HEAD_STIFFNESSES),)

LOOK_DOWN = ( ((0.0,25.0), 0.8, 1, stiff.LOW_HEAD_STIFFNESSES), )

LOOK_UP =  ( ((0.0,-30.0), 0.8, 1, stiff.LOW_HEAD_STIFFNESSES), )

LOOK_RIGHT = ( ((-70.0,25.0), 0.8, 1, stiff.LOW_HEAD_STIFFNESSES), )

LOOK_LEFT =  ( ((70.0,25.0), 0.8, 1, stiff.LOW_HEAD_STIFFNESSES), )

PENALIZED_HEADS = (((0.0,25.0),0.5,0,stiff.LOW_HEAD_STIFFNESSES),)

FIND_BALL_HEADS_LEFT = (((45.,-10.),0.8,0, stiff.LOW_HEAD_STIFFNESSES),
                        ((45.,20.),0.3,1, stiff.LOW_HEAD_STIFFNESSES))

FIND_BALL_HEADS_RIGHT =  (((-45.,-10.),0.8,0, stiff.LOW_HEAD_STIFFNESSES),
                          ((-45.,200.),0.3,1, stiff.LOW_HEAD_STIFFNESSES))


# Distance that can be seen at a certain head pitch
# | degree | close | far |
# |     20 |    15 |  53 |
# |     -5 |    33 | 104 |
# |    -30 |   104 | inf |
#HEAD SCANS

LOC_PANS = (
    (( 65.0, 10.0),1.5, 1, stiff.LOW_HEAD_STIFFNESSES),
    ((65.,-25.),1.0,  1, stiff.LOW_HEAD_STIFFNESSES),
    ((-65.,-25.),2.5, 1, stiff.LOW_HEAD_STIFFNESSES),
    ((-65.0, 10.0) ,1., 1, stiff.LOW_HEAD_STIFFNESSES),
    (( 0.0, 10.0),1.5,  1, stiff.LOW_HEAD_STIFFNESSES),)

QUICK_PANS = (
    ((  0.0,-40.0),.3,  1, stiff.LOW_HEAD_STIFFNESSES),
    (( 30.0,-25.0),.3,  1, stiff.LOW_HEAD_STIFFNESSES),
    (( 65.0,-25.0),.4,  1, stiff.LOW_HEAD_STIFFNESSES),
    (( 30.0,-25.0),.4,  1, stiff.LOW_HEAD_STIFFNESSES),
    ((  0.0,-40.0),.3,  1, stiff.LOW_HEAD_STIFFNESSES),
    ((-30.0,-25.0),.3,  1, stiff.LOW_HEAD_STIFFNESSES),
    ((-65.0,-25.0),.4,  1, stiff.LOW_HEAD_STIFFNESSES),
    ((-30.0,-25.0),.4,  1, stiff.LOW_HEAD_STIFFNESSES))

HIGH_SCAN_CLOSE_BOUND = 104
HIGH_SCAN_BALL = (
    (( -65.0, -30.0),0.6,  1, stiff.LOW_HEAD_STIFFNESSES),
    (( 65.0, -30.0),1.0, 1, stiff.LOW_HEAD_STIFFNESSES),
    ((65.,-5.),0.2,  1, stiff.LOW_HEAD_STIFFNESSES),
    ((-65.,-5.),1.0, 1, stiff.LOW_HEAD_STIFFNESSES),
    ((-65.,20.),0.2,1, stiff.LOW_HEAD_STIFFNESSES),
    ((65.,20.),1.0, 1, stiff.LOW_HEAD_STIFFNESSES),
    ((65.0, -5.0) ,0.2, 1, stiff.LOW_HEAD_STIFFNESSES),
    (( -65.0, -5.0),1.0,  1, stiff.LOW_HEAD_STIFFNESSES),)

MID_SCAN_CLOSE_BOUND = 40
MID_SCAN_FAR_BOUND = 104
MID_DOWN_SCAN_BALL = (
    ((65.,-5.),0.6,  1, stiff.LOW_HEAD_STIFFNESSES),
    ((-65.,-5.),1.0, 1, stiff.LOW_HEAD_STIFFNESSES),
    ((-65.,20.),0.2,1, stiff.LOW_HEAD_STIFFNESSES),
    ((65.,20.),1.0, 1, stiff.LOW_HEAD_STIFFNESSES),
    ((65.0, -5.0) ,0.2, 1, stiff.LOW_HEAD_STIFFNESSES),
    (( -65.0, -5.0),1.0,  1, stiff.LOW_HEAD_STIFFNESSES),
    (( -65.0, -30.0),0.2,  1, stiff.LOW_HEAD_STIFFNESSES),
    (( 65.0, -30.0),1.0, 1, stiff.LOW_HEAD_STIFFNESSES),)

MID_UP_SCAN_BALL = (
    ((65.0, -5.0) ,0.6, 1, stiff.LOW_HEAD_STIFFNESSES),
    (( -65.0, -5.0),1.0,  1, stiff.LOW_HEAD_STIFFNESSES),
    (( -65.0, -30.0),0.2,  1, stiff.LOW_HEAD_STIFFNESSES),
    (( 65.0, -30.0),1.0, 1, stiff.LOW_HEAD_STIFFNESSES),
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
    (( -65.0, -30.0),0.4,  1, stiff.LOW_HEAD_STIFFNESSES),
    (( 65.0, -30.0),1.0, 1, stiff.LOW_HEAD_STIFFNESSES),
    ((65.,-5.),0.2,  1, stiff.LOW_HEAD_STIFFNESSES),
    ((-65.,-5.),1.0, 1, stiff.LOW_HEAD_STIFFNESSES),)


POST_SCAN = (
    ((65.,-25.),2.0,  1, stiff.LOW_HEAD_STIFFNESSES),
    ((-65.,-25.),2.0, 1, stiff.LOW_HEAD_STIFFNESSES))

PAN_LEFT = (
    (( 65.0, -25.0),1.0, 1, stiff.LOW_HEAD_STIFFNESSES), )

PAN_RIGHT = (
    (( -65.0, -25.0),1.0, 1, stiff.LOW_HEAD_STIFFNESSES), )


KICK_SCAN = (
    ((0.0,-45),0.5, 1, stiff.LOW_HEAD_STIFFNESSES),
    ((90.,-25.),0.5,  1, stiff.LOW_HEAD_STIFFNESSES),
    ((0.0,-45),0.5, 1, stiff.LOW_HEAD_STIFFNESSES),
    ((-90.,-25.),0.5, 1, stiff.LOW_HEAD_STIFFNESSES),
    ((0.0,20.),0.5,1,stiff.LOW_HEAD_STIFFNESSES))

