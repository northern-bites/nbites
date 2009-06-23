
#constants file to store all our sweet ass-moves for the Nao
#import MotionConstants

import StiffnessModes as stiff

SWEET_MOVE_LENGTH = 7
CHAIN_MOVE_LENGTH = 5


def getMoveTime(move):
    totalTime = 0.0
    for target in move:
        totalTime += target[-3]

    return totalTime

OFF = None #OFF means the joint chain doesnt get enqueued during this motion

INITIAL_POS = (((60.0, 35.0, 0.0, 0.0),
                (0.0,  0.0,  -21.6, 52.13, -30.3, 0.0),
                (0.0,  0.0,  -21.6, 52.13, -30.3, 0.0),
                (60.0,-35.0, 0.0, 0.0),3.0,0,stiff.LOOSE_ARMS_STIFFNESSES),)

#Angles measured pretty exactly from the robot w/gains off.
#might want to make them even different if we suspect the motors are weakening
SIT_POS = (((0.,90.,0.,0.),
            (0.,0.,-55.,125.7,-75.7,0.),
            (0.,0.,-55.,125.7,-75.7,0.),
            (0.,-90.,0.,0.),3.0,0,stiff.LOW_HEAD_STIFFNESSES),
           ((90.,0.,-65.,-57.),
            (0.,0.,-55.,125.7,-75.7,0.),
            (0.,0.,-55.,125.7,-75.7,0.),
            (90.,0.,65.,57.),1.5,0,stiff.LOW_HEAD_STIFFNESSES))

ZERO_POS = (((0.,0.,0.,0.),
             (0.,0.,0.,0.,0.),
             (0.,0.,0.,0.,0.,0.),
             (0.,0.,0.,0.),4.0,0,stiff.LOOSE_ARMS_STIFFNESSES),)
PENALIZED_POS = INITIAL_POS

SET_POS = INITIAL_POS

READY_POS = INITIAL_POS

#same as INITIAL_POS, except for time ot get to pos
GOALIE_POS = ((INITIAL_POS[0][0],
               INITIAL_POS[0][1],
               INITIAL_POS[0][2],
               INITIAL_POS[0][3],
               1.0,0,stiff.LOOSE_ARMS_STIFFNESSES),)

#KICKS

STAND_FOR_KICK_LEFT = (
    ((80.,90.,-50.,-70.),
     (0.,0.,-10.,20.,-10.,0.),
     (0.,0.,-10.,20.,-10.,0.),
     (80.,-40.,50.,70.),2.0,0, stiff.LEFT_FAR_KICK_STIFFNESSES),
    #swing to the right
    ((80.,40.,-50.,-70.),
     (0.,20.,-10.,20.,-10.,-20.),
     (0.,23.,-13.,20.,-10.,-20.),
     (80.,-40.,50.,70.),2.0,0, stiff.LEFT_FAR_KICK_STIFFNESSES) )

# NEEDS 0.4 value for right leg
LEFT_FAR_KICK = (
    #swing to the right
    ((80.,40.,-50.,-70.),
     (0.,0.,-15.,20.,-10.,0.),
     (0.,0.,-15.,20.,-10.,0.),
     (80.,-40.,50.,70.),0.8,0, stiff.LEFT_FAR_KICK_STIFFNESSES),
    ((80.,40.,-50.,-70.),
     (0.,15.,-10.,20.,-10.,-20.),
     (0.,15.,-10.,20.,-10.,-20.),
     (80.,-40.,50.,70.),0.8,0, stiff.LEFT_FAR_KICK_STIFFNESSES),
    # Lift leg leg
    ((80.,40.,-50.,-70.),
     (0.,15.,-45.,85.,-40.,-20.),
     (0.,15.,-12.,16.,-10.,-20.),
     (80.,-40.,50.,70.),0.8,0, stiff.LEFT_FAR_KICK_STIFFNESSES),
    # kick left leg
    ((80.,40.,-50.,-70.),
     (0.,15.,-60.,41.,-8.,-20.),
     (0.,15.,-10.,30.,-10.,-20.),
     (80.,-40.,50.,70.),0.11,0, stiff.LEFT_FAR_KICK_STIFFNESSES),
    # unkick foot
    ((80.,40.,-50.,-70.),
     (0.,15.,-45.,85.,-40.,-10.),
     (0.,15.,-12.,16.,-10.,-20.),
     (80.,-40.,50.,70.),0.2,0, stiff.LEFT_FAR_KICK_STIFFNESSES),
    # put foot down
    ((80.,40.,-50.,-70.),
     (0.,15.,-22,42.,-20,-10.),
     (0.,15.,-10.,20.,-10.,-20.),
     (80.,-40.,50.,70.),1.0,0, stiff.LEFT_FAR_KICK_STIFFNESSES),
    #swing to normal
    ((80.,40.,-50.,-70.),
     (0.,15.,-12.5,25.,-12.5,-20.),
     (0.,15.,-10.,20.,-10.,-20.),
     (80.,-40.,50.,70.),0.7,0, stiff.LEFT_FAR_KICK_STIFFNESSES),
    ((80.,40.,-50.,-70.),
     (0.,0.,-15.,20.,-10.,0.),
     (0.,0.,-15.,20.,-10.,0.),
     (80.,-40.,50.,70.),1.0,0, stiff.LOW_HEAD_STIFFNESSES)
    )


