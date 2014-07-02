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

#Keyframe that moves can called from an unbalanced position to slowly return the joints to a stable, standing position.
INITIAL_POS_KEYFRAME = ((90.,10.,-90.,-10.),
			(0.0,0.0,-22.3,43.5,-21.2, 0.0),
			(0.0,0.0,-22.3,43.5,-21.2, 0.0),
			(90.,-10.,82.,13.2),
			2,0,stiff.NORMAL_STIFFNESSES)

#Keyframe that moves the joints to a stable, standing position quickly.
QUICK_INITIAL_POS_KEYFRAME = ((90.,10.,-90.,-10.),
			(0.0,0.0,-22.3,43.5,-21.2, 0.0),
			(0.0,0.0,-22.3,43.5,-21.2, 0.0),
			(90.,-10.,82.,13.2),
			0.8,0,stiff.NORMAL_STIFFNESSES)

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

STAND_UP_FRONT = (#Initial
                  ((74.9, 6.4, 94.8, -20.7),
                   (3.1, 1.9, 0.4, -4.3, 55.3, -0.8),
                   (0.0, 0.1, 2.5, -5.8, 61.1, 2.2),
                   (74.9, -6.4, -93.2, 19.6),
                  .4,1, stiff.STANDUP_STIFFNESSES ),

                  #Begin wrap around
                  ((69.7, -5.4, 100.9, -88.5),
                   (-68.8, 46.5, -25.7, -4.5, 27.0, -4.8),
                   (0.0, -47.1, -23.0, -0.2, 12.0, -2.6),
                   (69.5, 5.7, -100.4, 88.5),
                  .4,1, stiff.STANDUP_STIFFNESSES ),

                   #Feet in front
                  ((117.3, -40.3, 62.2, -89.8),
                   (-70.0, 41.3, -87.3, 11.4, -25.5, 32.9),
                   (0.0, -44.1, -74.4, -4.5, -25.3, -34.3),
                   (117.3, 41.3, -62.2, 89.8),
                   .4,1, stiff.STANDUP_STIFFNESSES ),

                  #Sit Back
                  ((111.7, -20.3, 69.5, -89.6),
                   (-69.4, -18.8, -89.1, -7.5, -16.9, 35.7),
                   (0.0, 11.7, -86.3, -6.5, -25.1, -33.7),
                   (106.9, 21.8, -59.2, 89.5),
                  .4,1, stiff.STANDUP_STIFFNESSES ),

                  #Arms behind sitting
                  ((120.8, -10.1, 0.2, -1.9),
                   (-23.1, 13.3, -88.7, 14.5, 32.4, 0.3),
                   (0.0, 1.3, -84.3, 13.4, 48.6, 2.0),
                   (117.5, 10.1, -13.0, 1.4),
                  .4,1, stiff.STANDUP_STIFFNESSES ),

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

                 ((77.5, 28.9, 5.6, -40.7),
                  (-30.1, 0.6, -28.2, 123.4, -69.9, 3.2),
                  (0.0, -15.5, -55.8, 123.7, -43.5, 7.2),
                  (88.1, -22.6, -1.0, 23.0),
                  0.6,1, stiff.STANDUP_STIFFNESSES),
                 #pulls both legs in, squat

                 #stands up
                 (INITIAL_POS[0][0],
                  INITIAL_POS[0][1],
                  INITIAL_POS[0][2],
                  INITIAL_POS[0][3],
                  1,0, stiff.STANDUP_STIFFNESSES) )

