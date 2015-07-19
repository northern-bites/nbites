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
# ORIGINAL DOCUMENTATION (above is mirrored)
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

INITIAL_POS = (((90., 10., -90., -3.),
                (0.0, 0.0, -25., 56.3, -31.3, 0.0),
                (0.0, 0.0, -25., 56.3, -31.3, 0.0),
                (90., -10., 90., 3.),
                3.0,0,stiff.NORMAL_STIFFNESSES),)

#Keyframe that moves can called from an unbalanced position to slowly return the joints to a stable, standing position.
INITIAL_POS_KEYFRAME = ((90., 10., -90., -3.),
                        (0.0, 0.0, -25., 56.3, -31.3, 0.0),
                        (0.0, 0.0, -25., 56.3, -31.3, 0.0),
                        (90., -10., 90., 3.),
			                   2,0,stiff.NORMAL_STIFFNESSES)

#Keyframe that moves the joints to a stable, standing position quickly.
QUICK_INITIAL_POS_KEYFRAME = ((90., 10., -90., -3.),
                              (0.0, 0.0, -25., 56.3, -31.3, 0.0),
                              (0.0, 0.0, -25., 56.3, -31.3, 0.0),
                              (90., -10., 90., 3.),
			                       0.8,0,stiff.NORMAL_STIFFNESSES)

#Angles measured pretty exactly from the robot w/gains off.
#might want to make them even different if we suspect the
#motors are weakening
SIT_POS = (((0.,90.,0.,0.),
            (0.,0.,-55.,125.7,-75.7,0.),
            (0.,0.,-55.,125.7,-75.7,0.),
            (0.,-90.,0.,0.),
            3.0,0,stiff.LOW_HEAD_STIFFNESSES),

           ((90.,10.,-85.,-10.),
            (1.0,0.,-55.,125.7,-75.7,0.),
            (1.0,0.,-55.,125.7,-75.7,0.),
            (90.,-10.,85.,10.),
            1.5,0,stiff.LOW_LEG_STIFFNESSES)
          )

SIT_POS_V5 = (((0.,90.,0.,0.),
               (0.,0.,-55.,125.7,-75.7,0.),
               (0.,0.,-55.,125.7,-75.7,0.),
               (0.,-90.,0.,0.),
               3.0,0,stiff.LOW_HEAD_STIFFNESSES),

              ((90.,10.,-85.,-10.),
               (-1.0,0.,-55.,125.7,-75.7,0.),
               (-1.0,0.,-55.,125.7,-75.7,0.),
               (90.,-10.,85.,10.),
               1.5,0,stiff.LOW_LEG_STIFFNESSES)
             )

#**********************
#                     *
#     STAND  UPS      *
#                     *
#**********************

