#constants file to store all our sweet ass-moves for the Nao

import StiffnessModes as stiff

#============================================================================
#   CODE FROM BURST-ISRAEL ROBOCUP 2009
#
#    Usage:
#    ------
#
#    *GOALIE*
#    GOALIE_DIVE_LEFT = mirrorMove(GOALIE_DIVE_RIGHT)
#    GOALIE_ROLL_OUT_LEFT = mirrorMove(GOALIE_ROLL_OUT_RIGHT)
#
#    *KICKS*
#    KICK_RIGHT = mirrorMove(KICK_LEFT)
#    RIGHT_LONG_BACK_KICK =  mirrorMove(LEFT_LONG_BACK_KICK)
#    mirrorMove(LEFT_D_KICK(-1*y,dist))
#    RIGHT_SHORT_STRAIGHT_KICK = mirrorMove(LEFT_SHORT_STRAIGHT_KICK)
#    GOALIE_TEST_DIVE_RIGHT = mirrorMove(GOALIE_TEST_DIVE_LEFT)
#    GOOGZ_SHORT_RIGHT_SIDE_KICK = mirrorMove(GOOGZ_SHORT_LEFT_SIDE_KICK)
#    GOOGZ_MID_RIGHT_SIDE_KICK = mirrorMove(GOOGZ_MID_LEFT_SIDE_KICK)
#    GOOGZ_RIGHT_SIDE_KICK = mirrorMove(GOOGZ_LEFT_SIDE_KICK)
#    RIGHT_SHORT_BACK_KICK =  mirrorMove(LEFT_SHORT_BACK_KICK)
#    RIGHT_BIG_KICK = mirrorMove(LEFT_BIG_KICK)
#============================================================================

def mirrorMove(positions):
    return tuple(
        tuple(((RShoulderPitch, -RShoulderRoll, -RElbowYaw, -RElbowRoll),
               (RHipYawPitch, -RHipRoll, RHipPitch, RKneePitch, RAnklePitch, -RAnkleRoll),
               (LHipYawPitch, -LHipRoll, LHipPitch, LKneePitch, LAnklePitch, -LAnkleRoll),
               (LShoulderPitch, -LShoulderRoll, -LElbowYaw, -LElbowRoll),
               interp_time, interpolation, stiff.flipStiffness(stiffness)))
        for
        ((LShoulderPitch, LShoulderRoll, LElbowYaw, LElbowRoll),
         (LHipYawPitch, LHipRoll, LHipPitch, LKneePitch, LAnklePitch, LAnkleRoll),
         (RHipYawPitch, RHipRoll, RHipPitch, RKneePitch, RAnklePitch, RAnkleRoll),
         (RShoulderPitch, RShoulderRoll, RElbowYaw, RElbowRoll),
         interp_time, interpolation, stiffness) in positions)

def getMoveTime(move):
    totalTime = 0.0
    for target in move:
        totalTime += target[-3]

    return totalTime

OFF = None #OFF means the joint chain doesnt get enqueued during this motion

INITIAL_POS = (((90., 10.0, -90., -10.),
                (0.0,  0.0,  -22.3, 43.5, -21.2, 0.0),
                (0.0,  0.0,  -22.3, 43.5, -21.2, 0.0),
                (90.0,-10., 82, 13.2),
                3.0,0,stiff.LOOSE_ARMS_STIFFNESSES),)

#Not used
NO_HEAD_INITIAL_POS = (((60.0, 35.0, 0.0, 0.0),
                        (0.0,  0.0,  -21.6, 52.13, -30.3, 0.0),
                        (0.0,  0.0,  -21.6, 52.13, -30.3, 0.0),
                        (60.0,-35.0, 0.0, 0.0),
                        3.0,0,stiff.NO_HEAD_STIFFNESSES),)

#Angles measured pretty exactly from the robot w/gains off.
#might want to make them even different if we suspect the
#motors are weakening
SIT_POS = (((0.,90.,0.,0.),
            (0.,0.,-55.,125.7,-75.7,0.),
            (0.,0.,-55.,125.7,-75.7,0.),
            (0.,-90.,0.,0.),
            3.0,0,stiff.LOW_HEAD_STIFFNESSES),

           ((90.,0.,-65.,-57.),
            (0.,0.,-55.,125.7,-75.7,0.),
            (0.,0.,-55.,125.7,-75.7,0.),
            (90.,0.,65.,57.),
            1.5,0,stiff.LOW_LEG_STIFFNESSES))
#Not used
ZERO_POS = (((0.,0.,0.,0.),
             (0.,0.,0.,0.,0.),
             (0.,0.,0.,0.,0.,0.),
             (0.,0.,0.,0.),
             4.0,0,stiff.LOOSE_ARMS_STIFFNESSES),)

#**********************
#                     *
#     STAND  UPS      *
#                     *
#**********************

STAND_UP_FRONT = ( ((90,50,0,0),
                    (0,0,8,120,-65,0),
                    (0,0,8,120,-65,4),
                    (90,-50,0,0 ),
                    0.3,1, stiff.STANDUP_STIFFNESSES ),

                   ((90,90,0,0),
                    (0,0,8,120,-65,0),
                    (0,0,8,120,-65,4),
                    (90,-90,0,0 ),
                    0.3,1, stiff.STANDUP_STIFFNESSES ),

                   ((-90,90,0,0),
                    (0,0,8,120,-65,0),
                    (0,0,8,120,-65,4),
                    (-90,-90,0,0 ),
                    0.3,1, stiff.STANDUP_STIFFNESSES ),

                   ((-90,0,0,0),
                    (0,0,8,120,-65,0),
                    (0,0,8,120,-65,4),
                    (-90,0,0,0 ),
                    0.3,1, stiff.STANDUP_STIFFNESSES ),

                   ((-90,0,-90,0),
                    (0,0,8,120,-65,0),
                    (0,0,8,120,-65,4),
                    (-90,0,90,0 ),
                    0.3,1, stiff.STANDUP_STIFFNESSES ),
                   #All of the above basically position the arms

                   ((-50,0,-90,-35),
                    (5,8,-90,120,-65,0),
                    (5,0,-90,120,-65,4),
                    (-50,0,90,35),
                    0.7,1, stiff.STANDUP_STIFFNESSES),

                   ((25,20,-60,-88),
                    (-50,0,-90,70,-44,-39),
                    (-50,0,-90,70,-44,39),
                    (25,-20,60,88),
                    0.7,1, stiff.STANDUP_STIFFNESSES),
                   #Bring arms out to avoid stuck elbows

                   ((25,0,-90,-88),
                    (-50,0,-90,70,-44,-39),
                    (-50,0,-90,70,-44,39),
                    (25,0,90,88),
                    0.2,1, stiff.STANDUP_STIFFNESSES),
                   #Squatting, supported by hands

                   ((0,0,-90,-8),
                    (-50,8,-90,58,5,-31),
                    (-50,0,-90,58,5,31),
                    (0,0,90,8),
                    1.0,1, stiff.STANDUP_STIFFNESSES),
                   #Push back onto feet

                   ((35,2,-14,-41),
                    (-50,5,-90,100,5,-5),
                    (-50,-5,-90,100,5,5),
                    (35,2,14,41),
                    .7, 1, stiff.STANDUP_STIFFNESSES),

                   ((35,2,-14,-41),
                    (-50,5,-90,123,-13,-5),
                    (-50,-5,-90,123,-13,5),
                    (35,2,14,41),
                    .4, 1, stiff.STANDUP_STIFFNESSES),
                   #Rock back to squatting upright

                   ((75,10,-53,-74),
                    (-40,6,-61,124,-35,-6),
                    (-40,-6,-61,124,-35,6),
                    (75,-10,53,74),
                    .3, 1, stiff.STANDUP_STIFFNESSES),

                   ((93,10,-90,-80),
                    (0,0,-60,120,-60,0),
                    (0,0,-60,120,-60,0),
                    (93,-10,90,80),
                    0.4,1, stiff.STANDUP_STIFFNESSES),
                   #Pull knees together

                   #Stands up:
                   ( INITIAL_POS[0][0],
                     INITIAL_POS[0][1],
                     INITIAL_POS[0][2],
                     INITIAL_POS[0][3],
                     0.7,1, stiff.STANDUP_STIFFNESSES))