# new robots
STAND_UP_BACK = (
                 (INITIAL_POS[0][0],
                  INITIAL_POS[0][1],
                  INITIAL_POS[0][2],
                  INITIAL_POS[0][3],
                  1,0, stiff.STANDUP_STIFFNESSES),

                 ((100,0,0,0),
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

                 #((67.9, -6.9, 66.9, -90.8),
                  #(-15.4, 20.0, -90.0, 121.0, 17.6, 10.9),
                  #(-15.4, -18.8, -89.8, 121.5, 22.0, -7.5),
                  #(69.2, 14.1, -71.6, 90.3),
                  #.7,1, stiff.STANDUP_STIFFNESSES),
                  #knees up

                 ((57.6, -3.2, 74.9, -90.9),
                  (-19.9, -1.1, 13.0, -7.1, 17.7, 23.0),
                  (0.0, -1.7, 10.3, -6.3, 22.1, -7.2),
                  (58.2, 8.9, -81.0, 89.8),
                  .3,1, stiff.STANDUP_STIFFNESSES),
                  #kick for momentum

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

                 ((77.5, 28.9, 5.6, -40.7),
                  (-30.1, 0.6, -28.2, 123.4, -69.9, 3.2),
                  (0.0, -15.5, -55.8, 123.7, -43.5, 7.2),
                  (88.1, -22.6, -1.0, 23.0),
                  0.6,1, stiff.STANDUP_STIFFNESSES),
                 #pulls both legs in, squat

                 #stands up
                 (INITIAL_POS[0][0],
                  INITIAL_POS[0][1],
                  INITIAL_POS[0][2],
                  INITIAL_POS[0][3],
                  1,0, stiff.STANDUP_STIFFNESSES)
                 )

#**********************
#                     *
#       GOALIE        *
#                     *
#**********************

#Squats with legs in M shape and arms at a 45 degree angle
GOALIE_SQUAT = (((-0.0, 90.0, 0.0, 0.0),
                 (-76.0, 28.0, -54.0, 86.0, 0.0, -11.0),
                 (-76.0, -28.0, -54.0, 86.0, 0.0, 11.0),
                 (-0.0, -90.0, -0.0, -0.0),
                 0.4, 1, stiff.NORMAL_STIFFNESSES),
                # goalieSquatPart2
                ((-0.0, 45.0, 0.0, 0.0),
                 (-90.0, 38.0, -75.0, 90.0, 30.0, -20.0),
                 (-90.0, -38.0, -75.0, 90.0, 30.0, 20.0),
                 (-0.0, -45.0, -0.0, -0.0),
                 0.2, 1, stiff.NORMAL_STIFFNESSES),
                )

#Stand up for GOALIE_SQUAT
GOALIE_SQUAT_STAND_UP = ( ((0.0, 55.0, 0.0, -0),
                           ( -76.0, 10.0, -75.0, 125.0,  -10.7, -0),
                           ( -76.0, -10.0, -75.0, 125.0, -10.7, 0),
                           (0.0, -55.0, 0.0, 0),
                           .4, 0, stiff.STANDUP_STIFFNESSES),

                          ((0.0, 45.0, 0.0, -0),
                           ( -35.0, 4.0, -50.0, 125.0,  -45., -5),
                           ( -35.0, -4.0, -50.0, 125.0, -45., 5),
                           (0.0,-45.0, 0.0, 0),
                           .4, 0, stiff.STANDUP_STIFFNESSES) ,

                          ((75,10,-53,-74),
                           (-35.0,6,-61,124,-35,-6),
                           (-35.0,-6,-61,124,-35,6),
                           (75,-10,53,74),
                           0.3, 1, stiff.STANDUP_STIFFNESSES),

                          # should push the ball out from between legs
                          ((76.90,27.60,-104.33,1.58),
                           (-45.0,23.56,-15.20,123.31,-69.52,-4.48),
                           (-45.0,-31.11,-19.43,124.54,-65.30,9.06),
                           (82.09,-27.07,84.64,2.81),
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
                           0.8,0, stiff.STANDUP_STIFFNESSES)
                          )
#MEGAN AND NIKKI TEST ROLL OUT
GOALIE_ROLL_OUT_MEGAN = ( (INITIAL_POS[0][0],
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

                      ((19.6, 46.8, 2.5, -1.6),
                       (-10.0, 0.1, -1.4, 17.7, 30.1, -10.7),
                       (0.0, 5.3, -6.4, 13.8, 30.1, -9.1),
                       (-61.3, 2.6, -51.7, 6.4),
                       .3, 1, stiff.GOALIE_DIVE_RIGHT_STIFFNESSES),
                      #move shoulder

                      ((19.6, 45.6, 2.3, -2.6),
                       (-8.3, 12.0, 4.3, 17.7, 30.1, -10.8),
                       (0.0, 7.4, -36.0, 15.1, 30.1, -4.1),
                       (-28.7, 9.1, -51.7, 2.6),
                       .3, 1, stiff.GOALIE_DIVE_RIGHT_STIFFNESSES),
                      #move leg

                      ((19.7, 45.6, 2.3, -2.6),
                       (-8.2, -5.8, 9.1, 17.7, 30.1, -10.7),
                       (0.0, 11.0, -41.2, 15.1, 30.1, -4.5),
                       (-28.7, 8.1, -51.7, 2.6),
                       .3, 1, stiff.GOALIE_DIVE_RIGHT_STIFFNESSES),
                      #move other shoulder 

                      ((19.7, 45.6, 2.3, -2.6),
                       (-4.6, -12.6, 8.9, 17.3, 30.0, -10.2),
                       (0.0, 12.6, -39.5, 15.1, 30.1, -4.7),
                       (-28.7, 8.1, -51.7, 2.6),
                       .3, 1, stiff.GOALIE_DIVE_RIGHT_STIFFNESSES),

                      ((19.7, 45.6, 2.3, -2.6),
                       (-13.0, -6.8, 9.1, 17.0, 26.9, -10.7),
                       (0.0, 12.7, -39.4, 14.2, 30.1, -1.6),
                       (-28.7, 8.1, -51.7, 2.6),
                       .3, 1, stiff.GOALIE_DIVE_RIGHT_STIFFNESSES),


                      ((19.7, 45.6, 2.3, -2.6),
                       (-30.5, -6.2, 13.4, 17.3, 28.1, -10.5),
                       (0.0, -5.8, -36.1, 14.4, 30.1, -1.2),
                       (-28.7, 8.1, -51.7, 2.6),
                       .3, 1, stiff.GOALIE_DIVE_RIGHT_STIFFNESSES),


                      ((122.5, 23.5, -3.3, -3.8),
                       (-31.0, -6.4, 14.3, 17.2, 28.0, -10.3),
                       (0.0, -14.8, -18.5, 14.2, 30.0, -0.7),
                       (47.4, -36.7, -55.9, 3.4),
                       .3, 1, stiff.GOALIE_DIVE_RIGHT_STIFFNESSES),
                       #okay legs

                      ((98.3, 23.5, 2.5, -3.7),
                       (-33.1, 5.1, 16.1, 13.0, 28.7, -4.7),
                       (0.0, -26.5, -3.5, 14.2, 30.0, -0.5),
                       (79.0, -37.4, -55.8, 4.0),
                       .3, 1, stiff.GOALIE_DIVE_RIGHT_STIFFNESSES),
                       #legs




"""
                      ((19.7, 45.6, 2.3, -2.6),
                       (-30.1, -3.7, 23.6, 1.6, 28.1, -10.1),
                       (0.0, -30.8, 2.9, 6.7, 30.1, -1.8),
                       (-28.7, 8.1, -51.7, 2.6),
                       .3, 1, stiff.GOALIE_DIVE_RIGHT_STIFFNESSES),

                      ((63.7, 45.3, 2.3, -3.0),
                       (-12.1, -6.3, -19.2, 17.7, 30.1, -10.4),
                       (0.0, 20.8, -53.4, 15.1, 30.1, -3.6),
                       (16.0, -7.8, -36.6, 2.8),
                       .3, 1, stiff.GOALIE_DIVE_RIGHT_STIFFNESSES),
                      #move other leg and shoulder was .4

                      ((63.7, 45.3, 2.3, -3.0),
                       (-10.3, -3.0, 12.7, 17.6, 29.3, -10.4),
                       (0.0, 20.8, -53.4, 15.1, 30.1, -3.6),
                       (16.0, -7.8, -36.6, 2.8),
                       .3, 1, stiff.GOALIE_DIVE_RIGHT_STIFFNESSES),
                      #move left leg  

                      ((63.7, 45.3, 2.3, -3.0),
                       (-10.3, -3.0, 12.7, 17.6, 29.3, -10.4),
                       (0.0, 20.8, -53.4, 15.1, 30.1, -3.6),
                       (25.1, -45.8, -49.6, 36.7),
                       .1, 1, stiff.GOALIE_DIVE_RIGHT_STIFFNESSES),
                      #move arm 


                      ((105.2, 44.9, 1.6, -3.0),
                       (-9.9, -3.0, 5.2, 16.2, 29.4, -9.9),
                       (0.0, -2.5, 3.8, 15.1, 30.1, -3.7),
                       (29.9, -74.6, -59.5, 29.6),
                       .3, 1, stiff.GOALIE_DIVE_RIGHT_STIFFNESSES),
                      #remove stiffness 

                      ((100.6, 45.2, 1.6, -2.6),
                       (-10.5, -3.1, 4.5, 16.1, 29.3, -9.9),
                       (0.0, -2.6, 3.9, 15.0, 30.1, -3.7),
                       (81.3, -64.6, -59.7, 15.6),
                       .3, 1, stiff.GOALIE_DIVE_RIGHT_STIFFNESSES),
                      #yeah was .5


                      ((102.2, 41.8, 2.3, -2.9),
                       (-10.4, -3.0, 3.7, 16.3, 29.4, -10.3),
                       (0.0, 20.1, -54.1, 13.7, 30.0, -3.6),
                       (25.6, -47.0, -50.1, 37.1),
                       .2, 1, stiff.GOALIE_DIVE_RIGHT_STIFFNESSES),
                      #yeah


                      ((98.9, 41.9, 2.2, -2.7),
                       (-10.6, -3.0, 3.7, 16.3, 29.4, -10.3),
                       (0.0, -1.1, 6.3, 10.4, 30.0, -3.8),
                       (96.4, -21.0, -50.2, 1.6),
                       .2, 1, stiff.GOALIE_DIVE_RIGHT_STIFFNESSES),
                      #yeah
"""

                         )



#MEGAN AND NIKKI TEST ROLL OUT
GOALIE_ROLL_OUT_APRIL_FOOLS = ( (INITIAL_POS[0][0],
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

                      ((19.6, 46.8, 2.5, -1.6),
                       (-10.0, 0.1, -1.4, 17.7, 30.1, -10.7),
                       (0.0, 5.3, -6.4, 13.8, 30.1, -9.1),
                       (-61.3, 2.6, -51.7, 6.4),
                       .9, 1, stiff.GOALIE_DIVE_RIGHT_STIFFNESSES),
                      #move shoulder

                      ((19.6, 45.6, 2.3, -2.6),
                       (-8.3, 12.0, 4.3, 17.7, 30.1, -10.8),
                       (0.0, 7.4, -36.0, 15.1, 30.1, -4.1),
                       (-28.7, 9.1, -51.7, 2.6),
                       1.5, 1, stiff.GOALIE_DIVE_RIGHT_STIFFNESSES),
                      #move leg

                      ((19.7, 45.6, 2.3, -2.6),
                       (-8.2, -5.8, 9.1, 17.7, 30.1, -10.7),
                       (0.0, 11.0, -41.2, 15.1, 30.1, -4.5),
                       (-28.7, 8.1, -51.7, 2.6),
                       1.0, 1, stiff.GOALIE_DIVE_RIGHT_STIFFNESSES),
                      #move other shoulder

                      ((63.7, 45.3, 2.3, -3.0),
                       (-12.1, -6.3, -19.2, 17.7, 30.1, -10.4),
                       (0.0, 20.8, -53.4, 15.1, 30.1, -3.6),
                       (16.0, -7.8, -36.6, 2.8),
                       1.0, 1, stiff.GOALIE_DIVE_RIGHT_STIFFNESSES),
                      #move other leg and shoulder

                      ((63.7, 45.3, 2.3, -3.0),
                       (-10.3, -3.0, 12.7, 17.6, 29.3, -10.4),
                       (0.0, 20.8, -53.4, 15.1, 30.1, -3.6),
                       (16.0, -7.8, -36.6, 2.8),
                       .5, 1, stiff.GOALIE_DIVE_RIGHT_STIFFNESSES),
                      #move left leg

                      ((63.7, 45.3, 2.3, -3.0),
                       (-10.3, -3.0, 12.7, 17.6, 29.3, -10.4),
                       (0.0, 20.8, -53.4, 15.1, 30.1, -3.6),
                       (25.1, -45.8, -49.6, 36.7),
                       .5, 1, stiff.GOALIE_DIVE_RIGHT_STIFFNESSES),
                      #move arm


                      ((105.2, 44.9, 1.6, -3.0),
                       (-9.9, -3.0, 5.2, 16.2, 29.4, -9.9),
                       (0.0, -2.5, 3.8, 15.1, 30.1, -3.7),
                       (29.9, -74.6, -59.5, 29.6),
                       .5, 1, stiff.GOALIE_DIVE_RIGHT_STIFFNESSES),
                      #remove stiffness

                      ((100.6, 45.2, 1.6, -2.6),
                       (-10.5, -3.1, 4.5, 16.1, 29.3, -9.9),
                       (0.0, -2.6, 3.9, 15.0, 30.1, -3.7),
                       (81.3, -64.6, -59.7, 15.6),
                       .5, 1, stiff.GOALIE_DIVE_RIGHT_STIFFNESSES),
                      #yeah


                      ((102.2, 41.8, 2.3, -2.9),
                       (-10.4, -3.0, 3.7, 16.3, 29.4, -10.3),
                       (0.0, 20.1, -54.1, 13.7, 30.0, -3.6),
                       (25.6, -47.0, -50.1, 37.1),
                       .2, 1, stiff.GOALIE_DIVE_RIGHT_STIFFNESSES),
                      #yeah

                      ((99.8, 41.9, 2.2, -2.7),
                       (-10.5, -3.0, 3.7, 16.4, 29.4, -10.4),
                       (0.0, 2.1, -38.2, 10.4, 30.0, -3.6),
                       (25.7, -51.9, -50.1, 37.1),
                       .2, 1, stiff.GOALIE_DIVE_RIGHT_STIFFNESSES),
                      #yeah

                      ((99.8, 41.8, 2.2, -2.7),
                       (-10.5, -3.0, 3.7, 16.3, 29.4, -10.3),
                       (0.0, -1.1, 6.3, 10.4, 30.0, -3.7),
                       (25.7, -52.0, -50.2, 37.1),
                       .2, 1, stiff.GOALIE_DIVE_RIGHT_STIFFNESSES),
                      #yeah

                      ((98.9, 41.9, 2.2, -2.7),
                       (-10.6, -3.0, 3.7, 16.3, 29.4, -10.3),
                       (0.0, -1.1, 6.3, 10.4, 30.0, -3.8),
                       (96.4, -21.0, -50.2, 1.6),
                       .2, 1, stiff.GOALIE_DIVE_RIGHT_STIFFNESSES),
                      #yeah


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
GOALIE_ROLL_OUT_RIGHT = ( ((0,70.92,0,0),
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
    #pause for dramatic effect
    ((90., 10.0, -90., -10.),
     (0.0,  0.0,  -22.3, 43.5, -21.2, 0.0),
     (0.0,  0.0,  -22.3, 43.5, -21.2, 0.0),
     (90.0,-10., 82, 13.2),
     .6,0,stiff.NORMAL_STIFFNESSES),

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
     (0.,40.,-65.,55., 30.,0.),
     (0.,10.,-38.,76.,-37.,-22.),
     (90.,-90.,50.,70.),
     .2 , 0, stiff.LEFT_SIDE_KICK_STIFFNESSES),

    # kick across body
    ((80.,80.,-50.,-70.),
     (0.,-22.,-70.,25.,37.,0.),
     (0.,15.,-38.,76.,-37.,-22.),
     (90.,-10.,50.,70.),
     .2, 0, stiff.LEFT_SIDE_KICK_STIFFNESSES),

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
     .6,0,stiff.NORMAL_STIFFNESSES))

    # # start recover
    # ((80.,0.,-50.,-70.),
    #  (0.,20.,-65.,60.,-21.,0.),
    #  (0.,10.,-38.,76.,-37.,-22.),
    #  (90.,-90.,50.,70.),
    #  .2 , 0, stiff.LEFT_SIDE_KICK_STIFFNESSES),

    # # recover
    # ((80.,40.,-50.,-70.),
    #  (0.,0.,-35.,65.,-28.,-10.),
    #  (0.,2,-38.,76.,-37.,-14.),
    #  (80.,-40.,50.,70.),
    #  0.4,0, stiff.NORMAL_STIFFNESSES))

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

    # lift left leg to side
    ((90.,30.,-50.,-70.),
     (0.,45.,-35.,65.,-10.,-10.),
     (0.,12,-27.,45.,-22.5,-20.),
     (90.,-30.,50.,70.),
     .5 , 0, stiff.LEFT_SIDE_KICK_STIFFNESSES),

    # swing around leg
    ((90.,22,-75.,-86.),
     (0.,45.,-95.,25.,-10.,-10.),
     (0.,12,-27.,45.,-22.5,-17),
     (120, -7, 80, 13),
     .7 , 0, stiff.LEFT_SIDE_KICK_STIFFNESSES),

    # set For kick
    ((90.,22,-75.,-86.),
     (0.,17.,-95.,20.,-68.,-10.),
     (0.,13.,-27.,45.,-22.5,-17),
     (120, -7, 80, 13),
     .4, 0, stiff.LEFT_SIDE_KICK_STIFFNESSES),

     # kick
    ((90.,22,-75.,-86.),
     (0.,17.,-50.,65.,-16.,-10.),
     (0.,13.,-27.,45,-22.5,-17),
     (90.,-40.,50.,70.),
     .2 , 0, stiff.LEFT_SIDE_KICK_STIFFNESSES),

    # recover, arms out for stability
    ((35, 3, -90, 0),
     (0.0,0.0,-22.3,43.5,-21.2, 0.0),
     (0.0,0.0,-22.3,43.5,-21.2, 0.0),
     (35, -3, 90, 0),
     .7,0,stiff.NORMAL_STIFFNESSES),

    # same thing, extra time for stability
    ((35, 3, -90, 0),
     (0.0,0.0,-22.3,43.5,-21.2, 0.0),
     (0.0,0.0,-22.3,43.5,-21.2, 0.0),
     (35, -3, 90, 0),
     .7,0,stiff.NORMAL_STIFFNESSES),

    # we're back
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

    #Raise stable arms
    ((80.,40.,-50.,-70.),
     (0.,0.,-15.,20.,-10.,0.),
     (0.,0.,-15.,20.,-10.,0.),
     (80.,-40.,50.,70.),
     0.7,1, stiff.NORMAL_STIFFNESSES),

    #Lean right
     ((65.2, 70.8, -52.5, -44.3),
     (0.,15.,-20.,20.,-10.,-20.),
     (0.,12.,-20.,20.,-8.,-18.),
     (80.,-40.,50.,70.),
     0.6,1, stiff.NORMAL_STIFFNESSES),

    #Lift left leg
    ((65.2, 70.8, -52.5, -44.3),
     (0.,15.,-55.,85.,0,-20.),
     (0.,11,-20.,16.,-8.,-18.),
     (80.,-40.,50.,70.),
     0.5,0, stiff.NORMAL_STIFFNESSES),

    #Cock left leg
    ((20.,40.,0.,0.),
    (0.,15.,-32.,120.,0.,-20.),
    (0.,11.,-22.,16.,-8.,-18.),
    (100.,-40.,0.,0.),
     0.6,0, stiff.NORMAL_STIFFNESSES),

    #Kick left leg
    ((50.,40.,0.,0.),
     (0.,15.,-80.,60.,-5.,-20.),
     (0.,11.,-22.,30.,-8.,-18.),
     (20.,-40.,0.,0.),
     0.2,0, stiff.LEFT_FAR_KICK_STIFFNESSES),

     # move foot down
     ((65.2, 8.9, -0.2, -1.5),
      (-4.5, 4.6, -36.2, 34.0, -7.8, -6.1),
      (0.0, 6.2, -35.3, 37.6, -12.9, -6.0),
      (62.4, -6.6, 0.2, 2.5),
      0.6,0, stiff.NORMAL_STIFFNESSES),

     #stand up close to initial pos
     ((89.5, 8.3, -88.9, -9.4),
      (0.0, -0.1, -22.1, 43.5, -21.4, 0.0),
      (0.0, 0.0, -22.3, 43.5, -21.2, 0.2),
      (89.5, -8.2, 81.5, 12.7),
      0.8,0, stiff.NORMAL_STIFFNESSES),

    INITIAL_POS_KEYFRAME,
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
     (0., 17, -40., 100.,-50.,-25.),
     (0., 10,-27,45.,-22.5,-18),
     (100.,-30,0.,0),
     .4,0, stiff.NORMAL_STIFFNESSES),

    # Kick?
    ((43.,30.,0.,0.),
     (0.,17, -60.,70.,-10,-15.),
     (0.,10,-27,45.,-22.5,-18),
     (20.,-30,0, 0),
     .14,0, stiff.NORMAL_STIFFNESSES),

    # Recover
    ((80.,30.,-50.,-70.),
     (0.,25.,-27.,43.5,-21.2,-20.),
     (0.,15,-27,45.,-22.5,-18.),
     (80.,-30.,50.,74.),
     .7,0, stiff.NORMAL_STIFFNESSES),

    ((90.,10.,-90.,-10.),
     (0.0,0.0,-22.3,43.5,-21.2, 0.0),
     (0.0,0.0,-22.3,43.5,-21.2, 0.0),
     (90.,-10.,82.,13.2),
     .7,0,stiff.NORMAL_STIFFNESSES),
    )

RIGHT_STRAIGHT_KICK = mirrorMove(LEFT_STRAIGHT_KICK)

LEFT_QUICK_STRAIGHT_KICK = (
    #stand for a bit
    ((90.,10.,-90.,-10.),
     (0.0,0.0,-22.3,43.5,-21.2, 0.0),
     (0.0,0.0,-22.3,43.5,-21.2, 0.0),
     (90.,-10.,82.,13.2),
     .5,0,stiff.NORMAL_STIFFNESSES),

    #lean right/lift leg
    ((20.,30.,0.,0.),
     (0.,17.,-30,70,-50,-25.),
     (0.,13.,-22.3,45,-22.5,-17),
     (100.,-30.,0.,0),
     .6,0, stiff.NORMAL_STIFFNESSES),

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

    #lean right/lift leg
    ((20.,30.,0.,0.),
     (0.,17.,-30,70,-50,-25.),
     (0.,13.,-22.3,45,-22.5,-17),
     (100.,-30.,0.,0),
     .6,0, stiff.NORMAL_STIFFNESSES),

    #kick?
    ((43.,30.,0.,0.),
     (0.,17.,-65,60,-10,-20.),
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
    ((40, 9, -80, -9),
     (0.0,0.0,-22.3,43.5,-21.2, 0.0),
     (0.0,0.0,-22.3,43.5,-21.2, 0.0),
     (40, -9, 80, -9),
     .7,0,stiff.NORMAL_STIFFNESSES),

    #stand for a bit
    ((90.,10.,-90.,-10.),
     (0.0,0.0,-22.3,43.5,-21.2, 0.0),
     (0.0,0.0,-22.3,43.5,-21.2, 0.0),
     (90.,-10.,82.,13.2),
     1,0,stiff.NORMAL_STIFFNESSES),
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

#**********************#
#                      #
#       Celebrations   #
#                      #
#**********************#

CELEBRATE_AIR_PUMP = (
	#Raise arms
	((-77.4, 4.7, -89.1, -11.0),
	(0.0,0.0,-22.3,43.5,-21.2, 0.0),
	(0.0,0.0,-22.3,43.5,-21.2, 0.0),
	(-67.9, -5.4, 97.3, 20.0),
	2.0,0,stiff.NORMAL_STIFFNESSES),

	INITIAL_POS_KEYFRAME,
	)

CELEBRATE = (
	#Prep for clap
	((19.9, -10.4, -24.1, -36.7),
	(0.0,0.0,-22.3,43.5,-21.2, 0.0),
	(0.0,0.0,-22.3,43.5,-21.2, 0.0),
	(50.5, 7.8, 13.9, 34.6),
	0.8,0,stiff.NORMAL_STIFFNESSES),

	#Clap
	((22.1, -15.0, -14.0, -36.2),
	(0.0,0.0,-22.3,43.5,-21.2, 0.0),
	(0.0,0.0,-22.3,43.5,-21.2, 0.0),
	(48.3, 20.6, 19.0, 34.3),
	0.2,0,stiff.NORMAL_STIFFNESSES),

	#Prep for clap
	((19.9, -10.4, -24.1, -36.7),
	(0.0,0.0,-22.3,43.5,-21.2, 0.0),
	(0.0,0.0,-22.3,43.5,-21.2, 0.0),
	(50.5, 7.8, 13.9, 34.6),
	0.2,0,stiff.NORMAL_STIFFNESSES),

	#Clap
	((22.1, -15.0, -14.0, -36.2),
	(0.0,0.0,-22.3,43.5,-21.2, 0.0),
	(0.0,0.0,-22.3,43.5,-21.2, 0.0),
	(48.3, 20.6, 19.0, 34.3),
	0.2,0,stiff.NORMAL_STIFFNESSES),

	#Prep for clap
	((19.9, -10.4, -24.1, -36.7),
	(0.0,0.0,-22.3,43.5,-21.2, 0.0),
	(0.0,0.0,-22.3,43.5,-21.2, 0.0),
	(50.5, 7.8, 13.9, 34.6),
	0.2,0,stiff.NORMAL_STIFFNESSES),

	#Clap
	((22.1, -15.0, -14.0, -36.2),
	(0.0,0.0,-22.3,43.5,-21.2, 0.0),
	(0.0,0.0,-22.3,43.5,-21.2, 0.0),
	(48.3, 20.6, 19.0, 34.3),
	0.2,0,stiff.NORMAL_STIFFNESSES),

	#Return
	((19.9, -10.4, -24.1, -36.7),
	(0.0,0.0,-22.3,43.5,-21.2, 0.0),
	(0.0,0.0,-22.3,43.5,-21.2, 0.0),
	(50.5, 7.8, 13.9, 34.6),
	0.2,0,stiff.NORMAL_STIFFNESSES),

	INITIAL_POS_KEYFRAME,
	)
