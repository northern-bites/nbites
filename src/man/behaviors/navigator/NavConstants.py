from . import BrunswickSpeeds as speeds

# orbitPoint values
ORBIT_LEFT = -1               # Spin right to orbit left
ORBIT_RIGHT = 1               # Spin left to orbit right
MIN_ORBIT_ANGLE = 10
# @TODO: maybe make this a parameter of goTo?
ADAPT_DISTANCE = 120.0

ZERO_SPEEDS = (0.0, 0.0, 0.0)

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

DGE_DESTS = ((40, 0, 0),            # Dodging to NORTH
             (30, -50, 0),            # TO NORTHEAST
             (0, -40, 0),            # TO EAST
             (-30, -50, 0),            # TO SOUTHEAST
             (-40, 0, 0),           # TO SOUTH
             (-30, 50, 0),             # TO SOUTHWEST
             (0, 40, 0),             # TO WEST
             (30, 50, 0)              # TO NORTHWEST
)