# old robots
STAND_UP_BACK_OLD = (((100,0,0,0),
                  (0,0,0,0,0,0),
                   (0,0,0,0,0,0),
                  (100,0,0,0),
                  0.6,0, stiff.STANDUP_STIFFNESSES),
                 #arms at sides

                 ((119,18,6,-88),
                  (0,0,27,105,-15,0),
                  (0,0,27,105,-15,0),
                  (119,-18,-6,88),
                  0.4,1, stiff.STANDUP_STIFFNESSES),
                 #hips arch and arms underneath

                 ((119,15,6,-85),
                  (-9.49,24.70,-40.25,115.22,22.41,7.74),
                  (-9.49,-24.12,-40.19,115.22,22.47,-7.74),
                  (119,-15,-6,85),
                  0.4,1, stiff.STANDUP_STIFFNESSES),
                 # hips sit and upper body comes up with hands under

                 ((73.48,-16.17,81.55,-88.88),
                  (-9.49,24.70,-40.25,115.22,22.41,7.74),
                  (-9.49,-24.12,-40.19,115.22,22.47,-7.74),
                  (72.78,16.70,-81.74,88.51),
                  0.4,1, stiff.STANDUP_STIFFNESSES),
                 #twists hands

                 ((72,-16, 81, -88),
                  (-9.49,24.70,-40.25,115.22,22.41,7.74),
                  (-9.49,-24.12,-40.19,115.22,22.47,-7.74),
                  (72.78,16.70,-81.74,88.51),
                  0.2,1, stiff.STANDUP_STIFFNESSES),
                 # legs go down and upper body comes up

                 ((119,-11.08,94.13,-1.93),
                  (-62.93,9.06,-84.64,116.72,21.88,23.73),
                  (-62.93,-29.44,-82.62,103.71,30.85,-10.11),
                  (115.93,17.58,-90.62,3.34),
                  0.6,1, stiff.STANDUP_STIFFNESSES),
                 #sitting legs spread hands behind facing forward

                 ((40,60,4,-28),
                  (-28,8,-49,126,-32,-22),
                  (-28,-31,-87,70,45,0),
                  (120,-33,-4,4),
                  0.6,0, stiff.STANDUP_STIFFNESSES),
                 # turns to right a little and stands with one arm on ground

                 ((42,28,5,-47),
                  (-49,-16,22,101,-70,-5),
                  (-49,-32,-89,61,39,-7),
                  (101,-15,-4,3),
                  0.6,1, stiff.STANDUP_STIFFNESSES),
                 #gets hips up and over knees

                 ((59.76,29.35,4.39,-45.79),
                  (-30.23,-10.19,-35.86,122.78,-67.68,8.44),
                  (-30.23,-26.72,-70.78,54.41,34.99,19.69),
                  (54.85,-45.62,-0.27,24.35),
                  0.4,1, stiff.STANDUP_STIFFNESSES),
                 #pulls right leg in a little

                 ((35,2,-14,-41),
                  (-50,5,-90,123,-13,-5),
                  (-50,-5,-90,123,-13,5),
                  (35,2,14,41),
                  0.6, 1, stiff.STANDUP_STIFFNESSES),
                  #squatting upright

                 ((75,10,-53,-74),
                  (-40,6,-61,124,-35,-6),
                  (-40,-6,-61,124,-35,6),
                  (75,-10,53,74),
                  0.4, 1, stiff.STANDUP_STIFFNESSES),

                 ((93,10,-90,-80),
                  (0,0,-60,120,-60,0),
                  (0,0,-60,120,-60,0),
                  (93,-10,90,80),
                  0.4,1, stiff.STANDUP_STIFFNESSES),
                 #Pull knees together

                 #stands up
                 (INITIAL_POS[0][0],
                  INITIAL_POS[0][1],
                  INITIAL_POS[0][2],
                  INITIAL_POS[0][3],
                  0.6,0, stiff.STANDUP_STIFFNESSES)
                 )

# new robots
STAND_UP_BACK = (((100,0,0,0),
                  (0,0,0,0,0,0),
                   (0,0,0,0,0,0),
                  (100,0,0,0),
                  0.6,0, stiff.STANDUP_STIFFNESSES),
                 #arms at sides

                 ((119,18,6,-88),
                  (0,0,27,105,-15,0),
                  (0,0,27,105,-15,0),
                  (119,-18,-6,88),
                  0.4,1, stiff.STANDUP_STIFFNESSES),
                 #hips arch and arms underneath

                 ((119,15,6,-85),
                  (-9.49,24.70,-40.25,115.22,22.41,7.74),
                  (-9.49,-24.12,-40.19,115.22,22.47,-7.74),
                  (119,-15,-6,85),
                  0.4,1, stiff.STANDUP_STIFFNESSES),
                 # hips sit and upper body comes up with hands under

                 ((73.48,-16.17,81.55,-88.88),
                  (-9.49,24.70,-40.25,100,22.41,7.74),
                  (-9.49,-24.12,-40.19,100,22.47,-7.74),
                  (72.78,16.70,-81.74,88.51),
                  .4,1, stiff.STANDUP_STIFFNESSES),
                 #twists hands

                 ((119,-11.08,94.13,-1.93),
                  (-62.93,9.06,-84.64,116.72,21.88,23.73),
                  (-62.93,-29.44,-82.62,103.71,30.85,-10.11),
                  (115.93,17.58,-90.62,3.34),
                  0.6,1, stiff.STANDUP_STIFFNESSES),
                 #sitting legs spread hands behind facing forward

                 ((40,60,4,-28),
                  (-28,8,-49,126,-25,-22),
                  (-28,-31,-87,80,52,0),
                  (120,-33,-4,4),
                  .6,1, stiff.STANDUP_STIFFNESSES),
                 # turns to right a little and stands with one arm on ground

                 ((42,28,5,-47),
                  (-49,-16,22,101,-70,-5),
                  (-49,-32,-89,61,39,-7),
                  (101,-15,-4,3),
                  0.6,1, stiff.STANDUP_STIFFNESSES),
                 #gets hips up and over knees

                 ((59.76,29.35,4.39,-45.79),
                  (-30.23,-10.19,-35.86,122.78,-67.68,8.44),
                  (-30.23,-26.72,-70.78,54.41,34.99,19.69),
                  (54.85,-45.62,-0.27,24.35),
                  0.4,1, stiff.STANDUP_STIFFNESSES),
                 #pulls right leg in a little

                 ((35,2,-14,-41),
                  (-50,5,-90,123,-13,-5),
                  (-50,-5,-90,123,-13,5),
                  (35,2,14,41),
                  0.6, 1, stiff.STANDUP_STIFFNESSES),
                  #squatting upright

                 ((75,10,-53,-74),
                  (-40,6,-61,124,-35,-6),
                  (-40,-6,-61,124,-35,6),
                  (75,-10,53,74),
                  0.4, 1, stiff.STANDUP_STIFFNESSES),

                 ((93,10,-90,-80),
                  (0,0,-60,120,-60,0),
                  (0,0,-60,120,-60,0),
                  (93,-10,90,80),
                  0.4,1, stiff.STANDUP_STIFFNESSES),
                 #Pull knees together

                 #stands up
                 (INITIAL_POS[0][0],
                  INITIAL_POS[0][1],
                  INITIAL_POS[0][2],
                  INITIAL_POS[0][3],
                  0.6,0, stiff.STANDUP_STIFFNESSES)
                 )

#**********************
#                     *
#       GOALIE        *
#                     *
#**********************