# STAND UPS
STAND_UP_FRONT = ( ((90,50,0,0),
                    (0,8,0,120,-65,0),
                    (0,0,8,120,-65,4),
                    (90,-50,0,0 ),1.0,0, stiff.STANDUP_STIFFNESSES ),

                   ((90,90,0,0),
                    (0,8,0,120,-65,0),
                    (0,0,8,120,-65,4),
                    (90,-90,0,0 ),1.0,0, stiff.STANDUP_STIFFNESSES ),

                   ((-90,90,0,0),
                    (0,8,0,120,-65,0),
                    (0,0,8,120,-65,4),
                    (-90,-90,0,0 ),0.5,0, stiff.STANDUP_STIFFNESSES ),

                   ((-90,0,0,0),
                    (0,8,0,120,-65,0),
                    (0,0,8,120,-65,4),
                    (-90,0,0,0 ),0.75,0, stiff.STANDUP_STIFFNESSES ),

                   ((-90,0,-90,0),
                    (0,8,0,120,-65,0),
                    (0,0,8,120,-65,4),
                    (-90,0,90,0 ),0.3,0, stiff.STANDUP_STIFFNESSES ),

                   ((-50,0,-90,-35),
                    (5,8,-90,120,-65,0),
                    (5,0,-90,120,-65,4),
                    (-50,0,90,35),2.0,0, stiff.STANDUP_STIFFNESSES),

                   ((-10,0,-90,-95),
                    (-50,8,-90,60,-44,-39),
                    (-50,0,-90,60,-44,39),
                    (-10,0,90,95),1.3,0, stiff.STANDUP_STIFFNESSES),

                   ((0,0,-90,-8),
                    (-50,8,-90,58,5,-31),
                    (-50,0,-90,58,5,31),
                    (0,0,90,8),1.7,0, stiff.STANDUP_STIFFNESSES),

                   ((35,2,-14,-41),
                    (-55,5,-90,123,-17,-17),
                    (-55,-5,-90,123,-17,17),
                    (35,2,14,41),0.8, 0, stiff.STANDUP_STIFFNESSES),

                   ((64,7,-53,-74),
                    (-45,6,-61,124,-41,-6),
                    (-45,-6,-61,124,-41,6),
                    (64,-7,53,74),1.2, 0, stiff.STANDUP_STIFFNESSES),

                   ((93,10,-90,-80),
                    (0,0,-60,120,-60,0),
                    (0,0,-60,120,-60,0),
                    (93,-10,90,80),1.0,0, stiff.STANDUP_STIFFNESSES),

                   ( INITIAL_POS[0][0],
                     INITIAL_POS[0][1],
                     INITIAL_POS[0][2],
                     INITIAL_POS[0][3],1.0,0, stiff.STANDUP_STIFFNESSES))

