
#constants file to store all our sweet ass-moves for the Nao
#import MotionConstants

import StiffnessModes as stiff

SWEET_MOVE_LENGTH = 7
CHAIN_MOVE_LENGTH = 5


#============================================================================
#   CODE FROM BURST-ISRAEL ROBOCUP 2009
#
#
#    Usage:
#    ------
#    KICK_RIGHT = mirrorMove(KICK_LEFT)
#    GOALIE_DIVE_LEFT = mirrorMove(GOALIE_DIVE_RIGHT)
#    GOALIE_ROLL_OUT_LEFT = mirrorMove(GOALIE_ROLL_OUT_RIGHT)
#    RIGHT_LONG_BACK_KICK =  mirrorMove(LEFT_LONG_BACK_KICK)
#    mirrorMove(LEFT_D_KICK(-1*y,dist))
#    SHORT_QUICK_RIGHT_KICK = mirrorMove(SHORT_QUICK_LEFT_KICK)
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

INITIAL_POS = (((60.0, 35.0, 0.0, 0.0),
                (0.0,  0.0,  -21.6, 52.13, -30.3, 0.0),
                (0.0,  0.0,  -21.6, 52.13, -30.3, 0.0),
                (60.0,-35.0, 0.0, 0.0),
                3.0,0,stiff.LOOSE_ARMS_STIFFNESSES),)

NO_HEAD_INITIAL_POS = (((60.0, 35.0, 0.0, 0.0),
                        (0.0,  0.0,  -21.6, 52.13, -30.3, 0.0),
                        (0.0,  0.0,  -21.6, 52.13, -30.3, 0.0),
                        (60.0,-35.0, 0.0, 0.0),
                        3.0,0,stiff.NO_HEAD_STIFFNESSES),)

#Angles measured pretty exactly from the robot w/gains off.
#might want to make them even different if we suspect the motors are weakening
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

ZERO_POS = (((0.,0.,0.,0.),
             (0.,0.,0.,0.,0.),
             (0.,0.,0.,0.,0.,0.),
             (0.,0.,0.,0.),
             4.0,0,stiff.LOOSE_ARMS_STIFFNESSES),)

PENALIZED_POS = INITIAL_POS

SET_POS = INITIAL_POS

READY_POS = INITIAL_POS

#same as INITIAL_POS, except for time ot get to pos
GOALIE_POS = ((INITIAL_POS[0][0],
               INITIAL_POS[0][1],
               INITIAL_POS[0][2],
               INITIAL_POS[0][3],
               1.0,0,stiff.LOOSE_ARMS_STIFFNESSES),)

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
                    0.9,1, stiff.STANDUP_STIFFNESSES),
                   #Push back onto feet

                   ((35,2,-14,-41),
                    (-50,5,-90,123,-13,-5),
                    (-50,-5,-90,123,-13,5),
                    (35,2,14,41),
                    1.0, 1, stiff.STANDUP_STIFFNESSES),
                   #Rock back to squatting upright

                   ((75,10,-53,-74),
                    (-40,6,-61,124,-35,-6),
                    (-40,-6,-61,124,-35,6),
                    (75,-10,53,74),
                    0.7, 1, stiff.STANDUP_STIFFNESSES),

                   ((93,10,-90,-80),
                    (0,0,-60,120,-60,0),
                    (0,0,-60,120,-60,0),
                    (93,-10,90,80),
                    0.6,1, stiff.STANDUP_STIFFNESSES),
                   #Pull knees together

                   #Stands up:
                   ( INITIAL_POS[0][0],
                     INITIAL_POS[0][1],
                     INITIAL_POS[0][2],
                     INITIAL_POS[0][3],
                     1.0,1, stiff.STANDUP_STIFFNESSES))


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
                   #Rock back to squatting upright

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

SAVE_LEFT_DEBUG = ( ((-90,0,0,0),
                     (GOALIE_POS[0][1]),
                     (GOALIE_POS[0][2]),
                     (GOALIE_POS[0][3]), 
                     .5, 0, stiff.LOOSE_ARMS_STIFFNESSES), )