#v5 robots
STAND_UP_FRONT = (#Initial
                  ((74.9, 6.4, 94.8, -20.7),
                  (3.1, 1.9, 0.4, -4.3, 55.3, -0.8),
                  (0.0, 0.1, 2.5, -5.8, 61.1, 2.2),
                  (74.9, -6.4, -93.2, 19.6),
                  0.3,1, stiff.STANDUP_STIFFNESSES),

                  #Begin wrap around by bending knees and twisting arms out of the way
                  ((97.6, -1.8, 72.5, -49.2),
                  (-0.5, -1.9, 11.3, 20.7, -6.6, 58.0),
                  (-0.5, -6.2, 10.3, 21.8, -7.2, 0.6),
                  (95.8, -1.8, -89.4, 53.3),
                  0.3,1, stiff.STANDUP_STIFFNESSES),

                  #Wrap feet to front be straightening legs
                  ((116.0, -8.9, 82.5, -86.7),
                  (-68.9, 46.1, -75.6, 98.3, 7.5, 58.0),
                  (-68.9, -46.8, -45.1, 69.3, 10.8, -4.5),
                  (115.6, 4.6, -79.5, 86.3),
                  0.4,1, stiff.STANDUP_STIFFNESSES ),

                  #Start to sit up
                  ((111.7, -20.3, 69.5, -89.6),
                  (-69.4, -18.8, -89.1, -7.5, -16.9, 35.7),
                  (0.0, 11.7, -86.3, -6.5, -25.1, -33.7),
                  (106.9, 21.8, -59.2, 89.5),
                  0.5,1, stiff.STANDUP_STIFFNESSES ),

                  #sit up straight
                  ((92.6, -5.4, 70.4, -58.6),
                  (-43.8, -4.7, -89.9, 0.5, -14.6, 14.9),
                  (-43.8, 7.2, -85.6, -6.3, -5.1, -13.2),
                  (92.6, 0.6, -53.6, 54.5),
                  0.5, 1, stiff.STANDUP_STIFFNESSES),

                  #Lean back on arms
                  ((119.6, -5.4, 3.7, -26.9),
                  (-23.1, 13.3, -88.7, 14.5, 32.4, 0.3),
                  (0.0, 1.3, -84.3, 13.4, 48.6, 2.0),
                  (115.8, 3.5, -8.6, 24.0),
                  0.5,1, stiff.STANDUP_STIFFNESSES ),

                  # FINISH WITH BACK STAND UP

                  #take a seat
                  ((120.9, 9.2, 16.6, -31.6),
                  (-30.4, 17.2, -91, 56.5, 53.3, 0.4),
                  (-30.4, -12.4, -87, 57.4, 57.0, 7.6),
                  (121.0, 6.9, -17.8, 2.2),
                  0.4,1, stiff.STANDUP_STIFFNESSES),

                  #take a seat
                  ((120.5, 19.4, 21.7, -39.7),
                  (-34.9, 46.1, -94.2, 105.4, 34.6, 10.7),
                  (-34.9, -34.1, -92.9, 122.5, 18.4, 13.9),
                  (125.4, 3.1, -50, 1.7),
                  0.4,1, stiff.STANDUP_STIFFNESSES),

                 #sitting legs spread hands behind facing forward
                  ((119,-11.08,94.13,-1.93),
                  (-62.93,9.06,-84.64,116.72,21.88,23.73),
                  (-62.93,-29.44,-82.62,103.71,30.85,-10.11),
                  (115.93,17.58,-90.62,3.34),
                  0.3,1, stiff.STANDUP_STIFFNESSES),

                 #makes elbow straight
                  ((40,60,4,-28),
                  (-28,8,-49,126,-25,-22),
                  (-28,-31,-87,80,52,0),
                  (123.1, -28.0, -1.7, 6.9),
                  0.4,1, stiff.STANDUP_STIFFNESSES),
                 
                 # turns to right a little and stands with one arm on ground
                  ((40,50,4,-34),
                  (-31.6, -9.1, -27.5,110,-50,-13),
                  (-32,-31,-95,73,44,0),
                  (109.9, -38.2, -1.6, 1.7),
                  0.5,1, stiff.STANDUP_STIFFNESSES),

                  ((42,28,5,-47),
                  (-49,-16,27,101,-70,-5),
                  (-44.0, -32.9, -91.9, 88.7, 21.7, -3.5),
                  (101,-15,-4,3),
                  0.5,1, stiff.STANDUP_STIFFNESSES),
                 #gets hips up and over knees

                 ((59.76,29.35,4.39,-45.79),
                 (-35.5, -7.0, -32.6, 121.0, -69.3, 3.6),
                 (-35.5, -21.2, -76.0, 67.4, 22.2, 17.7),
                 (54.85,-45.62,-0.27,24.35),
                  0.5,1, stiff.STANDUP_STIFFNESSES),
                 #pulls right leg in a little

                 ((77.5, 28.9, 5.6, -40.7),
                  (-30.1, -2.2, -23, 123.5, -69.4, 3.1),
                  (-30.1, -14.2, -55, 123.2, -42.8, 8.6),
                  (88.1, -22.6, -1.0, 23.0),
                  0.7, 1, stiff.STANDUP_STIFFNESSES),
                 # pulls both legs in, squat

                 #stands up
                 (INITIAL_POS[0][0],
                  INITIAL_POS[0][1],
                  INITIAL_POS[0][2],
                  INITIAL_POS[0][3],
                  1.5,0, stiff.STANDUP_STIFFNESSES)
                 )