STAND_UP_BACK = ( ((0,90,0,0),
                   (0,0,0,0,0,0),
                   (0,0,0,0,0,0),
                   (0,-90,0,0),1.0,0, stiff.STANDUP_STIFFNESSES),

                  ((120,46,9,0),
                   (0,8,10,96,14,0),
                   (0,0,10,96,14,4),
                   (120,-46,-9,0), 1.0, 0, stiff.STANDUP_STIFFNESSES),

                  ((120,25,10,-95),
                   (-2,8,0,70,18,0),
                   (-2,0,0,70,18,4),
                   (120,-25,-10,95),0.7,0, stiff.STANDUP_STIFFNESSES),

                  ((120,22,15,-30),
                   (-38,8,-90,96,14,0),
                   (-38, 0,-90, 96, 14, 4),
                   ( 120,-22,-15, 30), 0.7, 0, stiff.STANDUP_STIFFNESSES),


                  ((120,0,5,0),
                   (-38,31,-90,96,45,0),
                   (-38,-31,-90,96,45,4),
                   (120,0,-5,0), 1.0,0, stiff.STANDUP_STIFFNESSES),

                  ((40,60,4,-28),
                   (-28,8,-49,126,-32,-22),
                   (-28,-31,-87,70,45,0),
                   (120,-33,-4,4),1.0,0, stiff.STANDUP_STIFFNESSES),

                  ((42,28,5,-47),
                   (-49,-16,22,101,-70,-5),
                   (-49,-32,-89,61,39,-7),
                   (101,-15,-4,3),0.9,0, stiff.STANDUP_STIFFNESSES),

                  ((42,28,4,-46),
                   (-23,11,-49,126,-70,6),
                   (-23,-17,-51,50,23,38),
                   (51,-50,0,26), 1.0,0, stiff.STANDUP_STIFFNESSES),

                  ((42,28,4,-46),
                   (-23,21,-47,126,-70,5),
                   (-23,-1,-51,101,-33,15),
                   (51,-39,0,32), 0.5,0, stiff.STANDUP_STIFFNESSES),

                  ((98,2,-72,-65),
                   (0,0,-50,120,-70,0),
                   (0,0,-50,120,-70,0),
                   (98,-2,72,65), 1.1,0, stiff.STANDUP_STIFFNESSES),

                  ( INITIAL_POS[0][0],
                    INITIAL_POS[0][1],
                    INITIAL_POS[0][2],
                    INITIAL_POS[0][3],1.0,0, stiff.STANDUP_STIFFNESSES))

RIGHT_FAR_KICK = (
    ((80.0 , 40.0 , -50.0 , -70.0),
     (0.0 , -0.0 , -15.0 , 20.0 , -10.0 , -0.0),
     (0.0 , -0.0 , -15.0 , 20.0 , -10.0 , -0.0),
     (80.0 , -40.0 , 50.0 , 70.0), 0.8 , 0, stiff.RIGHT_FAR_KICK_STIFFNESSES),

    ((80.0 , 40.0 , -50.0 , -70.0),
     (0.0 , -15.0 , -10.0 , 20.0 , -10.0 , 20.0),
     (0.0 , -15.0 , -10.0 , 20.0 , -10.0 , 20.0),
     (80.0 , -40.0 , 50.0 , 70.0), 0.8 , 0, stiff.RIGHT_FAR_KICK_STIFFNESSES ),

    ((80.0 , 40.0 , -50.0 , -70.0),
     (0.0 , -15.0 , -12.0 , 16.0 , -10.0 , 20.0),
     (0.0 , -15.0 , -45.0 , 85.0 , -40.0 , 20.0),
     (80.0 , -40.0 , 50.0 , 70.0), 0.8 , 0, stiff.RIGHT_FAR_KICK_STIFFNESSES),

    ((80.0 , 40.0 , -50.0 , -70.0),
     (0.0 , -15.0 , -10.0 , 30.0 , -10.0 , 20.0),
     (0.0 , -15.0 , -60.0 , 41.0 , -8.0 , 20.0),
     (80.0 , -40.0 , 50.0 , 70.0), 0.11 , 0, stiff.RIGHT_FAR_KICK_STIFFNESSES),

    ((80.0 , 40.0 , -50.0 , -70.0),
     (0.0 , -15.0 , -12.0 , 16.0 , -10.0 , 20.0),
     (0.0 , -15.0 , -45.0 , 85.0 , -40.0 , 10.0),
     (80.0 , -40.0 , 50.0 , 70.0), 0.2 , 0, stiff.RIGHT_FAR_KICK_STIFFNESSES),

    ((80.0 , 40.0 , -50.0 , -70.0),
     (0.0 , -15.0 , -10.0 , 20.0 , -10.0 , 20.0),
     (0.0 , -15.0 , -22.0 , 42.0 , -20.0 , 10.0),
     (80.0 , -40.0 , 50.0 , 70.0), 1.0 , 0, stiff.RIGHT_FAR_KICK_STIFFNESSES),

    ((80.0 , 40.0 , -50.0 , -70.0),
     (0.0 , -15.0 , -10.0 , 20.0 , -10.0 , 20.0),
     (0.0 , -15.0 , -12.5 , 25.0 , -12.5 , 20.0),
     (80.0 , -40.0 , 50.0 , 70.0), 0.7 , 0, stiff.RIGHT_FAR_KICK_STIFFNESSES),

    ((80.0 , 40.0 , -50.0 , -70.0),
     (0.0 , -0.0 , -15.0 , 20.0 , -10.0 , -0.0),
     (0.0 , -0.0 , -15.0 , 20.0 , -10.0 , -0.0),
     (80.0 , -40.0 , 50.0 , 70.0), 1.0 , 0, stiff.LOW_HEAD_STIFFNESSES) )