SAVE_RIGHT_DEBUG = ( ((GOALIE_POS[0][0]),
                      (GOALIE_POS[0][1]),
                      (GOALIE_POS[0][2]),
                      (-90,0,0,0), 
                      .5, 0, stiff.LOOSE_ARMS_STIFFNESSES), )

SAVE_CENTER_DEBUG = ( ((-90,0,0,0),
                       (GOALIE_POS[0][1]),
                       (GOALIE_POS[0][2]),
                       (-90,0,0,0), 
                       .5,0, stiff.NORMAL_STIFFNESSES), )

SAVE_LEFT_HOLD_DEBUG = ( ((-90,0,0,0),
                          (GOALIE_POS[0][1]),
                          (GOALIE_POS[0][2]),
                          (GOALIE_POS[0][3]), 
                          0.1, 0, stiff.LOOSE_ARMS_STIFFNESSES), )

SAVE_RIGHT_HOLD_DEBUG = ( ((GOALIE_POS[0][0]),
                           (GOALIE_POS[0][1]),
                           (GOALIE_POS[0][2]),
                           (-90,0,0,0), 
                           0.1, 0, stiff.LOOSE_ARMS_STIFFNESSES), )

SAVE_CENTER_HOLD_DEBUG = ( ((-90,0,0,0),
                            (GOALIE_POS[0][1]),
                            (GOALIE_POS[0][2]),
                            (-90,0,0,0), 
                            0.1, 0, stiff.LOOSE_ARMS_STIFFNESSES), )

GOALIE_SAVE = (
    #shoulder pitch, shoulder roll, elbow yaw, elbow roll
    #hip yaw pitch, hip pitch, hip roll, knee pitch, ankle pitch, ankle roll
    ((60.0, 35.0, 0.0, -0), #right arm
     ( -76.0, 10.0, -50.0, 86.0, 0.0, -0),
     ( -76.0, -10.0, -50.0, 86.0, 0.0, 0),
     (60.0, -35.0, 0.0, 0), #left arm
     1.5, 0, stiff.LOOSE_ARMS_STIFFNESSES),

    ((40.0, 15.0, 0.0, -0), #right arm
     ( -76.0, 20.0, -95.0, 65.0, 65.0, 0), #left leg
     ( -76.0, -20.0, -95.0, 65.0, 65.0, 0), #right leg
     (40.0, -15.0, 0.0, 0), #left arm
     .5, 0, stiff.LOOSE_ARMS_STIFFNESSES) )

#Squats with legs in M shape and arms at a 45 degree angle
GOALIE_SQUAT = ( ((60.0, 55.0, 0.0, -0),
                  ( -76.0, 14.0, -54.0, 86.0, 0.0, -11),
                  ( -76.0, -14.0, -54.0, 86.0, 0.0, 11),
                  (60.0, -55.0, 0.0, 0),
                  0.3, 1, stiff.NORMAL_STIFFNESSES),

                 ((60., 35., 0., 0.),
                  (-90., 38., -75., 90, 30, -20.),
                  (-90., -38., -75., 90, 30, 20.),
                  (60., -35., 0., 0.),
                  0.3, 0, stiff.LOW_HEAD_STIFFNESSES),
                 )

GOALIE_PRE_SAVE_POS = ( ((45.88,38.93,-122.88,-32.43),
                        (-59.06,21.54,-17.49,122.69,-69.70,-5.01),
                        (-59.06,-37.44,-16.00,123.84,-69.17,5.54),
                        (41.31,-38.85,120.23,41.93),
                        1.0, 0, stiff.LOW_HEAD_STIFFNESSES),)
                      #squats down with legs spread
                      #arms bent with hands above elbow

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

#Points left when would dive left
GOALIE_TEST_DIVE_LEFT = ( ((0, 0, 0, 0),
                            INITIAL_POS[0][1],
                            INITIAL_POS[0][2],
                            (0, -70.0, 0, 0),
                            1.0,0,stiff.LOOSE_ARMS_STIFFNESSES),)

