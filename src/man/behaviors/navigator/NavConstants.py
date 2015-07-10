from . import BrunswickSpeeds as speeds

# orbitPoint values
ORBIT_LEFT = -1               # Spin right to orbit left
ORBIT_RIGHT = 1               # Spin left to orbit right
MIN_ORBIT_ANGLE = 10
# @TODO: maybe make this a parameter of goTo?
ADAPT_DISTANCE = 120.0

ZERO_SPEEDS = (0.0, 0.0, 0.0)

ODOM_CLOSE_ENOUGH = 15.0

# Obstacle avoidance stuff
AVOID_OBSTACLE_FRONT_DIST = 40.0 #cm
AVOID_OBSTACLE_SIDE_DIST = 30.0 #cm
AVOID_OBSTACLE_FRAMES_THRESH = 2
DONE_AVOIDING_FRAMES_THRESH = 10
DODGE_BACK_SPEED = speeds.REV_MAX_SPEED
DODGE_RIGHT_SPEED = speeds.RIGHT_MAX_SPEED
DODGE_LEFT_SPEED = speeds.LEFT_MAX_SPEED
ZONE_WIDTH = 180. / (2. * 4.) # 4 directions per PI
DODGE_DIST = 80.0 #cm

# variable speed chase ball
SLOW_CHASE_DIST = 100.
PREPARE_FOR_KICK_DIST = 50.

# relative speed to walk for an obstacle and its name
OBS_DICT = {
                1 : (0.5,1, "North"),
                2 : (0.5,1, "Northeast"),
                3 : (0,1, "East"),
                4 : (0.5,1, "Southeast"),
                5 : (0.5,-1, "South"),
                6 : (0.5,-1, "Southwest"),
                7 : (0,-1, "West"),
                8 : (0.5,-1, "Northwest")
            }