#Squats with legs in M shape and arms at a 45 degree angle
GOALIE_SQUAT = (((-0.0, 90.0, 0.0, 0.0),
                 (-76.0, 28.0, -54.0, 86.0, 0.0, -22.0),
                 (-76.0, -28.0, -54.0, 86.0, 0.0, 22.0),
                 (-0.0, -90.0, -0.0, -0.0),
                 0.4, 1, stiff.NORMAL_STIFFNESSES),
                # goalieSquatPart2
                ((-0.0, 45.0, 0.0, 0.0),
                 (-90.0, 38.0, -75.0, 90.0, 30.0, -22.0),
                 (-90.0, -38.0, -75.0, 90.0, 30.0, 22.0),
                 (-0.0, -45.0, -0.0, -0.0),
                 0.2, 1, stiff.NORMAL_STIFFNESSES),
                # goalieSquatPart2 DELAY
                ((-0.0, 45.0, 0.0, 0.0),
                 (-90.0, 38.0, -75.0, 90.0, 30.0, -22.0),
                 (-90.0, -38.0, -75.0, 90.0, 30.0, 22.0),
                 (-0.0, -45.0, -0.0, -0.0),
                 2.75, 1, stiff.NORMAL_STIFFNESSES),
                )

GOALIE_NEW_SQUAT = (((-0.0, 90.0, 0.0, 0.0),
                     (-65.6, 28.0, -54.0, 86.0, 0.3, -22.0),
                     (-65.6, -28.0, -54.0, 86.0, 0.3, 22.0),
                     (-0.0, -90.0, -0.0, -0.0),
                     .4, 1, stiff.NORMAL_STIFFNESSES),
                    # goalieSquatPart2
                    ((-0.0, 45.0, 0.0, 0.0),
                     (-90.0, 38.0, -75.0, 90.0, 30.0, -22.0),
                     (-90.0, -38.0, -75.0, 90.0, 30.0, 22.0),
                     (-0.0, -45.0, -0.0, -0.0),
                     0.2, 1, stiff.NORMAL_STIFFNESSES),
                    # goalieSquatPart2 DELAY
                    ((-0.0, 45.0, 0.0, 0.0),
                     (-90.0, 38.0, -75.0, 90.0, 30.0, -22.0),
                     (-90.0, -38.0, -75.0, 90.0, 30.0, 22.0),
                     (-0.0, -45.0, -0.0, -0.0),
                     2.75, 1, stiff.NORMAL_STIFFNESSES),
                )

GOALIE_NEW_SQUAT_STAND_UP = (((14.0, 16.2, -1.2, -2.4),  # goalieSquatDip
                              (-69.2, 55.0, -89.7, 92.2, 42.1, -22.),
                              (-69.2, -55.0, -89.7, 92.2, 42.1, 22.),
                              (14.0, -16.2, 1.2, 2.4),
                              0.3, 1, stiff.NORMAL_STIFFNESSES),
                             # goalieSquatGetupPart1
                             ((86.0, 32.0, -61.0, -2.4),
                              (-49.9, 24.3287533314, -24.5193819127, 124.276494062, -70.0521140146, 5.45169307096),
                              (-49.9, -44.1874737017, -14.8561122373, 108.812372045, -47.195459693, 20.0417147985),
                              (85.0, -5.0, 61.0, 2.4),
                              0.4, 1, stiff.NORMAL_STIFFNESSES),
                             # goalieSquatGetupPart2
                             ((80.0, 80.0, 0.0, 0.0),
                              (-35.0, -0.0, -50.0, 130.0, -50.0, -0.0),
                              (-35.0, 0.0, -50.0, 130.0, -50.0, 0.0),
                              (80.0, -80.0, 0.0, -0.0),
                              0.6, 1, stiff.NORMAL_STIFFNESSES),
                             #stands up
                             (INITIAL_POS[0][0],
                              INITIAL_POS[0][1],
                              INITIAL_POS[0][2],
                              INITIAL_POS[0][3],
                              0.6,0, stiff.STANDUP_STIFFNESSES)
)

#Fast dive to the right
GOALIE_DIVE_RIGHT = ( (INITIAL_POS[0][0],
                       (-5.10,3,-55,115,-60,-3),
                       (-5.10,-3,-55,115,-60,3),
                       INITIAL_POS[0][3],
                       0.3, 0, stiff.GOALIE_DIVE_RIGHT_STIFFNESSES),
                      #squats down

                      ((0, 70, 0, 0),
                       (-5.27,-2.63,-38.85,88.24,-45.88,-9.84),
                       (-5.27,-16.43,-63.46,123.23,-59.76,-11.07),
                       (0, -70, 0, 0),
                       0.3, 1, stiff.GOALIE_DIVE_RIGHT_STIFFNESSES),
                      #arms out leans to one side

                      ((0, 0, 0, 0),
                       (-5.27,-2.63,-38.85,60.24,-45.88,-9.84),
                       (-5.27,-16.43,-63.46,123.23,-59.76,-11.07),
                       (-85.25,-60.65,-28.22,51.77),
                       0.2, 1, stiff.GOALIE_DIVE_RIGHT_STIFFNESSES),
                      #falls over and puts arms in final position

                      ((0, 0, 0, 0),
                       (-5.27,-2.63,-38.85,60.24,-45.88,-9.84),
                       (-5.27,-16.43,-63.46,123.23,-59.76,-11.07),
                       (-85.25,-60.65,-28.22,51.77),
                       0.1, 1, stiff.GOALIE_GROUND_STIFFNESSES),
                      #removes stiffnesses so lies flat

                      ((28.30,-20.92,0.35,-1.58),
                       (-10.58,0,0,16.52,50.01,-10.81),
                       (-10.58,0,0,9.93,43.86,-10.54),
                       (-58.53,-2.20,-52.74,5.01),
                       0.3, 1, stiff.GOALIE_DIVE_RIGHT_STIFFNESSES),
                      #extend legs and arm
                       )

#same as GOALIE_DIVE_RIGHT except dives left
GOALIE_DIVE_LEFT = mirrorMove(GOALIE_DIVE_RIGHT)

#Rolls goalie over onto back towards goal so she can get up
GOALIE_ROLL_OUT_RIGHT = (((0,70.92,0,0),
                          (-10.58,20.38,25.41,0,0,0),
                          (-10.58,0.00 ,-90.00,0,0,0),
                          (-58.53,-2.20,-52.74,5.01),
                          0.3, 1, stiff.GOALIE_DIVE_RIGHT_STIFFNESSES),
                         )

GOALIE_ROLL_OUT_LEFT = mirrorMove(GOALIE_ROLL_OUT_RIGHT)

#Points right when would dive left
GOALIE_TEST_DIVE_LEFT = ( ((0 , 70, -50, 0),
                            INITIAL_POS[0][1],
                            INITIAL_POS[0][2],
                            INITIAL_POS[0][3],
                            1.0,0,stiff.STANDUP_STIFFNESSES),)

#Points left when would dive right
GOALIE_TEST_DIVE_RIGHT = mirrorMove(GOALIE_TEST_DIVE_LEFT)



#Both arms point forward when would save center
GOALIE_TEST_CENTER_SAVE = ( ((0, -5, -90, 0),
                             INITIAL_POS[0][1],
                             INITIAL_POS[0][2],
                             (0, 5, 90, 0),
                             1.0,0,stiff.STANDUP_STIFFNESSES),)