#v4 version
STAND_UP_FRONT_V4 = (#Initial
                  ((74.9, 6.4, 94.8, -20.7),
                  (3.1, 1.9, 0.4, -4.3, 55.3, -0.8),
                  (0.0, 0.1, 2.5, -5.8, 61.1, 2.2),
                  (74.9, -6.4, -93.2, 19.6),
                  0.3,1, stiff.STANDUP_STIFFNESSES),

                  #Begin wrap around by bending knees and twisting arms out of the way
                  ((97.6, -1.8, 72.5, -49.2),
                  (-0.5, -1.9, 11.3, 20.7, -6.6, 58.0),
                  (-0.5, -6.2, 10.3, 21.8, -7.2, 0.6),
                  (95.8, -1.8, -89.4, 53.3),
                  0.3,1, stiff.STANDUP_STIFFNESSES),

                  #Wrap feet to front be straightening legs
                  ((116.0, -8.9, 82.5, -86.7),
                  (-68.9, 46.1, -75.6, 98.3, 7.5, 58.0),
                  (-68.9, -46.8, -45.1, 69.3, 10.8, -4.5),
                  (115.6, 4.6, -79.5, 86.3),
                  0.4,1, stiff.STANDUP_STIFFNESSES ),

                  #Start to sit up
                  ((111.7, -20.3, 69.5, -89.6),
                  (-69.4, -18.8, -89.1, -7.5, -16.9, 35.7),
                  (0.0, 11.7, -86.3, -6.5, -25.1, -33.7),
                  (106.9, 21.8, -59.2, 89.5),
                  0.5,1, stiff.STANDUP_STIFFNESSES ),

                  #sit up straight
                  ((92.6, -5.4, 70.4, -58.6),
                  (-43.8, -4.7, -89.9, 0.5, -14.6, 14.9),
                  (-43.8, 7.2, -85.6, -6.3, -5.1, -13.2),
                  (92.6, 0.6, -53.6, 54.5),
                  0.5, 1, stiff.STANDUP_STIFFNESSES),

                  #Lean back on arms
                  ((119.6, -5.4, 3.7, -26.9),
                  (-23.1, 13.3, -88.7, 14.5, 32.4, 0.3),
                  (0.0, 1.3, -84.3, 13.4, 48.6, 2.0),
                  (115.8, 3.5, -8.6, 24.0),
                  0.5,1, stiff.STANDUP_STIFFNESSES ),

                  # FINISH WITH BACK STAND UP

                 ((120.9, 9.2, 16.6, -31.6),
                  (-30.4, 17.2, -91, 56.5, 53.3, 0.4),
                  (-30.4, -12.4, -87, 57.4, 57.0, 7.6),
                  (121.0, 6.9, -17.8, 2.2),
                  0.4,1, stiff.STANDUP_STIFFNESSES),
                 #take a seat

                 ((120.5, 19.4, 21.7, -39.7),
                  (-34.9, 46.1, -94.2, 105.4, 34.6, 10.7),
                  (-34.9, -34.1, -92.9, 122.5, 18.4, 13.9),
                  (125.4, 3.1, -50, 1.7),
                  0.4,1, stiff.STANDUP_STIFFNESSES),
                 #take a seat

                 ((119,-11.08,94.13,-1.93),
                  (-62.93,9.06,-84.64,116.72,21.88,23.73),
                  (-62.93,-29.44,-82.62,103.71,30.85,-10.11),
                  (115.93,17.58,-90.62,3.34),
                  0.3,1, stiff.STANDUP_STIFFNESSES),
                 #sitting legs spread hands behind facing forward

                 # makes elbow straight
                 ((40,60,4,-28),
                  (-28,8,-49,126,-25,-22),
                  (-28,-31,-87,80,52,0),
                  (123.1, -28.0, -1.7, 6.9),
                  0.4,1, stiff.STANDUP_STIFFNESSES),
                 # turns to right a little and stands with one arm on ground

                  ((40,50,4,-34),
                  (-41.3, -7.4, -20.0, 122.1, -67.7, -5.0),
                  (-41.3, -21.4, -90.3, 79.9, 27.6, -0.3),
                  (109.9, -38.2, -1.6, 1.7),
                  0.5,1, stiff.STANDUP_STIFFNESSES),

                  ((42,28,5,-47),
                  (-40.1, -6.9, -17.2, 122.1, -70.0, 1.3),
                  (-40.1, -21.7, -84.5, 95.0, 12.7, 2.7),
                  (101,-15,-4,3),
                  0.5,1, stiff.STANDUP_STIFFNESSES),
                 #gets hips up and over knees

                 ((59.76,29.35,4.39,-45.79),
                  (-34.7, -6.9, -28, 122.0, -70.1, 3.7),
                  (-34.7, -20.1, -78, 108.3, -10.9, 14.0),
                  (54.85,-45.62,-0.27,24.35),
                  0.5,1, stiff.STANDUP_STIFFNESSES),
                 #pulls right leg in a little

                 ((77.5, 28.9, 5.6, -40.7),
                  (-30.1, -2.2, -23, 123.5, -69.4, 3.1),
                  (-30.1, -14.2, -55, 123.2, -42.8, 8.6),
                  (88.1, -22.6, -1.0, 23.0),
                  0.7, 1, stiff.STANDUP_STIFFNESSES),
                 # pulls both legs in, squat

                 #stands up
                 (INITIAL_POS[0][0],
                  INITIAL_POS[0][1],
                  INITIAL_POS[0][2],
                  INITIAL_POS[0][3],
                  1.5,0, stiff.STANDUP_STIFFNESSES)
                 )