LEFT_SIDE_KICK = (
    ((80.,90.,-50.,-70.),
     (0.,0.,-10.,20.,-10.,0.),
     (0.,0.,-10.,20.,-10.,0.),
     (80.,-40.,50.,70.),2.0,0, stiff.LEFT_SIDE_KICK_STIFFNESSES),

    ((80.,40.,-50.,-70.),
     (0.,20.,-10.,20.,-10.,-20.),
     (0.,16.,-13.,20.,-10.,-22.),
     (80.,-40.,50.,70.),2.0,0, stiff.LEFT_SIDE_KICK_STIFFNESSES),

    (2, (0.,40.,-30.,70.,-32.,-20.), 0.4 , 0, stiff.LEFT_SIDE_KICK_STIFFNESSES),
    (2, (0.,40.,-50.,30.,20.,0.), 0.4 , 0, stiff.LEFT_SIDE_KICK_STIFFNESSES),

    ((80.,70.,-10.,-10.),
     (0.,9.,-60.,30.,23.,0.),
     (0.,23.,-13.,20.,-10.,-25.),
     (80.,-40.,50.,70.),0.2,0, stiff.LEFT_SIDE_KICK_STIFFNESSES),

    ((80.,70.,-10.,-10.),
     (0.,23.,-13.,20.,-10.,-25.),
     (0.,20.,-13.,20.,-10.,-25.),
     (80.,-40.,50.,70.),0.8,0, stiff.LEFT_SIDE_KICK_STIFFNESSES),

    ((80.,90.,-50.,-70.),
     (0.,0.,-10.,20.,-10.,0.),
     (0.,0.,-10.,20.,-10.,0.),
     (80.,-40.,50.,70.),2.0,0, stiff.LEFT_SIDE_KICK_STIFFNESSES) )


RIGHT_SIDE_KICK = (
    ((80.0 , 40.0 , -50.0 , -70.0),
     (0.0 , -0.0 , -10.0 , 20.0 , -10.0 , -0.0),
     (0.0 , -0.0 , -10.0 , 20.0 , -10.0 , -0.0),
     (80.0 , -90.0 , 50.0 , 70.0), 2.0 , 0, stiff.RIGHT_SIDE_KICK_STIFFNESSES),

    ((80.0 , 40.0 , -50.0 , -70.0),
     (0.0 , -16.0 , -13.0 , 20.0 , -10.0 , 22.0),
     (0.0 , -20.0 , -10.0 , 20.0 , -10.0 , 20.0),
     (80.0 , -40.0 , 50.0 , 70.0), 2.0 , 0, stiff.RIGHT_SIDE_KICK_STIFFNESSES),

    (3, (0.,-40.,-30.,70.,-32.,20.), 0.4 , 0, stiff.RIGHT_SIDE_KICK_STIFFNESSES),
    (3, (0.,-40.,-50.,30.,20.,0.), 0.4 , 0, stiff.RIGHT_SIDE_KICK_STIFFNESSES),


    ((80.0 , 40.0 , -50.0 , -70.0),
     (0.0 , -23.0 , -13.0 , 20.0 , -10.0 , 25.0),
     (0.0 , -9.0 , -60.0 , 30.0 , 23.0 , -0.0),
     (80.0 , -70.0 , 10.0 , 10.0), 0.2 , 0, stiff.RIGHT_SIDE_KICK_STIFFNESSES),

    ((80.0 , 40.0 , -50.0 , -70.0),
     (0.0 , -20.0 , -13.0 , 20.0 , -10.0 , 25.0),
     (0.0 , -23.0 , -13.0 , 20.0 , -10.0 , 25.0),
     (80.0 , -70.0 , 10.0 , 10.0), 0.8 , 0, stiff.RIGHT_SIDE_KICK_STIFFNESSES),

    ((80.0 , 40.0 , -50.0 , -70.0),
     (0.0 , 0.0 , -10.0 , 20.0 , -10.0 , -0.0),
     (0.0 , 0.0 , -10.0 , 20.0 , -10.0 , -0.0),
     (80.0 , -90.0 , 50.0 , 70.0), 2.0 , 0, stiff.RIGHT_SIDE_KICK_STIFFNESSES) )