#Stand up for GOALIE_SQUAT
GOALIE_SQUAT_STAND_UP = ( ((0.0, 55.0, 0.0, -0),
                           ( -76.0, 10.0, -75.0, 125.0,  -10.7, -0),
                           ( -76.0, -10.0, -75.0, 125.0, -10.7, 0),
                           (0.0, -55.0, 0.0, 0),
                           .8, 0, stiff.STANDUP_STIFFNESSES),

                          ((0.0, 45.0, 0.0, -0),
                           ( -35.0, 4.0, -50.0, 125.0,  -45., -5),
                           ( -35.0, -4.0, -50.0, 125.0, -45., 5),
                           (0.0, -45.0, 0.0, 0),
                           .8, 0, stiff.STANDUP_STIFFNESSES) ,

                          ((75,10,-53,-74),
                           (-35.0,6,-61,124,-35,-6),
                           (-35.0,-6,-61,124,-35,6),
                           (75,-10,53,74),
                           0.6, 1, stiff.STANDUP_STIFFNESSES),

                          # should push the ball out from between legs
                          ((76.90,27.60,-104.33,1.58),
                           (-45.0,23.56,-15.20,123.31,-69.52,-4.48),
                           (-45.0,-31.11,-19.43,124.54,-65.30,9.06),
                           (82.09,-27.07,84.64,2.81),
                           0.5, 1, stiff.STANDUP_STIFFNESSES),

                          ((93,10,-90,-80),
                           (0,0,-60,120,-60,0),
                           (0,0,-60,120,-60,0),
                           (93,-10,90,80),
                           0.6,1, stiff.STANDUP_STIFFNESSES),
                          #Pull knees together

                          #stands up
                          (INITIAL_POS[0][0],
                           INITIAL_POS[0][1],
                           INITIAL_POS[0][2],
                           INITIAL_POS[0][3],
                           0.6,0, stiff.STANDUP_STIFFNESSES)
                          )

#**********************
#                     *
#       KICKS         *
#                     *
#**********************
def DREW_KICK(y,dist):
    if y<0:
        return mirrorMove(LEFT_D_KICK(-1*y,dist))
    elif y>=0:
        return LEFT_D_KICK(y,dist)


MAX_KICK_HIP_PITCH = -58.
MIN_KICK_HIP_PITCH = -100.
KICK_HIP_INTERVAL = MIN_KICK_HIP_PITCH - MAX_KICK_HIP_PITCH

MAX_KICK_DIST = 480.
MIN_KICK_DIST = 100.
KICK_DIST_INTERVAL = MAX_KICK_DIST - MIN_KICK_DIST
INTERVAL_RATIO = KICK_HIP_INTERVAL / KICK_DIST_INTERVAL

def LEFT_D_KICK(y,dist):
    y = 15
    dist = 400
    kick_leg_hip_roll=y*2;

    if kick_leg_hip_roll>20.0:
        kick_leg_hip_roll=20.0

    if kick_leg_hip_roll<0.0:
        kick_leg_hip_roll=0.0

    # range from 15 to 60 to balance against kick
    support_arm_shoulder_roll = -3.*kick_leg_hip_roll

    # range from -58 to -100
    # relX goes from 9 to 15, aka 0 to 6
    # dist goes from 100 to 380
    if dist > MAX_KICK_DIST:
        dist = MAX_KICK_DIST
    elif dist < MIN_KICK_DIST:
        dist = MIN_KICK_DIST

    kick_hip_pitch = -((MIN_KICK_DIST - dist) * INTERVAL_RATIO - \
        MAX_KICK_HIP_PITCH)

    return (
        #swing to the right
        ((80.,40.,-50.,-70.),
         (0.,0.,-35.,73.,-37.,-13.),
         (0.,0,-38.,76.,-37.,-13.),
         (80.,-40.,50.,70.),
         .5,0, stiff.NORMAL_STIFFNESSES),

        # Lift/cock leg
        ((80.,0.,-50.,-90.),
         (0.,kick_leg_hip_roll-10., -20.,120.,-37.,-20.),
         (0.,0,-45.,70.,-35.,-15.),
         (80.,support_arm_shoulder_roll,50.,70.),
         1.,0, stiff.NORMAL_STIFFNESSES),

        # kick left leg
        ((80.,0.,-50.,-90.),
         (0.,kick_leg_hip_roll-5., kick_hip_pitch-5, 75.,-10.,-20.),
         (0.,1.,-45.,86.,-37.,-13.),
         (80.,support_arm_shoulder_roll,50.,70.),
         .3,0, stiff.NORMAL_STIFFNESSES),

        #recover
        ((80.,40.,-50.,-70.),
         (0.,0.,-35.,73.,-45.,-13.),
         (0.,0,-45.,80.,-37.,-13.),
         (80.,-40.,50.,70.),
         .5,0, stiff.NORMAL_STIFFNESSES),

        # # return to normal position
        # ((60., 35., 0.,0.),
        #  (0.0,  0.0,  -22., 50., -30., 0.0),
        #  (0.0,  0.0,  -21., 52., -30., 0.0),
        #  (60., -35, 0., 0.),
        #  1.5,0, stiff.LOW_HEAD_STIFFNESSES)
        )

STAND_FOR_KICK_LEFT = (
    ((80.,90.,-50.,-70.),
     (0.,0.,-10.,20.,-10.,0.),
     (0.,0.,-10.,20.,-10.,0.),
     (80.,-40.,50.,70.),
     2.0,0, stiff.LEFT_FAR_KICK_STIFFNESSES),
    #swing to the right
    ((80.,40.,-50.,-70.),
     (0.,20.,-10.,20.,-10.,-20.),
     (0.,23.,-13.,20.,-10.,-20.),
     (80.,-40.,50.,70.),
     2.0,0, stiff.LEFT_FAR_KICK_STIFFNESSES) )

# NEEDS 0.4 value for right leg
LEFT_FAR_KICK = (
    #swing to the right
    ((80.,40.,-50.,-70.),
     (0.,0.,-15.,20.,-10.,0.),
     (0.,0.,-15.,20.,-10.,0.),
     (80.,-40.,50.,70.),
     0.8,0, stiff.LEFT_FAR_KICK_STIFFNESSES),

    ((80.,40.,-50.,-70.),
     (0.,15.,-10.,20.,-10.,-20.),
     (0.,15.,-10.,20.,-10.,-20.),
     (80.,-40.,50.,70.),
     0.8,0, stiff.LEFT_FAR_KICK_STIFFNESSES),

    # Lift leg leg
    ((80.,40.,-50.,-70.),
     (0.,15.,-45.,85.,-37.,-20.),
     (0.,15.,-12.,16.,-10.,-20.),
     (80.,-40.,50.,70.),
     0.8,0, stiff.LEFT_FAR_KICK_STIFFNESSES),

    # kick left leg
    ((80.,40.,-50.,-70.),
     (0.,15.,-60.,41.,-8.,-20.),
     (0.,15.,-10.,30.,-10.,-20.),
     (80.,-40.,50.,70.),
     0.11,0, stiff.LEFT_FAR_KICK_STIFFNESSES),
    # unkick foot
    ((80.,40.,-50.,-70.),
     (0.,15.,-45.,85.,-37.,-10.),
     (0.,15.,-12.,16.,-10.,-20.),
     (80.,-40.,50.,70.),
     0.2,0, stiff.LEFT_FAR_KICK_STIFFNESSES),

    # put foot down
    ((80.,40.,-50.,-70.),
     (0.,15.,-22,42.,-20,-10.),
     (0.,15.,-10.,20.,-10.,-20.),
     (80.,-40.,50.,70.),
     1.0,0, stiff.LEFT_FAR_KICK_STIFFNESSES),

    #swing to normal
    ((80.,40.,-50.,-70.),
     (0.,15.,-12.5,25.,-12.5,-20.),
     (0.,15.,-10.,20.,-10.,-20.),
     (80.,-40.,50.,70.),
     0.7,0, stiff.LEFT_FAR_KICK_STIFFNESSES),

    ((80.,40.,-50.,-70.),
     (0.,0.,-15.,20.,-10.,0.),
     (0.,0.,-15.,20.,-10.,0.),
     (80.,-40.,50.,70.),1.0,0, stiff.LOW_HEAD_STIFFNESSES)
    )