#Points right when would dive right
GOALIE_TEST_DIVE_RIGHT = ( ((0, 70.0, 0, 0),
                            INITIAL_POS[0][1],
                            INITIAL_POS[0][2],
                            (0, 0, 0, 0),
                            1.0,0,stiff.LOOSE_ARMS_STIFFNESSES),)

#Both arms point forward when would save center
GOALIE_TEST_CENTER_SAVE = ( ((0, 0, 0, 0),
                             INITIAL_POS[0][1],
                             INITIAL_POS[0][2],
                             (0, 0, 0, 0),
                             1.0,0,stiff.LOOSE_ARMS_STIFFNESSES),)

#Stand up for GOALIE_SQUAT
GOALIE_SQUAT_STAND_UP = ( ((0.0, 55.0, 0.0, -0),
                           ( -76.0, 10.0, -75.0, 125.0,  -10.7, -0),
                           ( -76.0, -10.0, -75.0, 125.0, -10.7, 0),
                           (0.0, -55.0, 0.0, 0),
                           .5, 0, stiff.STANDUP_STIFFNESSES) ,

                          ((0.0, 45.0, 0.0, -0),
                           ( -46.0, 4.0, -50.0, 125.0,  -45., -5),
                           ( -46.0, -4.0, -50.0, 125.0, -45., 5),
                           (0.0, -45.0, 0.0, 0),
                           .5, 0, stiff.STANDUP_STIFFNESSES) ,

                          (INITIAL_POS[0][0],
                           (0.0,  5.0, -28., 52.13, -30.3, 0.0),
                           (0.0,  -5.0, -28., 52.13, -30.3, 0.0),
                           INITIAL_POS[0][3],
                           .75,0,stiff.STANDUP_STIFFNESSES),
                          )

GOALIE_SQUAT_STRAFE_RIGHT = ( ((-90., 90., 0., 0.),
                               (-90., 50., -75., 125.7, -0., -20.),
                               (-90., -50., -75., 90, 30, 20.),
                               (-90., -90., 0., 0.),
                               1.5, 0, stiff.LOW_HEAD_STIFFNESSES),

                              ((-90., 90., 0., 0.),
                               (-90., 50., -75., 125.7, -10., -20.),
                               (-90., -50., -75., 40, 40, 20.),
                               (-90., -90., 0., 0.),
                               1.5, 0, stiff.LOW_HEAD_STIFFNESSES))