#v5 robots
STAND_UP_BACK = (

                 (INITIAL_POS[0][0],
                  INITIAL_POS[0][1],
                  INITIAL_POS[0][2],
                  INITIAL_POS[0][3],
                  0.6,0, stiff.STANDUP_STIFFNESSES),
                 #rotate arms

                 ((100,0,0,0),
                  (0,0,0,0,0,0),
                  (0,0,0,0,0,0),
                  (100,0,0,0),
                  0.4,0, stiff.STANDUP_STIFFNESSES),
                 #arms at sides

                 ((109.9, 13.3, 15.3, -78.5),
                 (-0.4, 22.8, 17.1, 62.1, 32.2, 19.3),
                 (-0.4, -14.2, 20.3, 69.9, 29.1, 1.1),
                  (113.9, -4.3, -12.7, 75.3),
                  0.4,1, stiff.STANDUP_STIFFNESSES),
                 #start to arch hips and move arms under back

                 ((121.0, 20.3, -0.3, -90.1),
                  (-0.4, 12.7, 25.4, 66.3, 29, 10.3),
                  (-0.4, -10.6, 26.4, 72.1, 29, 0.8),
                  (122.6, -17.6, -4.7, 90.2),
                  0.2,1, stiff.STANDUP_STIFFNESSES),
                 #hips arch and arms underneath

                 ((101.9, 19.1, 19.8, -89.8),
                 (-1.6, 10.8, 30.6, 6.7, 51.5, 2.1),
                 (-1.6, -4.6, 30.8, 6.7, 44.2, -0.3),
                  (116.6, -12.7, -2.5, 79.6),
                  0.5,1, stiff.STANDUP_STIFFNESSES),
                 #lift legs for momentum

                 ((102.6, 8.3, 32.0, -89.6),
                  (-5.6, 13.8, -15.3, 23.1, 35, 8.5),
                  (-5.6, -8.6, -15.3, 35.4, 35, -0.3),
                  (117.5, -9.0, -13.8, 84.6),
                  0.3,1, stiff.STANDUP_STIFFNESSES),
                 #lift legs more to sit up

                 ((120.9, 9.2, 16.6, -31.6),
                  (-30.4, 17.2, -91, 56.5, 53.3, 0.4),
                  (-30.4, -12.4, -87, 57.4, 57.0, 7.6),
                  (121.0, 6.9, -17.8, 2.2),
                  0.4,1, stiff.STANDUP_STIFFNESSES),
                 #take a seat

                 ((120.5, 19.4, 21.7, -39.7),
                  (-34.9, 46.1, -94.2, 105.4, 34.6, 10.7),
                  (-34.9, -34.1, -92.9, 122.5, 18.4, 13.9),
                  (125.4, 3.1, -50, 1.7),
                  0.4,1, stiff.STANDUP_STIFFNESSES),
                 #take a seat

                 ((119,-11.08,94.13,-1.93),
                  (-62.93,9.06,-84.64,116.72,21.88,23.73),
                  (-62.93,-29.44,-82.62,103.71,30.85,-10.11),
                  (115.93,17.58,-90.62,3.34),
                  0.3,1, stiff.STANDUP_STIFFNESSES),
                 #sitting legs spread hands behind facing forward

                 # makes elbow straight
                 ((40,60,4,-28),
                  (-28,8,-49,126,-25,-22),
                  (-28,-31,-87,80,52,0),
                  (123.1, -28.0, -1.7, 6.9),
                  0.4,1, stiff.STANDUP_STIFFNESSES),
                 # turns to right a little and stands with one arm on ground

                  ((40,50,4,-34),
                  (-31.6, -9.1, -27.5,110,-50,-13),
                  (-32,-31,-95,73,44,0),
                  (109.9, -38.2, -1.6, 1.7),
                  0.5,1, stiff.STANDUP_STIFFNESSES),

                  ((42,28,5,-47),
                  (-49,-16,27,101,-70,-5),
                  (-44.0, -32.9, -91.9, 88.7, 21.7, -3.5),
                  (101,-15,-4,3),
                  0.5,1, stiff.STANDUP_STIFFNESSES),
                 #gets hips up and over knees

                 ((59.76,29.35,4.39,-45.79),
                 (-35.5, -7.0, -32.6, 121.0, -69.3, 3.6),
                 (-35.5, -21.2, -76.0, 67.4, 22.2, 17.7),
                  (54.85,-45.62,-0.27,24.35),
                  0.5,1, stiff.STANDUP_STIFFNESSES),
                 #pulls right leg in a little

                 ((77.5, 28.9, 5.6, -40.7),
                  (-30.1, -2.2, -23, 123.5, -69.4, 3.1),
                  (-30.1, -14.2, -55, 123.2, -42.8, 8.6),
                  (88.1, -22.6, -1.0, 23.0),
                  0.7, 1, stiff.STANDUP_STIFFNESSES),
                 # pulls both legs in, squat

                 #stands up
                 (INITIAL_POS[0][0],
                  INITIAL_POS[0][1],
                  INITIAL_POS[0][2],
                  INITIAL_POS[0][3],
                  1.5,0, stiff.STANDUP_STIFFNESSES)
                 )
                 