# Goalie saving stuff

SAVE_LEFT_DEBUG = ( ((-90,0,0,0),
                     (GOALIE_POS[0][1]),
                     (GOALIE_POS[0][2]),
                     (GOALIE_POS[0][3]), .5, 0, stiff.LOOSE_ARMS_STIFFNESSES), )

SAVE_RIGHT_DEBUG = ( ((GOALIE_POS[0][0]),
                      (GOALIE_POS[0][1]),
                      (GOALIE_POS[0][2]),
                      (-90,0,0,0), .5, 0, stiff.LOOSE_ARMS_STIFFNESSES), )

SAVE_CENTER_DEBUG = ( ((-90,0,0,0),
                       (GOALIE_POS[0][1]),
                       (GOALIE_POS[0][2]),
                       (-90,0,0,0), .5,0, stiff.NORMAL_STIFFNESSES), )

SAVE_LEFT_HOLD_DEBUG = ( ((-90,0,0,0),
                         (GOALIE_POS[0][1]),
                         (GOALIE_POS[0][2]),
                         (GOALIE_POS[0][3]), 0.1, 0, stiff.LOOSE_ARMS_STIFFNESSES), )

SAVE_RIGHT_HOLD_DEBUG = ( ((GOALIE_POS[0][0]),
                           (GOALIE_POS[0][1]),
                           (GOALIE_POS[0][2]),
                           (-90,0,0,0), 0.1, 0, stiff.LOOSE_ARMS_STIFFNESSES), )

SAVE_CENTER_HOLD_DEBUG = ( ((-90,0,0,0),
                            (GOALIE_POS[0][1]),
                            (GOALIE_POS[0][2]),
                            (-90,0,0,0), 0.1, 0, stiff.LOOSE_ARMS_STIFFNESSES), )

GOALIE_SAVE = (
    #shoulder pitch, shoulder roll, elbow yaw, elbow roll
    #hip yaw pitch, hip roll, hip pitch, knee pitch, ankle pitch, ankle roll
    ((60.0, 15.0, 0.0, -0), #right arm
     ( -76.0, 10.0, -50.0, 86.0, 0.0, -0),
     ( -76.0, -10.0, -50.0, 86.0, 0.0, 0),
     (60.0, -15.0, 0.0, 0), #left arm
     1.5, 0, stiff.LOOSE_ARMS_STIFFNESSES),

    ((40.0, 15.0, 0.0, -0), #right arm
    ( -76.0, 10.0, -95.0, 65.0, 65.0, 0),
     ( -76.0, -10.0, -95.0, 65.0, 65.0, 0),
    (40.0, -15.0, 0.0, 0), #left arm
    .5, 0, stiff.LOOSE_ARMS_STIFFNESSES) )

GOALIE_STAND_UP = (
                   ((35,2,-14,-41),
                    (-55,5,-90,123,-17,-17),
                    (-55,-5,-90,123,-17,17),
                    (35,2,14,41),0.8, 0, stiff.STANDUP_STIFFNESSES),

                   ((64,7,-53,-74),
                    (-45,6,-61,124,-41,-6),
                    (-45,-6,-61,124,-41,6),
                    (64,-7,53,74),1.2, 0, stiff.STANDUP_STIFFNESSES),

                   ((93,10,-90,-80),
                    (0,0,-60,120,-60,0),
                    (0,0,-60,120,-60,0),
                    (93,-10,90,80),1.0,0, stiff.STANDUP_STIFFNESSES),

                   ( INITIAL_POS[0][0],
                     INITIAL_POS[0][1],
                     INITIAL_POS[0][2],
                     INITIAL_POS[0][3],1.0,0, stiff.STANDUP_STIFFNESSES))

CRAB_SIT = (
    ((120.0, 90.0, -12, 0),
     (-66.0, 5.0, -90.0, 47.0, 65.0, -3.0),
     (-66.0, -5.0, -90.0, 47.0, 65.0, 3.0),
     (120.0, -90.0, 12, 0),
     5.0, 0, stiff.LOOSE_ARMS_STIFFNESSES),)