RIGHT_FAR_KICK = (
    ((80.0 , 40.0 , -50.0 , -70.0),
     (0.0 , -0.0 , -15.0 , 20.0 , -10.0 , -0.0),
     (0.0 , -0.0 , -15.0 , 20.0 , -10.0 , -0.0),
     (80.0 , -40.0 , 50.0 , 70.0),
     0.8 , 0, stiff.RIGHT_FAR_KICK_STIFFNESSES),

    ((80.0 , 40.0 , -50.0 , -70.0),
     (0.0 , -15.0 , -10.0 , 20.0 , -10.0 , 20.0),
     (0.0 , -15.0 , -10.0 , 20.0 , -10.0 , 20.0),
     (80.0 , -40.0 , 50.0 , 70.0),
     0.8 , 0, stiff.RIGHT_FAR_KICK_STIFFNESSES ),

    ((80.0 , 40.0 , -50.0 , -70.0),
     (0.0 , -15.0 , -12.0 , 16.0 , -10.0 , 20.0),
     (0.0 , -15.0 , -45.0 , 85.0 , -37.0 , 20.0),
     (80.0 , -40.0 , 50.0 , 70.0),
     0.8 , 0, stiff.RIGHT_FAR_KICK_STIFFNESSES),

    ((80.0 , 40.0 , -50.0 , -70.0),
     (0.0 , -15.0 , -10.0 , 30.0 , -10.0 , 20.0),
     (0.0 , -15.0 , -60.0 , 41.0 , -8.0 , 20.0),
     (80.0 , -40.0 , 50.0 , 70.0),
     0.11 , 0, stiff.RIGHT_FAR_KICK_STIFFNESSES),

    ((80.0 , 40.0 , -50.0 , -70.0),
     (0.0 , -15.0 , -12.0 , 16.0 , -10.0 , 20.0),
     (0.0 , -15.0 , -45.0 , 85.0 , -37.0 , 10.0),
     (80.0 , -40.0 , 50.0 , 70.0),
     0.2 , 0, stiff.RIGHT_FAR_KICK_STIFFNESSES),

    ((80.0 , 40.0 , -50.0 , -70.0),
     (0.0 , -15.0 , -10.0 , 20.0 , -10.0 , 20.0),
     (0.0 , -15.0 , -22.0 , 42.0 , -20.0 , 10.0),
     (80.0 , -40.0 , 50.0 , 70.0),
     1.0 , 0, stiff.RIGHT_FAR_KICK_STIFFNESSES),

    ((80.0 , 40.0 , -50.0 , -70.0),
     (0.0 , -15.0 , -10.0 , 20.0 , -10.0 , 20.0),
     (0.0 , -15.0 , -12.5 , 25.0 , -12.5 , 20.0),
     (80.0 , -40.0 , 50.0 , 70.0),
     0.7 , 0, stiff.RIGHT_FAR_KICK_STIFFNESSES),

    ((80.0 , 40.0 , -50.0 , -70.0),
     (0.0 , -0.0 , -15.0 , 20.0 , -10.0 , -0.0),
     (0.0 , -0.0 , -15.0 , 20.0 , -10.0 , -0.0),
     (80.0 , -40.0 , 50.0 , 70.0),
     1.0 , 0, stiff.LOW_HEAD_STIFFNESSES) )

LEFT_SHORT_SIDE_KICK = (
    #lean right
    ((80.,40.,-50.,-70.),
     (0.,17.,-25,43.5,-15,-15.),
     (0.,10.,-22.3,45,-22.5,-17),
     (80.,-40.,50.,70),
     .8,0, stiff.NORMAL_STIFFNESSES),

    # move left leg forward
    ((80.,40.,-50.,-70.),
     (0.,12.,-57.,55.,15.,7.5),
     (0.,10,-22.3,45.,-22.5,-17.),
     (80.,-40.,50.,70.),
     .4,0, stiff.NORMAL_STIFFNESSES),

    #kick
    ((80.,40.,-50.,-70.),
     (0.,0,-68.,40.,25.,-15),
     (0.,10,-22.3,45.,-22.5,-17.),
     (80.,-40.,50.,70.),
     .14,0, stiff.NORMAL_STIFFNESSES),

    #recover
    ((80.,40.,-50.,-70.),
     (0.,20.,-60.,55.,15.,7.5),
     (0.,10,-22.3,45.,-22.5,-17.),
     (80.,-40.,50.,70.),
     .3,0, stiff.NORMAL_STIFFNESSES),

    ((80.,40.,-50.,-70.),
     (0.,20.,-25,40,-15,-15.),
     (0.,10.,-22.3,45,-22.5,-14),
     (80.,-40.,50.,70),
     .4,0, stiff.NORMAL_STIFFNESSES),

    #back to normal
    ((90., 10.0, -90., -10.),
     (0.0,  0.0,  -22.3, 43.5, -21.2, 0.0),
     (0.0,  0.0,  -22.3, 43.5, -21.2, 0.0),
     (90.0,-10., 82, 13.2),
     .5,0,stiff.NORMAL_STIFFNESSES)
    )

RIGHT_SHORT_SIDE_KICK = mirrorMove(LEFT_SHORT_SIDE_KICK)

LEFT_SIDE_KICK = (
    #lean right
    ((80.,25.,-50.,-70.),
     (0.,17.,-25,43.5,-15,-15.),
     (0.,10.,-22.3,45,-22.5,-17),
     (80.,-25.,50.,70),
     .8,0, stiff.NORMAL_STIFFNESSES),

    # move left leg forward
    ((80.,25.,-50.,-70.),
     (0.,20.,-57.,55.,15.,7.5),
     (0.,10,-22.3,45.,-22.5,-17.),
     (80.,-15.,50.,70.),
     .4,0, stiff.NORMAL_STIFFNESSES),

    #kick
    ((80.,40.,-50.,-70.),
     (0.,-13,-68.,40.,25.,-20),
     (0.,10,-22.3,45.,-22.5,-17.),
     (80.,-20.,50.,70.),
     .14,0, stiff.NORMAL_STIFFNESSES),

    #recover
    ((80.,25.,-50.,-70.),
     (0.,20.,-60.,55.,15.,7.5),
     (0.,10,-22.3,45.,-22.5,-17.),
     (80.,-30.,50.,70.),
     0.3,0, stiff.NORMAL_STIFFNESSES),

    ((80.,25.,-50.,-70.),
     (0.,20.,-25,40,-15,-15.),
     (0.,10.,-22.3,45,-22.5,-14),
     (80.,-25.,50.,70),
     .4,0, stiff.NORMAL_STIFFNESSES),

    #back to normal
    ((90., 10.0, -90., -10.),
     (0.0,  0.0,  -22.3, 43.5, -21.2, 0.0),
     (0.0,  0.0,  -22.3, 43.5, -21.2, 0.0),
     (90.0,-10., 82, 13.2),
     .5,0,stiff.NORMAL_STIFFNESSES)
    )


RIGHT_SIDE_KICK = mirrorMove(LEFT_SIDE_KICK)

GOOGZ_LEFT_SIDE_KICK = (
    # lean right
    ((80.,40.,-50.,-70.),
     (0.,0.,-35.,65.,-28.,-14.),
     (0.,2,-38.,76.,-37.,-14.),
     (80.,-40.,50.,70.),
     0.8,0, stiff.NORMAL_STIFFNESSES),

    #lift left leg to side
    ((80.,0.,-50.,-70.),
     (0.,45.,-35.,65.,-10.,-10.),
     (0.,5.,-38.,76.,-37.,-22.),
     (80.,-40.,50.,70.),
     0.5 , 0, stiff.LEFT_SIDE_KICK_STIFFNESSES),

    # move leg forward
    ((80.,0.,-50.,-70.),
     (0.,40.,-65.,55.,-20.,0.),
     (0.,10.,-38.,76.,-37.,-22.),
     (90.,-90.,50.,70.),
     .2 , 0, stiff.LEFT_SIDE_KICK_STIFFNESSES),

    # kick across body
    ((80.,80.,-50.,-70.),
     (0.,-22.,-70.,25.,31.,0.),
     (0.,15.,-38.,76.,-37.,-22.),
     (90.,-10.,50.,70.),
     .2, 0, stiff.LEFT_SIDE_KICK_STIFFNESSES),

    # start recover
    ((80.,0.,-50.,-70.),
     (0.,20.,-65.,60.,-21.,0.),
     (0.,10.,-38.,76.,-37.,-22.),
     (90.,-90.,50.,70.),
     .2 , 0, stiff.LEFT_SIDE_KICK_STIFFNESSES),

    # recover
    ((80.,40.,-50.,-70.),
     (0.,0.,-35.,65.,-28.,-10.),
     (0.,2,-38.,76.,-37.,-14.),
     (80.,-40.,50.,70.),
     0.4,0, stiff.NORMAL_STIFFNESSES))