GOALIE_STAND_UP = (
    ((35,2,-14,-41),
     (-55,5,-90,123,-17,-17),
     (-55,-5,-90,123,-17,17),
     (35,2,14,41),
     0.8, 0, stiff.STANDUP_STIFFNESSES),

    ((75,10,-53,-74),
     (-40,6,-61,124,-35,-6),
     (-40,-6,-61,124,-35,6),
     (75,-10,53,74),
     1.2, 0, stiff.STANDUP_STIFFNESSES),

    ((93,10,-90,-80),
     (0,0,-60,120,-60,0),
     (0,0,-60,120,-60,0),
     (93,-10,90,80),
     1.0,0, stiff.STANDUP_STIFFNESSES),

    ( INITIAL_POS[0][0],
      INITIAL_POS[0][1],
      INITIAL_POS[0][2],
      INITIAL_POS[0][3],
      1.0,0, stiff.STANDUP_STIFFNESSES))


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
    kick_leg_hip_roll=y*2.0;

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
         (0.,0.,-35.,73.,-40.,-13.),
         (0.,0,-38.,76.,-40.,-13.),
         (80.,-40.,50.,70.),0.5,0, stiff.NORMAL_STIFFNESSES),

        # Lift/cock leg
        ((80.,0.,-50.,-90.),
         (0.,kick_leg_hip_roll-10., -20.,120.,-68.,-20.),
         (0.,0,-40.,86.,-45.,-20.),
         (80.,support_arm_shoulder_roll,50.,70.),
         .5,0, stiff.NORMAL_STIFFNESSES),

        # kick left leg
        ((80.,0.,-50.,-90.),
         (0.,kick_leg_hip_roll-5., kick_hip_pitch-5, 60.,10.,-20.),
         (0.,0.,-40.,86.,-45.,-20.),
         (80.,support_arm_shoulder_roll,50.,70.),
         .20,0, stiff.NORMAL_STIFFNESSES),

        # return to normal position
        ((60., 35., 0.,0.),
         (0.0,  0.0,  -22., 50., -30., 0.0),
         (0.0,  0.0,  -21., 52., -30., 0.0),
         (60., -35, 0., 0.),
         .5,0, stiff.LOW_HEAD_STIFFNESSES)
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
     (0.,15.,-45.,85.,-40.,-20.),
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
     (0.,15.,-45.,85.,-40.,-10.),
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
     (0.0 , -15.0 , -45.0 , 85.0 , -40.0 , 20.0),
     (80.0 , -40.0 , 50.0 , 70.0),
     0.8 , 0, stiff.RIGHT_FAR_KICK_STIFFNESSES),

    ((80.0 , 40.0 , -50.0 , -70.0),
     (0.0 , -15.0 , -10.0 , 30.0 , -10.0 , 20.0),
     (0.0 , -15.0 , -60.0 , 41.0 , -8.0 , 20.0),
     (80.0 , -40.0 , 50.0 , 70.0),
     0.11 , 0, stiff.RIGHT_FAR_KICK_STIFFNESSES),

    ((80.0 , 40.0 , -50.0 , -70.0),
     (0.0 , -15.0 , -12.0 , 16.0 , -10.0 , 20.0),
     (0.0 , -15.0 , -45.0 , 85.0 , -40.0 , 10.0),
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

DREW_LEFT_SIDE_KICK = (
    # lean right
    ((80.,40.,-50.,-70.),
     (0.,0.,-35.,65.,-28.,-14.),
     (0.,2,-38.,76.,-40.,-14.),
     (80.,-40.,50.,70.),
     0.8,0, stiff.NORMAL_STIFFNESSES),

    #lift left leg to side
    ((80.,0.,-50.,-70.),
     (0.,45.,-35.,65.,-10.,-10.),
     (0.,5.,-38.,76.,-40.,-22.),
     (80.,-40.,50.,70.),
     0.5 , 0, stiff.LEFT_SIDE_KICK_STIFFNESSES),

    # move leg forward
    ((80.,0.,-50.,-70.),
     (0.,45.,-65.,55.,-20.,0.),
     (0.,10.,-38.,76.,-40.,-22.),
     (90.,-90.,50.,70.),
     .4 , 0, stiff.LEFT_SIDE_KICK_STIFFNESSES),

    # kick across body
    ((80.,0.,-50.,-70.),
     (0.,-22.,-70.,25.,23.,0.),
     (0.,15.,-38.,76.,-40.,-22.),
     (90.,-90.,50.,70.),
     .2, 0, stiff.LEFT_SIDE_KICK_STIFFNESSES),

    # recover
    ((80.,40.,-50.,-70.),
     (0.,0.,-35.,65.,-28.,-10.),
     (0.,2,-38.,76.,-40.,-14.),
     (80.,-40.,50.,70.),
     0.4,0, stiff.NORMAL_STIFFNESSES))

DREW_RIGHT_SIDE_KICK = (
    ((80.0, 40.0, -50.0, -70.0),
     (0.0, -2, -38.0, 76.0, -40.0, 14.0),
     (0.0, -0.0, -35.0, 65.0, -28.0, 14.0),
     (80.0, -40.0, 50.0, 70.0),
     0.8, 0, stiff.RIGHT_SIDE_KICK_STIFFNESSES),

    ((80.0, 40.0, -50.0, -70.0),
     (0.0, -5.0, -38.0, 76.0, -40.0, 22.0),
     (0.0, -45.0, -35.0, 65.0, -10.0, 10.0),
     (80.0, -0.0, 50.0, 70.0),
     0.5, 0, stiff.RIGHT_SIDE_KICK_STIFFNESSES),

    ((90.0, 90.0, -50.0, -70.0),
     (0.0, -10.0, -38.0, 76.0, -40.0, 22.0),
     (0.0, -45.0, -55.0, 55.0, -20.0, -0.0),
     (80.0, -0.0, 50.0, 70.0),
     0.4, 0, stiff.RIGHT_SIDE_KICK_STIFFNESSES),

    ((90.0, 90.0, -50.0, -70.0),
     (0.0, -15.0, -38.0, 76.0, -40.0, 22.0),
     (0.0, 22.0, -70.0, 25.0, 23.0, -0.0),
     (80.0, -0.0, 50.0, 70.0),
     0.2, 0, stiff.RIGHT_SIDE_KICK_STIFFNESSES),

    ((80.0, 40.0, -50.0, -70.0),
     (0.0, -2, -38.0, 76.0, -40.0, 14.0),
     (0.0, -0.0, -35.0, 65.0, -28.0, 10.0),
     (80.0, -40.0, 50.0, 70.0),
     0.4, 0, stiff.NORMAL_STIFFNESSES))

LEFT_SIDE_KICK = (
    # lean right
    ((80.,40.,-50.,-70.),
     (0.,20.,-10.,20.,-10.,-20.),
     (0.,16.,-13.,20.,-10.,-22.),
     (80.,-40.,50.,70.),
     2.0,0, stiff.LEFT_SIDE_KICK_STIFFNESSES),

    (2, (0.,40.,-30.,70.,-32.,-20.),0.4 , 0, stiff.LEFT_SIDE_KICK_STIFFNESSES),

    (2, (0.,40.,-50.,30.,20.,0.),0.4 , 0, stiff.LEFT_SIDE_KICK_STIFFNESSES),

    ((80.,70.,-10.,-10.),
     (0.,9.,-60.,30.,23.,0.),
     (0.,23.,-13.,20.,-10.,-25.),
     (80.,-40.,50.,70.),
     0.2,0, stiff.LEFT_SIDE_KICK_STIFFNESSES),

    ((80.,70.,-10.,-10.),
     (0.,23.,-13.,20.,-10.,-25.),
     (0.,20.,-13.,20.,-10.,-25.),
     (80.,-40.,50.,70.),
     0.8,0, stiff.LEFT_SIDE_KICK_STIFFNESSES),

    ((80.,90.,-50.,-70.),
     (0.,0.,-10.,20.,-10.,0.),
     (0.,0.,-10.,20.,-10.,0.),
     (80.,-40.,50.,70.),
     2.0,0, stiff.LEFT_SIDE_KICK_STIFFNESSES) )


RIGHT_SIDE_KICK = (
    ((80.0 , 40.0 , -50.0 , -70.0),
     (0.0 , -0.0 , -10.0 , 20.0 , -10.0 , -0.0),
     (0.0 , -0.0 , -10.0 , 20.0 , -10.0 , -0.0),
     (80.0 , -90.0 , 50.0 , 70.0),
     2.0 , 0, stiff.RIGHT_SIDE_KICK_STIFFNESSES),

    ((80.0 , 40.0 , -50.0 , -70.0),
     (0.0 , -16.0 , -13.0 , 20.0 , -10.0 , 22.0),
     (0.0 , -20.0 , -10.0 , 20.0 , -10.0 , 20.0),
     (80.0 , -40.0 , 50.0 , 70.0),
     2.0 , 0, stiff.RIGHT_SIDE_KICK_STIFFNESSES),

    (3, (0.,-40.,-30.,70.,-32.,20.),0.4 , 0, stiff.RIGHT_SIDE_KICK_STIFFNESSES),
    (3, (0.,-40.,-50.,30.,20.,0.),0.4 , 0, stiff.RIGHT_SIDE_KICK_STIFFNESSES),


    ((80.0 , 40.0 , -50.0 , -70.0),
     (0.0 , -23.0 , -13.0 , 20.0 , -10.0 , 25.0),
     (0.0 , -9.0 , -60.0 , 30.0 , 23.0 , -0.0),
     (80.0 , -70.0 , 10.0 , 10.0),
     0.2 , 0, stiff.RIGHT_SIDE_KICK_STIFFNESSES),

    ((80.0 , 40.0 , -50.0 , -70.0),
     (0.0 , -20.0 , -13.0 , 20.0 , -10.0 , 25.0),
     (0.0 , -23.0 , -13.0 , 20.0 , -10.0 , 25.0),
     (80.0 , -70.0 , 10.0 , 10.0),
     0.8 , 0, stiff.RIGHT_SIDE_KICK_STIFFNESSES),

    ((80.0 , 40.0 , -50.0 , -70.0),
     (0.0 , 0.0 , -10.0 , 20.0 , -10.0 , -0.0),
     (0.0 , 0.0 , -10.0 , 20.0 , -10.0 , -0.0),
     (80.0 , -90.0 , 50.0 , 70.0),
     2.0 , 0, stiff.RIGHT_SIDE_KICK_STIFFNESSES) )

LEFT_LONG_BACK_KICK = (

    # lean right
    ((80.,40.,-50.,-70.),
     (0.,0.,-35.,65.,-28.,-14.),
     (0.,2,-38.,76.,-40.,-14.),
     (80.,-40.,50.,70.),
     0.8,0, stiff.NORMAL_STIFFNESSES),

    #lift left leg to side
    ((80.,0.5,-50.,-70.),
     (0.,45.,-35.,65.,-10.,-10.),
     (0.,4.5,-38.,76.,-40.,-22.),
     (80.,-40.,50.,70.),
     0.5 , 0, stiff.LEFT_SIDE_KICK_STIFFNESSES),

    #Swing around Leg
    ((80.,22,-75.,-86.),
     (0.,45.,-95.,25.,-10.,-10.),
     (0.,4.5,-38.,76.,-40.,-16.5),
     (80.,-40.,50.,70.),
     0.7 , 0, stiff.LEFT_SIDE_KICK_STIFFNESSES),

    #  #Set For Kick
    ((80.,22,-75.,-86.),
     (0.,14.,-95.,20.,-68.,-10.),
     (0.,5.,-38.,76.,-40.,-16.5),
     (80.,-40.,50.,70.),
     0.4 , 0, stiff.LEFT_SIDE_KICK_STIFFNESSES),

     # KICK!!!
    ((80.,22,-75.,-86.),
     (0.,13.,-50.,65.,-16.,-10.),
     (0.,5.,-38.,76.,-40.,-16.5),
     (80.,-40.,50.,70.),
     .2 , 0, stiff.LEFT_SIDE_KICK_STIFFNESSES),

    # # #Lift left leg up a bit
    # ((90.,50.,-50.,-40.),
    #  (0.,42.,-55.,75.,-20.,5.),
    #  (0.,8,-9.,16.,-10.,-20.),
    #  (110.,-40.,60.,30.),
    #  200.8,0, stiff.NORMAL_STIFFNESSES),

    # # #Swing leg around (part uno)
    # ((100.,60.,-50.,-30.),
    #  (0.,29.,-72.,35.,-13.,23.),
    #  (0.,9,-9.,20.,-10.,-20.),
    #  (105.,-40.,60.,30.),
    #  200.8,0, stiff.BACK_KICK_STIFFNESSES),

    #  #Extend and lower the foot
    # ((110.,70.,-50.,-20.),
    #  (0.,15.,-85.,5.,-13.,-20.),
    #  (0.,14.5,-20.,58.,-23.,-20.),
    #  (80.,-45.,50.,70.),
    #  5,0, stiff.BACK_KICK_STIFFNESSES),

    #  #Kick!!!
    # ((80.,56.,-50.,-70.),
    #  (0.,12.,-62.,50.,-13.,-20.),
    #  (0.,15,-30.,58.,-32.,-20.),
    #  (80.,-45.,50.,70.),
    #  .2,0, stiff.LEFT_FAR_KICK_STIFFNESSES),

    # recover
    ((80.,40.,-50.,-70.),
     (0.,0.,-35.,65.,-28.,-10.),
     (0.,2,-38.,76.,-44.,-14.),
     (90.,-40.,50.,70.),
    .8,0, stiff.NORMAL_STIFFNESSES))


RIGHT_LONG_BACK_KICK =  mirrorMove(LEFT_LONG_BACK_KICK)

LEFT_BIG_KICK = (
    #swing to the right
    ((80.,40.,-50.,-70.),
     (0.,0.,-15.,20.,-10.,0.),
     (0.,0.,-15.,20.,-10.,0.),
     (80.,-40.,50.,70.),
     0.8,0, stiff.NORMAL_STIFFNESSES),

    ((80.,40.,-50.,-70.),
     (0.,15.,-10.,20.,-10.,-20.),
     (0.,15.,-10.,20.,-10.,-20.),
     (80.,-40.,50.,70.),
     0.8,0, stiff.NORMAL_STIFFNESSES),

    # Lift leg leg
    ((80.,40.,-50.,-70.),
     (0.,15.,-45.,85.,-40.,-20.),
     (0.,15.,-12.,16.,-10.,-20.),
     (80.,-40.,50.,70.),
     0.8,0, stiff.NORMAL_STIFFNESSES),

    #cock left leg
    ((20.,40.,0.,0.),
     (0.,15.,-22.,120.,0.,-20.),
     (0.,15.,-12.,16.,-10.,-20.),
     (100.,-40.,0.,0.),
     0.8,0, stiff.NORMAL_STIFFNESSES),

    # kick left leg
    ((50.,40.,0.,0.),
     (0.,15.,-80.,60.,-20.,-20.),
     (0.,15.,-22.,30.,-10.,-20.),
     (20.,-40.,0.,0.),
     0.18,0, stiff.LEFT_FAR_KICK_STIFFNESSES),

    # unkick foot
    ((80.,40.,-50.,-70.),
     (0.,15.,-45.,85.,-40.,-10.),
     (0.,15.,-12.,16.,-10.,-20.),
     (80.,-40.,50.,70.),
     1.5,0, stiff.LEFT_FAR_KICK_STIFFNESSES),

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
     (80.,-40.,50.,70.),
     1.0,0, stiff.LOW_HEAD_STIFFNESSES)
    )