#v4 version
STAND_UP_BACK_V4 = (

                 (INITIAL_POS[0][0],
                  INITIAL_POS[0][1],
                  INITIAL_POS[0][2],
                  INITIAL_POS[0][3],
                  0.6,0, stiff.STANDUP_STIFFNESSES),
                 #rotate arms

                 ((100,0,0,0),
                  (0,0,0,0,0,0),
                  (0,0,0,0,0,0),
                  (100,0,0,0),
                  0.4,0, stiff.STANDUP_STIFFNESSES),
                 #arms at sides

                 ((109.9, 13.3, 15.3, -78.5),
                 (-0.4, 22.8, 17.1, 62.1, 32.2, 19.3),
                 (-0.4, -14.2, 20.3, 69.9, 29.1, 1.1),
                  (113.9, -4.3, -12.7, 75.3),
                  0.4,1, stiff.STANDUP_STIFFNESSES),
                 #start to arch hips and move arms under back

                 ((121.0, 20.3, -0.3, -90.1),
                  (-0.4, 12.7, 25.4, 66.3, 29, 10.3),
                  (-0.4, -10.6, 26.4, 72.1, 29, 0.8),
                  (122.6, -17.6, -4.7, 90.2),
                  0.2,1, stiff.STANDUP_STIFFNESSES),
                 #hips arch and arms underneath

                 ((101.9, 19.1, 19.8, -89.8),
                 (-1.6, 10.8, 19, 6.7, 51.5, 2.1),
                 (-1.6, -4.6, 19, 6.7, 44.2, -0.3),
                  (116.6, -12.7, -2.5, 79.6),
                  0.5,1, stiff.STANDUP_STIFFNESSES),
                 #lift legs for momentum

                 ((102.6, 8.3, 32.0, -89.6),
                  (-5.6, 13.8, -15.3, 23.1, 40, 8.5),
                  (-5.6, -8.6, -15.3, 35.4, 40, -0.3),
                  (117.5, -9.0, -13.8, 84.6),
                  0.5,1, stiff.STANDUP_STIFFNESSES),
                 #lift legs more to sit up

                 ((120.9, 9.2, 16.6, -31.6),
                  (-30.4, 17.2, -91, 56.5, 53.3, 0.4),
                  (-30.4, -12.4, -87, 57.4, 57.0, 7.6),
                  (121.0, 6.9, -17.8, 2.2),
                  0.4,1, stiff.STANDUP_STIFFNESSES),
                 #take a seat

                 ((120.5, 19.4, 21.7, -39.7),
                  (-34.9, 46.1, -94.2, 105.4, 34.6, 10.7),
                  (-34.9, -34.1, -92.9, 122.5, 18.4, 13.9),
                  (125.4, 3.1, -50, 1.7),
                  0.4,1, stiff.STANDUP_STIFFNESSES),
                 #take a seat

                 ((119,-11.08,94.13,-1.93),
                  (-62.93,9.06,-84.64,116.72,21.88,23.73),
                  (-62.93,-29.44,-82.62,103.71,30.85,-10.11),
                  (115.93,17.58,-90.62,3.34),
                  0.3,1, stiff.STANDUP_STIFFNESSES),
                 #sitting legs spread hands behind facing forward

                 # makes elbow straight
                 ((40,60,4,-28),
                  (-28,8,-49,126,-25,-22),
                  (-28,-31,-87,80,52,0),
                  (123.1, -28.0, -1.7, 6.9),
                  0.4,1, stiff.STANDUP_STIFFNESSES),
                 # turns to right a little and stands with one arm on ground

                  ((40,50,4,-34),
                  (-41.3, -7.4, -20.0, 122.1, -67.7, -5.0),
                  (-41.3, -21.4, -90.3, 79.9, 27.6, -0.3),
                  (109.9, -38.2, -1.6, 1.7),
                  0.5,1, stiff.STANDUP_STIFFNESSES),

                  ((42,28,5,-47),
                  (-40.1, -6.9, -17.2, 122.1, -70.0, 1.3),
                  (-40.1, -21.7, -84.5, 95.0, 12.7, 2.7),
                  (101,-15,-4,3),
                  0.5,1, stiff.STANDUP_STIFFNESSES),
                 #gets hips up and over knees

                 ((59.76,29.35,4.39,-45.79),
                  (-34.7, -6.9, -28, 122.0, -70.1, 3.7),
                  (-34.7, -20.1, -78, 108.3, -10.9, 14.0),
                  (54.85,-45.62,-0.27,24.35),
                  0.5,1, stiff.STANDUP_STIFFNESSES),
                 #pulls right leg in a little

                 ((77.5, 28.9, 5.6, -40.7),
                  (-30.1, -2.2, -23, 123.5, -69.4, 3.1),
                  (-30.1, -14.2, -55, 123.2, -42.8, 8.6),
                  (88.1, -22.6, -1.0, 23.0),
                  0.7, 1, stiff.STANDUP_STIFFNESSES),
                 # pulls both legs in, squat

                 #stands up
                 (INITIAL_POS[0][0],
                  INITIAL_POS[0][1],
                  INITIAL_POS[0][2],
                  INITIAL_POS[0][3],
                  1.5,0, stiff.STANDUP_STIFFNESSES)
                 )