GOOGZ_RIGHT_SIDE_KICK = mirrorMove(GOOGZ_LEFT_SIDE_KICK)

# Depricated sweet moves as of 4/1/2013
# OLD_LEFT_SIDE_KICK = (
#     # lean right
#     ((80.,40.,-50.,-70.),
#      (0.,20.,-10.,20.,-10.,-20.),
#      (0.,16.,-13.,20.,-10.,-22.),
#      (80.,-40.,50.,70.),
#      2.0,0, stiff.LEFT_SIDE_KICK_STIFFNESSES),

#     (2, (0.,40.,-30.,70.,-32.,-20.),0.4 , 0, stiff.LEFT_SIDE_KICK_STIFFNESSES),

#     (2, (0.,40.,-50.,30.,20.,0.),0.4 , 0, stiff.LEFT_SIDE_KICK_STIFFNESSES),

#     ((80.,70.,-10.,-10.),
#      (0.,9.,-60.,30.,23.,0.),
#      (0.,23.,-13.,20.,-10.,-25.),
#      (80.,-40.,50.,70.),
#      0.2,0, stiff.LEFT_SIDE_KICK_STIFFNESSES),

#     ((80.,70.,-10.,-10.),
#      (0.,23.,-13.,20.,-10.,-25.),
#      (0.,20.,-13.,20.,-10.,-25.),
#      (80.,-40.,50.,70.),
#      0.8,0, stiff.LEFT_SIDE_KICK_STIFFNESSES),

#     ((80.,90.,-50.,-70.),
#      (0.,0.,-10.,20.,-10.,0.),
#      (0.,0.,-10.,20.,-10.,0.),
#      (80.,-40.,50.,70.),
#      2.0,0, stiff.LEFT_SIDE_KICK_STIFFNESSES) )


# OLD_RIGHT_SIDE_KICK = (
#     ((80.0 , 40.0 , -50.0 , -70.0),
#      (0.0 , -0.0 , -10.0 , 20.0 , -10.0 , -0.0),
#      (0.0 , -0.0 , -10.0 , 20.0 , -10.0 , -0.0),
#      (80.0 , -90.0 , 50.0 , 70.0),
#      2.0 , 0, stiff.RIGHT_SIDE_KICK_STIFFNESSES),

#     ((80.0 , 40.0 , -50.0 , -70.0),
#      (0.0 , -16.0 , -13.0 , 20.0 , -10.0 , 22.0),
#      (0.0 , -20.0 , -10.0 , 20.0 , -10.0 , 20.0),
#      (80.0 , -40.0 , 50.0 , 70.0),
#      2.0 , 0, stiff.RIGHT_SIDE_KICK_STIFFNESSES),

#     (3, (0.,-40.,-30.,70.,-32.,20.),0.4 , 0, stiff.RIGHT_SIDE_KICK_STIFFNESSES),
#     (3, (0.,-40.,-50.,30.,20.,0.),0.4 , 0, stiff.RIGHT_SIDE_KICK_STIFFNESSES),


#     ((80.0 , 40.0 , -50.0 , -70.0),
#      (0.0 , -23.0 , -13.0 , 20.0 , -10.0 , 25.0),
#      (0.0 , -9.0 , -60.0 , 30.0 , 23.0 , -0.0),
#      (80.0 , -70.0 , 10.0 , 10.0),
#      0.2 , 0, stiff.RIGHT_SIDE_KICK_STIFFNESSES),

#     ((80.0 , 40.0 , -50.0 , -70.0),
#      (0.0 , -20.0 , -13.0 , 20.0 , -10.0 , 25.0),
#      (0.0 , -23.0 , -13.0 , 20.0 , -10.0 , 25.0),
#      (80.0 , -70.0 , 10.0 , 10.0),
#      0.8 , 0, stiff.RIGHT_SIDE_KICK_STIFFNESSES),

#     ((80.0 , 40.0 , -50.0 , -70.0),
#      (0.0 , 0.0 , -10.0 , 20.0 , -10.0 , -0.0),
#      (0.0 , 0.0 , -10.0 , 20.0 , -10.0 , -0.0),
#      (80.0 , -90.0 , 50.0 , 70.0),
#      2.0 , 0, stiff.RIGHT_SIDE_KICK_STIFFNESSES) )

LEFT_LONG_BACK_KICK = (

    # lean right
    ((90.,30.,-50.,-70.),
     (0.,17.,-15.,43.5,-30.,-20.),
     (0.,10,-27.,45.,-22.5,-17.),
     (90.,-30.,50.,70.),
     0.8,0, stiff.NORMAL_STIFFNESSES),

    #lift left leg to side
    ((90.,5,-50.,-70.),
     (0.,45.,-35.,65.,-10.,-10.),
     (0.,12,-27.,45.,-22.5,-20.),
     (90.,-30.,50.,70.),
     .5 , 0, stiff.LEFT_SIDE_KICK_STIFFNESSES),

    #Swing around Leg
    ((90.,22,-75.,-86.),
     (0.,45.,-95.,25.,-10.,-10.),
     (0.,12,-27.,45.,-22.5,-17),
     (80.,-40.,50.,70.),
     .7 , 0, stiff.LEFT_SIDE_KICK_STIFFNESSES),

    #  #Set For Kick
    ((90.,22,-75.,-86.),
     (0.,17.,-95.,20.,-68.,-10.),
     (0.,13.,-27.,45.,-22.5,-17),
     (90.,-40.,50.,70.),
     .4, 0, stiff.LEFT_SIDE_KICK_STIFFNESSES),

     # KICK!!!
    ((90.,22,-75.,-86.),
     (0.,17.,-50.,65.,-16.,-10.),
     (0.,13.,-27.,45,-22.5,-17),
     (90.,-40.,50.,70.),
     .2 , 0, stiff.LEFT_SIDE_KICK_STIFFNESSES),

    # recover and we're back
    ((90.,10.,-90.,-10.),
     (0.0,0.0,-22.3,43.5,-21.2, 0.0),
     (0.0,0.0,-22.3,43.5,-21.2, 0.0),
     (90.,-10.,82.,13.2),
     .7,0,stiff.NORMAL_STIFFNESSES),)

RIGHT_LONG_BACK_KICK =  mirrorMove(LEFT_LONG_BACK_KICK)

LEFT_SHORT_BACK_KICK = (
 # lean right
    ((90.,30.,-50.,-70.),
     (0.,17.,-15.,43.5,-30.,-20.),
     (0.,10,-27.,45.,-22.5,-17.),
     (90.,-30.,50.,70.),
     0.8,0, stiff.NORMAL_STIFFNESSES),

    #lift left leg to side
    ((90.,30,-50.,-70.),
     (0.,45.,-35.,65.,-10.,-10.),
     (0.,12,-27.,45.,-22.5,-20.),
     (90.,-30.,50.,70.),
     .5 , 0, stiff.LEFT_SIDE_KICK_STIFFNESSES),

    #Swing around Leg
    ((90.,30,-75.,-86.),
     (0.,45.,-95.,25.,-10.,-10.),
     (0.,12,-27.,45.,-22.5,-17),
     (80.,-40.,50.,70.),
     .7 , 0, stiff.LEFT_SIDE_KICK_STIFFNESSES),

    #  #Set For Kick
    ((90.,30,-75.,-86.),
     (0.,17.,-95.,20.,-68.,-10.),
     (0.,13.,-27.,45.,-22.5,-17),
     (90.,-40.,50.,70.),
     .4, 0, stiff.LEFT_SIDE_KICK_STIFFNESSES),

     # KICK!!!
    ((90.,22,-75.,-86.),
     (0.,17.,-50.,65.,-16.,-10.),
     (0.,13.,-27.,45,-22.5,-17),
     (90.,-40.,50.,70.),
     .4 , 0, stiff.LEFT_SIDE_KICK_STIFFNESSES),

    # recover and we're back
    ((90.,10.,-90.,-10.),
     (0.0,0.0,-22.3,43.5,-21.2, 0.0),
     (0.0,0.0,-22.3,43.5,-21.2, 0.0),
     (90.,-10.,82.,13.2),
     .7,0,stiff.NORMAL_STIFFNESSES),)