RIGHT_BIG_KICK = mirrorMove(LEFT_BIG_KICK)

SHORT_QUICK_LEFT_KICK = (
    ((90,15,0,0),
     (0.,0.,-15.,20.,-10.,0.),
     (0.,0.,-15.,20.,-10.,0.),
     (90,-15,0,0),
     0.4,0, stiff.LOW_HEAD_STIFFNESSES),

    ((90,15,0,0),
     (0.,15.,-25.,50.,-20.,-20.),
     (0.,17.,-15.,20.,-10.,-20.),
     (90,-15,0,0),
     0.8,0, stiff.LOW_HEAD_STIFFNESSES),

    ((90,15,0,0),
     (0.,15.,-45.,20.,-20.,-20.),
     (0.,16.,-15.,20.,-8.,-20.),
     (90,-15,0,0),
     0.08,0, stiff.LEFT_FAR_KICK_STIFFNESSES),

    ((90,15,0,0),
     (0.,15.,-25.,50.,-25.,-20.),
     (0.,15.,-10.,20.,-10.,-20.),
     (90,-15,0,0),
     0.3,0, stiff.LOW_HEAD_STIFFNESSES),

    ((90,15,0,0),
     (0.,0.,-15.,20.,-10.,0.),
     (0.,0.,-15.,20.,-10.,0.),
     (90,-15,0,0),0.6,0, stiff.LOW_HEAD_STIFFNESSES),
    )