REVERSE_STAND_UP_BACK = mirrorMove(STAND_UP_BACK)

#**********************
#                     *
#       GOALIE        *
#                     *
#**********************

GOALIE_SQUAT = (# semi squat
                 ((19.4, 22.8, 35.9, -10.9),
                  (-45.4, 17.8, -36.6, 75.2, -9.8, -9.5),
                  (-45.4, -20.7, -44.7, 78.4, -8.8, 11.6),
                  (15.6, -29.7, -30.8, 2.0),
                  0.2, 1, stiff.NORMAL_STIFFNESSES),

                # get low
                 ((36.1, -19.6, 81.1, -32.5),
                 (-67.5, 41.1, -87.9, 88.0, 40.7, -16.9),
                 (-67.5, -46.1, -87.7, 86.8, 41.0, 20.7),
                 (32.8, 17.9, -87.5, 46.5),
                 0.2,1, stiff.NORMAL_STIFFNESSES),

                 # semi squat
                 ((35.7, -7.6, 59.6, -30.8),
                 (-64.0, 14.2, -90.6, 52.6, 38.1, -5.0),
                 (-64.0, -19.0, -88.8, 46.8, 46.5, 9.3),
                 (41.2, 17.8, -65.4, 29.3),
                 0.3,1, stiff.NORMAL_STIFFNESSES),

                 # hands out
                 ((32.5, -21.6, 78.3, -36.9),
                 (-62.8, 22.1, -88.5, 60.0, 36.2, -8.8),
                 (-62.8, -27.2, -87.5, 57.5, 31.7, 11.6),
                 (37.4, 21.2, -87.7, 33.3),
                  0.3,1, stiff.NORMAL_STIFFNESSES),
                 )