RIGHT_SHORT_BACK_KICK =  mirrorMove(LEFT_SHORT_BACK_KICK)

LEFT_BIG_KICK = (
    #swing to the right
    ((80.,40.,-50.,-70.),
     (0.,0.,-15.,20.,-10.,0.),
     (0.,0.,-15.,20.,-10.,0.),
     (80.,-40.,50.,70.),
     0.6,0, stiff.NORMAL_STIFFNESSES),

    ((80.,40.,-50.,-70.),
     (0.,15.,-10.,20.,-10.,-20.),
     (0.,12.,-10.,20.,-8.,-18.),
     (80.,-40.,50.,70.),
     .8,0, stiff.NORMAL_STIFFNESSES),

    # Lift leg leg
    ((80.,40.,-50.,-70.),
     (0.,15.,-45.,85.,-37.,-20.),
     (0.,11,-10.,16.,-8.,-18.),
     (80.,-40.,50.,70.),
     .8,0, stiff.NORMAL_STIFFNESSES),

    #cock left leg
    ((20.,40.,0.,0.),
     (0.,15.,-22.,120.,0.,-20.),
     (0.,11.,-12.,16.,-8.,-18.),
     (100.,-40.,0.,0.),
     .8,0, stiff.NORMAL_STIFFNESSES),

    # kick left leg
    ((50.,40.,0.,0.),
     (0.,15.,-80.,60.,-5.,-20.),
     (0.,11.,-22.,30.,-8.,-18.),
     (20.,-40.,0.,0.),
     .18,0, stiff.LEFT_FAR_KICK_STIFFNESSES),

    # unkick foot
    # ((80.,40.,-50.,-70.),
    #  (0.,15.,-45.,85.,-40.,-10.),
    #  (0.,12.,-12.,16.,-10.,-18.),
    #  (80.,-40.,50.,70.),
    #  1000001.5,0, stiff.LEFT_FAR_KICK_STIFFNESSES),

    # put foot down
    # ((80.,40.,-50.,-70.),
    #  (0.,15.,-22,42.,-20,-10.),
    #  (0.,13.,-10.,20.,-10.,-19.),
    #  (80.,-40.,50.,70.),
    #  1.0,0, stiff.LEFT_FAR_KICK_STIFFNESSES),

    #swing to normal
    # ((80.,40.,-50.,-70.),
    #  (0.,15.,-12.5,25.,-12.5,-20.),
    #  (0.,15.,-10.,20.,-10.,-20.),
    #  (80.,-40.,50.,70.),
    #  0.7,0, stiff.LEFT_FAR_KICK_STIFFNESSES),

    # #swing to normal
    ((80.,40.,-50.,-70.),
     (0.,10,-15.,20.,-8.,-15.),
     (0.,0.,-15.,20.,-8.,0.),
     (80.,-40.,50.,70.),
     .7,0, stiff.LOW_HEAD_STIFFNESSES),

    # Wait a second, it's a rough recovery...
    ((80.,40.,-50.,-70.),
     (0.,0.,-15.,20.,-10.,0.),
     (0.,0.,-15.,20.,-10.,0.),
     (80.,-40.,50.,70.),
     1.0,0, stiff.LOW_HEAD_STIFFNESSES)
    )

RIGHT_BIG_KICK = mirrorMove(LEFT_BIG_KICK)

LEFT_STRAIGHT_KICK = (
    #swing to the right
    ((20.,30.,0.,0.),
     (0.,17.,-15,43.5,-30,-20.),
     (0.,10.,-27,45,-22.5,-17),
     (80.,-30.,0.,0),
     .8,0, stiff.NORMAL_STIFFNESSES),

    # Lift/cock leg
    ((20.,30.,0.,0.),
     (0.,17, -30.,100.,-40.,-25.),
     (0.,10,-27,45.,-22.5,-18),
     (100.,-30,0.,0),
     .3,0, stiff.NORMAL_STIFFNESSES),

    # Kick?
    ((43.,30.,0.,0.),
     (0.,17, -60.,70.,-10,-15.),
     (0.,10,-27,45.,-22.5,-18),
     (20.,-30,0, 0),
     .16,0, stiff.NORMAL_STIFFNESSES),

    # Recover
    ((80.,30.,-50.,-70.),
     (0.,25.,-27.,43.5,-21.2,-20.),
     (0.,15,-27,45.,-22.5,-18.),
     (80.,-30.,50.,74.),
     .4,0, stiff.NORMAL_STIFFNESSES),

    ((90.,10.,-90.,-10.),
     (0.0,0.0,-22.3,43.5,-21.2, 0.0),
     (0.0,0.0,-22.3,43.5,-21.2, 0.0),
     (90.,-10.,82.,13.2),
     .7,0,stiff.NORMAL_STIFFNESSES),)

RIGHT_STRAIGHT_KICK = mirrorMove(LEFT_STRAIGHT_KICK)

LEFT_QUICK_STRAIGHT_KICK = (
    #stand for a bit
    ((90.,10.,-90.,-10.),
     (0.0,0.0,-22.3,43.5,-21.2, 0.0),
     (0.0,0.0,-22.3,43.5,-21.2, 0.0),
     (90.,-10.,82.,13.2),
     .5,0,stiff.NORMAL_STIFFNESSES),

    #push off...
    ((90.,10.,-90.,-10.),
     (0.0,0.0,-22.3,43.5,-21.2, 0.0),
     (0.0,0.0,-22.3,43.5, -18, 0.0),
     (90.,-10.,82.,13.2),
     .1,0,stiff.NORMAL_STIFFNESSES),

    #lean right/lift leg
    ((20.,30.,0.,0.),
     (0.,17.,-30,70,-45,-25.),
     (0.,13.,-22.3,45,-22.5,-17),
     (100.,-30.,0.,0),
     .4,0, stiff.NORMAL_STIFFNESSES),

    #kick?
    ((43.,30.,0.,0.),
     (0.,17.,-65,60,0,-20.),
     (0.,13.,-22.3,45,-22.5,-17),
     (40.,-30.,0.,0),
     .18,0, stiff.NORMAL_STIFFNESSES),

    #recover
    ((35.,30.,0.,0.),
     (0.,25.,-35,65,-30,-25.),
     (0.,10.,-22.3,45,-22.5,-17),
     (90.,-30.,0.,0),
     .4,0, stiff.NORMAL_STIFFNESSES),

    #back to normal
    ((90.,10.,-90.,-10.),
     (0.0,0.0,-22.3,43.5,-21.2, 0.0),
     (0.0,0.0,-22.3,43.5,-21.2, 0.0),
     (90.,-10.,82.,13.2),
     .7,0,stiff.NORMAL_STIFFNESSES),

    #stand for a bit
    ((90.,10.,-90.,-10.),
     (0.0,0.0,-22.3,43.5,-21.2, 0.0),
     (0.0,0.0,-22.3,43.5,-21.2, 0.0),
     (90.,-10.,82.,13.2),
     1,0,stiff.NORMAL_STIFFNESSES),
)

RIGHT_QUICK_STRAIGHT_KICK = mirrorMove(LEFT_QUICK_STRAIGHT_KICK)