SHORT_QUICK_RIGHT_KICK = mirrorMove(SHORT_QUICK_LEFT_KICK)

SHORT_LEFT_SIDE_KICK = (
    ((80.,90.,-50.,-70.),
     (0.,0.,-10.,20.,-10.,0.),
     (0.,0.,-10.,20.,-10.,0.),
     (80.,-40.,50.,70.),
     2.0,0, stiff.LEFT_SIDE_KICK_STIFFNESSES),

    ((80.,40.,-50.,-70.),
     (0.,20.,-10.,20.,-10.,-20.),
     (0.,16.,-13.,20.,-10.,-22.),
     (80.,-40.,50.,70.),
     2.0,0, stiff.LEFT_SIDE_KICK_STIFFNESSES),

    (2, (0.,40.,-30.,70.,-32.,-20.), 0.4 , 0, stiff.LEFT_SIDE_KICK_STIFFNESSES),
    (2, (0.,40.,-50.,30.,20.,0.), 0.4 , 0, stiff.LEFT_SIDE_KICK_STIFFNESSES),

    ((80.,70.,-10.,-10.),
     (0.,26.,-60.,30.,23.,0.),
     (0.,23.,-13.,20.,-10.,-25.),
     (80.,-40.,50.,70.),
     0.2,0, stiff.LEFT_SIDE_KICK_STIFFNESSES),

    ((80.,70.,-10.,-10.),
     (0.,23.,-13.,20.,-10.,-25.),
     (0.,20.,-13.,20.,-10.,-25.),
     (80.,-40.,50.,70.),
     0.8,0, stiff.LEFT_SIDE_KICK_STIFFNESSES),

    ((80.,90.,-50.,-70.),
     (0.,0.,-10.,20.,-10.,0.),
     (0.,0.,-10.,20.,-10.,0.),
     (80.,-40.,50.,70.),
     2.0,0, stiff.LEFT_SIDE_KICK_STIFFNESSES) )