#Stand up for GOALIE_SQUAT
GOALIE_SQUAT_STAND_UP = (((-28.9, 17.5, 80.5, -1.7),
                           (-52.0, 18.8, -84.5, 29.0, 45.5, -5.5),
                           (-52.0, -9.0, -80.7, 24.4, 62.1, 5.1),
                           (-24.3, -6.0, -104.7, 3.7),
                           0.3, 1, stiff.NORMAL_STIFFNESSES),

                          ((-18.2, 66.4, 97.4, -8.5),
                           (-52.6, 28.2, -76.5, 30.1, 41.2, -6.6),
                           (-52.6, -19.2, -81.5, 31.6, 32.5, 4.4),
                           (-3.7, -59.4, -85.5, 4.6),
                           0.5, 1, stiff.NORMAL_STIFFNESSES),

                          # set arms up
                          ((107.9, 74.4, -0.8, -2.5),
                           (-27.2, 44.5, -93.7, 72.0, 53.4, -8.3),
                           (-27.2, -40.3, -92.6, 58.5, 63.7, 3.5),
                           (102.5, -74.6, -11.1, 4.7),
                           0.5, 1, stiff.NORMAL_STIFFNESSES),

                          # get arms down
                          ((114.5, -11.3, -1.8, -14.0),
                           (-47.5, 7.6, -84.5, 20.1, 63.2, -3.0),
                           (-47.5, -3.6, -86.6, 27.9, 63.8, 3.9),
                           (116.3, 12.8, -9.8, 12.1),
                           0.4, 1, stiff.NORMAL_STIFFNESSES),
                          # start to get up
                          ((39.9, 51.8, -18.5, -2.2),
                          (-52.6, 19.7, -88.9, 122.6, 18.1, -5.4),
                          (-52.6, -29.7, -89.4, 61.3, 59.7, 0.7),
                          (122.0, 17.8, 62.0, 4.1),
                          0.4, 1, stiff.NORMAL_STIFFNESSES),

                        # turns to right a little and stands with one arm on ground
                          ((9.8, 6.2, 17.7, -6.5),
                           (-66.8, -12.2, -11.1, 122.9, -53.2, -0.2),
                           (-66.8, -36.9, -73.0, 24.4, 62.7, 4.0),
                           (105.8, -33.7, 30.2, 4.0),
                           .5,1, stiff.STANDUP_STIFFNESSES),
                        # problem?
                         ((42,28,5,-47),
                         (-40.1, -6.9, -17.2, 122.1, -70.0, 1.3),
                         (-40.1, -21.7, -84.5, 95.0, 12.7, 2.7),
                         (101,-15,-4,3),
                         0.5,1, stiff.STANDUP_STIFFNESSES),
                        #gets hips up and over knees

                        ((59.76,29.35,4.39,-45.79),
                         (-34.7, -6.9, -28, 122.0, -70.1, 3.7),
                         (-34.7, -20.1, -78, 108.3, -10.9, 14.0),
                         (54.85,-45.62,-0.27,24.35),
                         0.5,1, stiff.STANDUP_STIFFNESSES),
                        #pulls right leg in a little

                        ((77.5, 28.9, 5.6, -40.7),
                         (-30.1, -2.2, -23, 123.5, -69.4, 3.1),
                         (-30.1, -14.2, -55, 123.2, -42.8, 8.6),
                         (88.1, -22.6, -1.0, 23.0),
                         0.7, 1, stiff.STANDUP_STIFFNESSES),
                        # pulls both legs in, squat

                        #stands up
                        (INITIAL_POS[0][0],
                         INITIAL_POS[0][1],
                         INITIAL_POS[0][2],
                         INITIAL_POS[0][3],
                         1.5,0, stiff.STANDUP_STIFFNESSES)
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
        )

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
     .5 , 0, stiff.NORMAL_STIFFNESSES),

    #Swing around Leg
    ((90.,30,-75.,-86.),
     (0.,45.,-95.,25.,-10.,-10.),
     (0.,12,-27.,45.,-22.5,-17),
     (80.,-40.,50.,70.),
     .7 , 0, stiff.NORMAL_STIFFNESSES),

    #  #Set For Kick
    ((90.,30,-75.,-86.),
     (0.,17.,-95.,20.,-68.,-10.),
     (0.,13.,-27.,45.,-22.5,-17),
     (90.,-40.,50.,70.),
     .4, 0, stiff.NORMAL_STIFFNESSES),

     # KICK!!!
    ((90.,22,-75.,-86.),
     (0.,17.,-50.,65.,-16.,-10.),
     (0.,13.,-27.,45,-22.5,-17),
     (90.,-40.,50.,70.),
     .4 , 0, stiff.NORMAL_STIFFNESSES),

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

    # lessen right roll
    ((50.,40.,0.,0.),
     (0.,15.,-80.,60.,-5.,-20.),
     (0.,1.,-22.,30.,-8.,-18.),
     (20.,-40.,0.,0.),
     0.2,0, stiff.NORMAL_STIFFNESSES),

     # lean back more
     ((65.2, 8.9, -0.2, -1.5),
      (-4.5, 4.6, -36.2, 34.0, -7.8, -6.1),
      (0.0, 1., -35.3, 37.6, -12.9, -6.0),
      (62.4, -6.6, 0.2, 2.5),
      0.6,0, stiff.NORMAL_STIFFNESSES),

     #stand up close to initial pos
     ((89.5, 8.3, -88.9, -9.4),
      (0.0, 0., -22.1, 43.5, -21.4, 0.0),
      (0.0, 0., -22.3, 43.5, -21.2, 0.2),
      (89.5, -8.2, 81.5, 12.7),
      0.9,0, stiff.NORMAL_STIFFNESSES),

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
    # ((80.,30.,-50.,-70.),
    ((90.,10.,-90.,-10.),
     (0.,25.,-27.,43.5,-21.2,-20.),
     (0.,15,-27,45.,-22.5,-18.),
     (80.,-30.,50.,74.),
     .7,0, stiff.NORMAL_STIFFNESSES),

    ((90.,10.,-90.,-10.),
     (0.0,0.0,-22.3,43.5,-21.2, 0.0),
     (0.0,0.0,-22.3,43.5,-21.2, 0.0),
     (90.,-10.,82.,13.2),
     .7,0,stiff.NORMAL_STIFFNESSES)
    )