LEFT_SHORT_STRAIGHT_KICK = (
    #stand for a bit
    ((90.,10.,-90.,-10.),
     (0.0,0.0,-22.3,43.5,-21.2, 0.0),
     (0.0,0.0,-22.3,43.5,-21.2, 0.0),
     (90.,-10.,82.,13.2),
     .5,0,stiff.NORMAL_STIFFNESSES),

    #push off...
    ((90.,10.,-90.,-10.),
     (0.0,0.0,-22.3,43.5,-21.2, 0.0),
     (0.0,0.0,-22.3,43.5, -18, 0.0),
     (90.,-10.,82.,13.2),
     .1,0,stiff.NORMAL_STIFFNESSES),

    #lean right/lift leg
    ((20.,30.,0.,0.),
     (0.,17.,-30,70,-45,-25.),
     (0.,13.,-22.3,45,-22.5,-17),
     (100.,-30.,0.,0),
     .4,0, stiff.NORMAL_STIFFNESSES),

    #kick?
    ((43,30.,0.,0.),
     (0.,17.,-65,60, -5,-20.),
     (0.,13.,-22.3,45,-22.5,-17),
     (40.,-30.,0.,0),
     .30,0, stiff.NORMAL_STIFFNESSES),

    #recover
    ((35.,30.,-0.,-0.),
     (0.,25.,-35,65,-30,-25.),
     (0.,10.,-22.3,45,-22.5,-17),
     (90.,-30.,0.,0),
     .4,0, stiff.NORMAL_STIFFNESSES),

    #back to normal
    ((90.,10.,-90.,-10.),
     (0.0,0.0,-22.3,43.5,-21.2, 0.0),
     (0.0,0.0,-22.3,43.5,-21.2, 0.0),
     (90.,-10.,82.,13.2),
     .7,0,stiff.NORMAL_STIFFNESSES),

    #stand for a bit
    ((90.,10.,-90.,-10.),
     (0.0,0.0,-22.3,43.5,-21.2, 0.0),
     (0.0,0.0,-22.3,43.5,-21.2, 0.0),
     (90.,-10.,82.,13.2),
     .8,0,stiff.NORMAL_STIFFNESSES),
)

RIGHT_SHORT_STRAIGHT_KICK = mirrorMove(LEFT_SHORT_STRAIGHT_KICK)

LEFT_SHORTER_STRAIGHT_KICK = (
    #lean right/lift leg
    ((80.,40.,-50.,-70.),
     (0.,17.,-35,70,-45,-25.),
     (0.,12.,-22.3,45,-22.5,-17),
     (80.,-40.,50.,70),
     .3,0, stiff.NORMAL_STIFFNESSES),

    #kick?
    ((80.,40.,-50.,-70.),
     (0.,17.,-55,75,-20,-20.),
     (0.,10.,-22.3,45,-22.5,-17),
     (80.,-40.,50.,70),
     .15,0, stiff.NORMAL_STIFFNESSES),

    #recover
    ((80.,40.,-50.,-70.),
     (0.,25.,-25,50,-20,-25.),
     (0.,10.,-22.3,45,-22.5,-17),
     (80.,-40.,50.,70),
     .6,0, stiff.NORMAL_STIFFNESSES),

    #back to normal
    ((90.,10.,-90.,-10.),
     (0.0,0.0,-22.3,43.5,-21.2, 0.0),
     (0.0,0.0,-22.3,43.5,-21.2, 0.0),
     (90.,-10.,82.,13.2),
     .5,0,stiff.NORMAL_STIFFNESSES),
    )

RIGHT_SHORTER_STRAIGHT_KICK = mirrorMove(LEFT_SHORTER_STRAIGHT_KICK)

# Deprecated sweet moves as of 4/1/2013
# OLD_SHORT_LEFT_SIDE_KICK = (
#     ((80.,90.,-50.,-70.),
#      (0.,0.,-10.,20.,-10.,0.),
#      (0.,0.,-10.,20.,-10.,0.),
#      (80.,-40.,50.,70.),
#      2.0,0, stiff.LEFT_SIDE_KICK_STIFFNESSES),

#     ((80.,40.,-50.,-70.),
#      (0.,20.,-10.,20.,-10.,-20.),
#      (0.,16.,-13.,20.,-10.,-22.),
#      (80.,-40.,50.,70.),
#      2.0,0, stiff.LEFT_SIDE_KICK_STIFFNESSES),

#     (2, (0.,40.,-30.,70.,-32.,-20.), 0.4 , 0, stiff.LEFT_SIDE_KICK_STIFFNESSES),
#     (2, (0.,40.,-50.,30.,20.,0.), 0.4 , 0, stiff.LEFT_SIDE_KICK_STIFFNESSES),

#     ((80.,70.,-10.,-10.),
#      (0.,26.,-60.,30.,23.,0.),
#      (0.,23.,-13.,20.,-10.,-25.),
#      (80.,-40.,50.,70.),
#      0.2,0, stiff.LEFT_SIDE_KICK_STIFFNESSES),

#     ((80.,70.,-10.,-10.),
#      (0.,23.,-13.,20.,-10.,-25.),
#      (0.,20.,-13.,20.,-10.,-25.),
#      (80.,-40.,50.,70.),
#      0.8,0, stiff.LEFT_SIDE_KICK_STIFFNESSES),

#     ((80.,90.,-50.,-70.),
#      (0.,0.,-10.,20.,-10.,0.),
#      (0.,0.,-10.,20.,-10.,0.),
#      (80.,-40.,50.,70.),
#      2.0,0, stiff.LEFT_SIDE_KICK_STIFFNESSES) )

# OLD_SHORT_RIGHT_SIDE_KICK = (
#     ((80.0, 40.0, -50.0, -70.0),
#      (0.0, -0.0, -10.0, 20.0, -10.0, -0.0),
#      (0.0, -0.0, -10.0, 20.0, -10.0, -0.0),
#      (80.0, -90.0, 50.0, 70.0),
#      2.0, 0, stiff.RIGHT_SIDE_KICK_STIFFNESSES ),

#     ((80.0, 40.0, -50.0, -70.0),
#      (0.0, -16.0, -13.0, 20.0, -10.0, 22.0),
#      (0.0, -20.0, -10.0, 20.0, -10.0, 20.0),
#      (80.0, -40.0, 50.0, 70.0),
#      2.0, 0, stiff.RIGHT_SIDE_KICK_STIFFNESSES ),

#     (3, (0.,-40.,-30.,70.,-32.,20.), 0.4 , 0, stiff.LEFT_SIDE_KICK_STIFFNESSES),
#     (3, (0.,-40.,-50.,30.,20.,0.), 0.4 , 0, stiff.LEFT_SIDE_KICK_STIFFNESSES),

#     ((80.0, 40.0, -50.0, -70.0),
#      (0.0, -23.0, -13.0, 20.0, -10.0, 25.0),
#      (0.0, -26.0, -60.0, 30.0, 23.0, -0.0),
#      (80.0, -70.0, 10.0, 10.0),
#      0.2, 0, stiff.RIGHT_SIDE_KICK_STIFFNESSES),

#     ((80.0, 40.0, -50.0, -70.0),
#      (0.0, -20.0, -13.0, 20.0, -10.0, 25.0),
#      (0.0, -23.0, -13.0, 20.0, -10.0, 25.0),
#      (80.0, -70.0, 10.0, 10.0),
#      0.8, 0, stiff.RIGHT_SIDE_KICK_STIFFNESSES ),

#     ((80.0, 40.0, -50.0, -70.0),
#      (0.0, -0.0, -10.0, 20.0, -10.0, -0.0),
#      (0.0, -0.0, -10.0, 20.0, -10.0, -0.0),
#      (80.0, -90.0, 50.0, 70.0),
#      2.0, 0, stiff.RIGHT_SIDE_KICK_STIFFNESSES))

LEARN_KICK_LEFT = (
    ((80.,90.,-50.,-70.),
     (0.,0.,-10.,20.,-10.,0.),
     (0.,0.,-10.,20.,-10.,0.),
     (80.,-40.,50.,70.),
     2.0,0, stiff.LEARN_MOTION_STIFFNESSES),
    #swing to the right
    ((80.,40.,-50.,-70.),
     (0.,20.,-10.,20.,-10.,-20.),
     (0.,23.,-13.,20.,-10.,-20.),
     (80.,-40.,50.,70.),
     2.0,0, stiff.LEARN_MOTION_STIFFNESSES) )