SHORT_RIGHT_SIDE_KICK = (
    ((80.0, 40.0, -50.0, -70.0),
     (0.0, -0.0, -10.0, 20.0, -10.0, -0.0),
     (0.0, -0.0, -10.0, 20.0, -10.0, -0.0),
     (80.0, -90.0, 50.0, 70.0),
     2.0, 0, stiff.RIGHT_SIDE_KICK_STIFFNESSES ),

    ((80.0, 40.0, -50.0, -70.0),
     (0.0, -16.0, -13.0, 20.0, -10.0, 22.0),
     (0.0, -20.0, -10.0, 20.0, -10.0, 20.0),
     (80.0, -40.0, 50.0, 70.0),
     2.0, 0, stiff.RIGHT_SIDE_KICK_STIFFNESSES ),

    (3, (0.,-40.,-30.,70.,-32.,20.), 0.4 , 0, stiff.LEFT_SIDE_KICK_STIFFNESSES),
    (3, (0.,-40.,-50.,30.,20.,0.), 0.4 , 0, stiff.LEFT_SIDE_KICK_STIFFNESSES),

    ((80.0, 40.0, -50.0, -70.0),
     (0.0, -23.0, -13.0, 20.0, -10.0, 25.0),
     (0.0, -26.0, -60.0, 30.0, 23.0, -0.0),
     (80.0, -70.0, 10.0, 10.0),
     0.2, 0, stiff.RIGHT_SIDE_KICK_STIFFNESSES),

    ((80.0, 40.0, -50.0, -70.0),
     (0.0, -20.0, -13.0, 20.0, -10.0, 25.0),
     (0.0, -23.0, -13.0, 20.0, -10.0, 25.0),
     (80.0, -70.0, 10.0, 10.0),
     0.8, 0, stiff.RIGHT_SIDE_KICK_STIFFNESSES ),

    ((80.0, 40.0, -50.0, -70.0),
     (0.0, -0.0, -10.0, 20.0, -10.0, -0.0),
     (0.0, -0.0, -10.0, 20.0, -10.0, -0.0),
     (80.0, -90.0, 50.0, 70.0),
     2.0, 0, stiff.RIGHT_SIDE_KICK_STIFFNESSES))

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