RIGHT_STRAIGHT_KICK = mirrorMove(LEFT_STRAIGHT_KICK)

# This is a relatively stable straight kick for arms back
LEFT_SHORT_STRAIGHT_KICK = (
    #stand for a bit
    ((90.,10.,-90.,-10.),
     (-0.2,5,-25,43.5,-21.2, 0.0),
     (-0.2,0.0,-22.3,43.5,-21.2, 0.0),
     (90.,-10.,82.,13.2),
     0.3,0,stiff.NORMAL_STIFFNESSES),

    #lean right/lift leg
    ((20.,30.,0.,0.),
     (0.,20.,-30,70,-40,-25.),
     (0.,20.,-22.3,50,-22.5,-17),
     (100.,-30.,0.,0),
     0.6,0, stiff.NORMAL_STIFFNESSES),

    #kick
    ((43.,30.,0.,0.),
     (0.,19.,-60,50,3,-19.),
     (0.,17,-22.3,43,-23,-17),
     (40.,-30.,0.,0),
     0.15,0, stiff.NORMAL_STIFFNESSES),

    #recover
    ((35.,30.,0.,0.),
     (0.,25.,-35,65,-30,-25.),
     (0.,10.,-22.3,45,-22.5,-17),
     (90.,-30.,0.,0),
     0.6,0, stiff.NORMAL_STIFFNESSES),

    #back to normal
    ((40, 9, -80, -9),
     (0.0,0.0,-22.3,43.5,-21.2, 0.0),
     (0.0,0.0,-22.3,43.5,-21.2, 0.0),
     (40, -9, 80, -9),
     .7,0,stiff.NORMAL_STIFFNESSES),

    #stand for a bit
    ((90., 10., -90., -3.),
      (0.0, 0.0, -25., 56.3, -31.3, 0.0),
      (0.0, 0.0, -25., 56.3, -31.3, 0.0),
      (90., -10., 90., 3.),
      1,0,stiff.NORMAL_STIFFNESSES),
    )

RIGHT_SHORT_STRAIGHT_KICK = mirrorMove(LEFT_SHORT_STRAIGHT_KICK)

# Stands for the Dans; Zeller and Navarro
ZELLVARRO_LEFT_KICK = (
   # stand for a bit
    ((90.,10.,-90.,-10.),
     (0.0,0.0,-22.3,43.5,-21.2, 0.0),
     (0.0,0.0,-22.3,43.5,-21.2, 0.0),
     (90.,-10.,82.,13.2),
     .5,0,stiff.NORMAL_STIFFNESSES),

    # lean weight onto the right foot
    ((75.,15.,-90.,-10.),
     (-1.1, 13.1, -23.6, 50.1, -28.3, -13.7),
     (0.0, 16.4, -23.3, 47.1, -27.2, -17.2),
     (100.,-30.,90.,10),
     .9,0, stiff.NORMAL_STIFFNESSES),

    # lift leg and bring it back a little
    ((50.,15.,-90.,-10.),
     (-0.4, 20, -12.0, 70, -52.6, -9.8),
     (0.,14,-16,45,-25,-20),
     (97.,-37.,90.,10),
     .45,0, stiff.NORMAL_STIFFNESSES),

    # kick it!!
    ((86.7, 37.3, -90., -10),
     (-3.9, 20, -45, 70, -25, -16.8),
     (0.0, 13.8, -10, 45, -27.6, -20),
     (91.7, -15, 90., 10),
     .2,0, stiff.NORMAL_STIFFNESSES),

    # recover #2
    ((86.8, 34.9, -89.8, -9.7),
     (0.0, 18.5, -27.9, 48.3, -15.6, -19.3),
     (0.0, 13.3, -18.2, 44.8, -25.1, -19.6),
     (89.5, -13.9, 89.7, 9.6),
     .6,0, stiff.NORMAL_STIFFNESSES),

    # stand for a bit
    ((90.,10.,-90.,-10.),
     (0.0,0.0,-22.3,43.5,-21.2, 0.0),
     (0.0,0.0,-22.3,43.5,-21.2, 0.0),
     (90.,-10.,82.,13.2),
     1,0,stiff.NORMAL_STIFFNESSES),
)

ZELLVARRO_RIGHT_KICK = mirrorMove(ZELLVARRO_LEFT_KICK)

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
