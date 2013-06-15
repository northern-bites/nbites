from . import BrunswickSpeeds as speeds

# orbitPoint values
ORBIT_LEFT = -1               # Spin right to orbit left
ORBIT_RIGHT = 1               # Spin left to orbit right
MIN_ORBIT_ANGLE = 10
# @TODO: maybe make this a parameter of goTo?
ADAPT_DISTANCE = 60.0

ZERO_SPEEDS = (0.0, 0.0, 0.0)

# Obstacle avoidance stuff
AVOID_OBSTACLE_FRONT_DIST = 40.0 #cm
AVOID_OBSTACLE_SIDE_DIST = 30.0 #cm
AVOID_OBSTACLE_FRAMES_THRESH = 2
DONE_AVOIDING_FRAMES_THRESH = 10
DODGE_BACK_SPEED = speeds.REV_MAX_SPEED
DODGE_RIGHT_SPEED = speeds.RIGHT_MAX_SPEED
DODGE_LEFT_SPEED = speeds.LEFT_MAX_SPEED
